#include "rdc_fs_watcher.h"

watch_info::watch_info(const int64_t& rId,
	std::unique_ptr<OVERLAPPED>&& overlapped,
	const std::wstring& path, handle_ptr&& dirHandle)
	:rId_(rId)
	, state_(watch_state::initialized)
	, notif_buffer_()
	, overlapped_(std::move(overlapped))
	, path_(path)
	, directory_(std::move(dirHandle))

{

}

watch_info::~watch_info()
{
	if (state_ == watch_state::listening) {
		std::cerr << "Destructing a listening WatchInfo" << std::endl;
	}
}

//返回的是绝对路径
void watch_info::try_to_getlong_name(std::wstring& path_name) const
{
	const std::wstring full_path = path_ + L"/" + path_name;
	const unsigned long long_size = GetLongPathNameW(full_path.c_str(), NULL, 0);
	std::wstring long_path_name;
	long_path_name.resize(long_size);
	const unsigned long ret_val = GetLongPathNameW(
		full_path.c_str(), &long_path_name[0], static_cast<unsigned long>(long_path_name.size()));
	if (ret_val == 0) {
		return;
	}

	while (!long_path_name.empty() && long_path_name.back() == L'\0') {
		long_path_name.pop_back();
	}

	if (long_path_name.find(path_) != std::wstring::npos ||
		path_.size() >= long_path_name.size()) {
		return;
	}

	//std::wstring long_name = long_path_name.substr(this->path_.size() + 1);
	//if (long_name.empty()) {
	//	return;
	//}

	path_name = long_path_name;
}

void watch_info::process_notification(const FILE_NOTIFY_INFORMATION& not_if,
	std::set<std::pair<std::wstring, uint32_t>>& notifications) const
{
	std::wstring path_name(
		not_if.FileName, not_if.FileName + (not_if.FileNameLength / sizeof(not_if.FileName)));
	if (not_if.Action != FILE_ACTION_REMOVED && not_if.Action != FILE_ACTION_RENAMED_OLD_NAME) {
		try_to_getlong_name(path_name);
	}

	notifications.emplace(path_name, not_if.Action);
}

bool watch_info::listen()
{
	if (state_ != watch_state::initialized) {
		std::cerr << "Invalid state." << std::endl;
		return false;
	}

	if (overlapped_ == nullptr) {
		std::cerr << "invalid state" << std::endl;
		return false;
	}

	constexpr unsigned long flags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
		| FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION
		| FILE_NOTIFY_CHANGE_SECURITY;

	const bool res = ReadDirectoryChangesW(directory_.get(), &notif_buffer_,
		static_cast<unsigned long>(sizeof(notif_buffer_)), true, flags,
		nullptr, overlapped_.get(), nullptr);

	if (!res) {
		state_ = watch_state::initialized;
		std::cerr << "An error has occurred: " << GetLastError() << std::endl;
		return false;
	}

	state_ = watch_state::listening;
	return true;
}

void watch_info::stop()
{
	if (state_ == watch_state::listening) {
		state_ = watch_state::pending_close;
	}

	directory_.reset(INVALID_HANDLE_VALUE);
}

std::set<std::pair<std::wstring, uint32_t>> watch_info::process_notifications() const
{
	std::set<std::pair<std::wstring, uint32_t>> notifications;

	auto not_inf = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(&notif_buffer_);
	for (bool more_not_if = true; more_not_if; more_not_if = not_inf->NextEntryOffset > 0,
		not_inf = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(
			reinterpret_cast<const char*>(not_inf) + not_inf->NextEntryOffset)) {
		this->process_notification(*not_inf, notifications);
	}

	return std::move(notifications);
}

rdc_fs_watcher::rdc_fs_watcher()
	:stopped_(false)
{
	iocp_.reset(CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 1));
	if (!iocp_) {
		throw std::runtime_error("Error when creating IOCP.");
	}

	loop_thread_ = std::thread([this]() { this->event_loop(); });
}

rdc_fs_watcher::~rdc_fs_watcher()
{
	stop_event_loop();

	if (loop_thread_.joinable()) {
		loop_thread_.join();
	}
}

void rdc_fs_watcher::stop_event_loop()
{
	{
		std::lock_guard<std::mutex>lock(watch_info_mutex_);

		for (auto& info : watch_infos_) {
			info.second.stop();
		}
	}

	// send stop "magic packet"
	if (this->iocp_.get() != INVALID_HANDLE_VALUE) {
		PostQueuedCompletionStatus(this->iocp_.get(), 0, reinterpret_cast<ULONG_PTR>(this), nullptr);
	}
}

void rdc_fs_watcher::set_change_event(const ChangeEvent& event)
{
	change_event_ = event;
}

void rdc_fs_watcher::set_error_event(const ErrorEvent& event)
{
	error_event_ = event;
}

bool rdc_fs_watcher::add_directory(int64_t id, const std::wstring& path)
{
	if (stopped_) {
		std::cerr << "Watcher thread is not running." << std::endl;
		return false;
	}

	handle_ptr dir_handle(CreateFileW(path.c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL));

	if (dir_handle.get() == INVALID_HANDLE_VALUE) {
		std::cerr << "Cannot create directory handle: " << GetLastError() << std::endl;
		return false;
	}
	// check if it is even a directory:
	{
		BY_HANDLE_FILE_INFORMATION file_info{};
		const bool res = GetFileInformationByHandle(dir_handle.get(), &file_info);
		if (!res) {
			return false;
		}
		else if (!(file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::cerr << "Not a directory." << std::endl;
			return false;
		}
	}

	// the "old" IOCP handle should not be freed, because:
	// https://devblogs.microsoft.com/oldnewthing/20130823-00/?p=3423
	if (!CreateIoCompletionPort(dir_handle.get(), this->iocp_.get(), NULL, 1)) {
		std::cerr << "Cannot create IOCP: " << GetLastError() << std::endl;
		return false;
	}

	// create the internal data structures, and set up listening with RDC():
	{
		auto ov = std::make_unique<OVERLAPPED>();
		OVERLAPPED* const ov_ptr = ov.get();
		{
			std::lock_guard<std::mutex> lock(this->watch_info_mutex_);
			const auto info = this->watch_infos_.emplace(std::piecewise_construct, std::forward_as_tuple(ov_ptr),
				std::forward_as_tuple(id, std::move(ov), path, std::move(dir_handle)));
			if (!info.second) {
				return false;
			}

			if (stopped_) {
				watch_infos_.erase(info.first);
				return false;
			}

			bool result = info.first->second.listen();
			if (!result) {
				watch_infos_.erase(info.first);
				return false;
			}
		}
	}

	return true;
}

void rdc_fs_watcher::remove_directory(int64_t id)
{
	std::lock_guard<std::mutex>lock(watch_info_mutex_);

	auto iter_find = std::find_if(watch_infos_.begin(), watch_infos_.end(),
		[id](decltype(watch_infos_)::const_reference value) {
			return value.second.get_file_id() == id;
		});

	if (iter_find != watch_infos_.end()) {
		iter_find->second.stop();
	}
}

void rdc_fs_watcher::event_loop()
{
	DWORD num_of_bytes = 0;
	OVERLAPPED* ov = nullptr;
	ULONG_PTR comp_key = 0;
	bool res = false;
	while ((res = GetQueuedCompletionStatus(this->iocp_.get(),
		&num_of_bytes, &comp_key, &ov, INFINITE))) {
		if (comp_key != 0 &&
			comp_key == reinterpret_cast<ULONG_PTR>(this)) {
			// stop "magic packet" was sent, so we shut down:
			break;
		}

		process_event(num_of_bytes, ov);
	}

	stopped_.store(true);
	if (res) {
		// IOCP is intact, so we clean up outstanding calls:
		std::lock_guard<std::mutex> lock(watch_info_mutex_);
		const auto is_pending = [&lock, this]() {
			auto iter_find =
				std::find_if(watch_infos_.cbegin(), watch_infos_.cend(),
					[](const auto& value) {
						return value.second.get_watch_state() == watch_state::pending_close;
					});

			return iter_find != watch_infos_.cend();
		};

		while (is_pending()
			&& (res = GetQueuedCompletionStatus(
				iocp_.get(), &num_of_bytes, &comp_key, &ov, INFINITE))) {
			const auto iter_find = watch_infos_.find(ov);
			if (iter_find != watch_infos_.end()) {
				iter_find->second.set_watch_state(watch_state::pending_close);
				watch_infos_.erase(iter_find);
			}
		}

		return;
	}

	const unsigned int error_code = static_cast<unsigned int>(GetLastError());
	std::cerr << "There is something wrong with the IOCP: " << error_code << std::endl;
	// alert all subscribers that they will not receive events from now on:
	std::lock_guard<std::mutex> lock(watch_info_mutex_);
	for (auto& watchInfo : watch_infos_) {
		error_event_(watchInfo.second.get_file_id());
	}
}

void rdc_fs_watcher::process_event(DWORD trans_bytes, OVERLAPPED* overlapped)
{
	std::lock_guard<std::mutex> lock(watch_info_mutex_);
	// initialization:
	const auto iter_find = watch_infos_.find(overlapped);
	if (iter_find == watch_infos_.end()) {
		std::cerr << "WatchInfo was not found for filesystem event." << std::endl;
		return;
	}

	if (iter_find->second.get_watch_state() == watch_state::listening) {
		iter_find->second.set_watch_state(watch_state::initialized);
	}

	// actual logic:
	if (trans_bytes == 0) {
		if (iter_find->second.get_watch_state() == watch_state::pending_close) {
			// this is the "closing" notification, se we clean up:
			watch_infos_.erase(iter_find);
			return;
		}

		error_event_(iter_find->second.get_file_id());
		return;
	}

	watch_info& info = iter_find->second;

	// If we're already in PendingClose state, and receive a legitimate notification, then
	// we don't emit a change notification, however, we delete the WatchInfo, just like when
	// we receive a "closing" notification.

	if (info.is_run()) {
		auto result = info.process_notifications();

		if (!result.empty()) {
			change_event_(info.get_file_id(), std::move(result));
		}

		if (!info.listen()) {
			error_event_(info.get_file_id());
			watch_infos_.erase(iter_find);
		}

		return;
	}

	watch_infos_.erase(iter_find);
}

