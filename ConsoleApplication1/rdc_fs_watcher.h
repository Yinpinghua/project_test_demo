#ifndef rdc_fs_watcher_h__
#define rdc_fs_watcher_h__

#include <memory>
#include <functional>
#include <atomic>
#include <set>
#include <iostream>
#include <windows.h>
#include <mutex>
#include <unordered_map>

//这样的写法,指定这个枚举占1字节
enum class watch_state : uint8_t
{
	initialized,    // No outstanding RDC() call.
	listening,      // RDC() call was made, and we're waiting for changes.
	pending_close,  // Directory handle was closed, and we're waiting for the "closing"
					// notification on IOCP.
					// Most of the time this is an "empty" notification, but sometimes it is a
					// legitimate notification about a change. This is behavior is not documented
					// explicitly.
};

enum class file_action :uint8_t
{
	file_add = FILE_ACTION_ADDED,
	file_move = FILE_ACTION_REMOVED,
	file_modified = FILE_ACTION_MODIFIED,
	file_rename_old_name = FILE_ACTION_RENAMED_OLD_NAME,
	file_renmae_new_name = FILE_ACTION_RENAMED_NEW_NAME
};

inline std::string g_file_action_mess[] = { "文件增加","文件移动","文件修改","文件重命名","文件命名" };

class handle_deleter
{
public:
	void operator()(HANDLE handle)
	{
		if (handle != INVALID_HANDLE_VALUE && handle != nullptr) {
			CloseHandle(handle);
		}
	}
};

using handle_ptr = std::unique_ptr<std::remove_pointer<HANDLE>::type, handle_deleter>;

class watch_info final
{
public:
	watch_info(const int64_t& rId, std::unique_ptr<OVERLAPPED>&& overlapped,
		const std::wstring& path, handle_ptr&& dirHandle);

	~watch_info();
	watch_info(const watch_info&) = delete;
	watch_info& operator=(const watch_info&) = delete;
	watch_info(watch_info&&) = delete;
	watch_info& operator=(watch_info&&) = delete;

	int64_t get_file_id()const
	{
		return rId_;
	}

	const std::atomic<watch_state>& get_watch_state()const
	{
		return state_;
	}

	void set_watch_state(const watch_state& state)
	{
		state_ = state;
	}

	bool listen();
	bool is_run() const
	{
		return this->state_ != watch_state::pending_close;
	}
	void stop();
	std::set<std::pair<std::wstring, uint32_t>> process_notifications() const;
private:
	void process_notification(
		const FILE_NOTIFY_INFORMATION& notIf,
		std::set<std::pair<std::wstring, uint32_t>>& notifications) const;
	void try_to_getlong_name(std::wstring& pathName) const;
private:
	std::aligned_storage_t<64 * 1024, sizeof(unsigned long)> notif_buffer_;
	static_assert(
		sizeof(watch_info::notif_buffer_) <= 64 * 1024, "Must be smaller than RDC()'s network limit!");
	static_assert(sizeof(watch_info::notif_buffer_) >= (sizeof(FILE_NOTIFY_INFORMATION) + (1 << 15)),
		"Must be able to store a long path.");

	std::wstring path_;
	const int64_t rId_;
	std::atomic<watch_state> state_;
	std::unique_ptr<OVERLAPPED> overlapped_{};
	handle_ptr directory_;
};

class rdc_fs_watcher final
{
	using ChangeEvent = std::function<void(int64_t,
		const std::set<std::pair<std::wstring, uint32_t>>&)>;
	using ErrorEvent = std::function<void(int64_t)>;
public:
	rdc_fs_watcher();
	~rdc_fs_watcher();
	void set_change_event(const ChangeEvent& event);
	void set_error_event(const ErrorEvent& event);
	bool add_directory(int64_t id, const std::wstring& path);
	void remove_directory(int64_t id);
private:
	void event_loop();
	void stop_event_loop();
	void process_event(DWORD trans_bytes, OVERLAPPED* overlapped);
private:
	std::atomic<bool> stopped_;
	ChangeEvent change_event_;
	ErrorEvent error_event_;
	std::thread loop_thread_;
	std::unordered_map<OVERLAPPED*, watch_info> watch_infos_;
	std::mutex watch_info_mutex_;
	handle_ptr iocp_;
};
#endif // rdc_fs_watcher_h__
