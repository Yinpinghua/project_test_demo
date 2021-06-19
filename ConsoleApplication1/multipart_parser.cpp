#include "multipart_parser.h"

multipart_parser::multipart_parser(const std::string& boundary_str)
	:boundary_(boundary_str)
{
	boundary_len_ = strlen(boundary_.c_str());
}

size_t multipart_parser::parser_data(const char* data, size_t size)
{
	const char* mark;
	const char* p;
	unsigned char c;

	for (p = data; p < data + size; ++p) {
		c = *p;

	reexecute:
		switch (static_cast<state>(state_)) {

		case state::s_preamble:
			if (c == HYPHEN)
				state_ = static_cast<uint16_t>(state::s_preamble_hy_hy);
			// else ignore everything before first boundary
			break;
		case state::s_preamble_hy_hy:
			if (c == HYPHEN) {
				state_ = static_cast<uint16_t>(state::s_first_boundary);
			}else {
				state_ = static_cast<uint16_t>(state::s_preamble);
			}
			break;

		case state::s_first_boundary:
			if (index_ == boundary_len_) {
				if (c != CR)
					goto error;
				index_++;
				break;
			}
			if (index_ == boundary_len_ + 1) {
				if (c != LF)
					goto error;
				CALLBACK_NOTIFY(body_begin);
				CALLBACK_NOTIFY(part_begin);
				index_ = 0;
				state_ = static_cast<uint16_t>(state::s_header_field_start);
				break;
			}
			if (c == boundary_[index_]) {
				index_++;
				break;
			}
			goto error;

		case state::s_header_field_start:
			if (c == CR) {
				state_ = static_cast<uint16_t>(state::s_headers_done);
				break;
			}
			state_ = static_cast<uint16_t>(state::s_header_field);
			// fallthrough;

		case state::s_header_field:
			mark = p;
			while (p != data + size) {
				c = *p;
				if (g_header_field_chars[c] == 0)
					break;
				++p;
			}
			if (p > mark) {
				CALLBACK_DATA(header_field, mark, p - mark);
			}
			if (p == data + size) {
				break;
			}
			if (c == ':') {
				state_ = static_cast<uint16_t>(state::s_header_value_start);
				break;
			}
			goto error;

		case state::s_header_value_start:
			if (c == SP || c == HT) {
				break;
			}
			state_ = static_cast<uint16_t>(state::s_header_value);
			// fallthrough;

		case state::s_header_value:
			mark = p;
			while (p != data + size) {
				c = *p;
				if (c == CR) {
					state_ = static_cast<uint16_t>(state::s_header_value_cr);
					break;
				}
				++p;
			}
			if (p > mark) {
				CALLBACK_DATA(header_value, mark, p - mark);
			}
			break;

		case state::s_header_value_cr:
			if (c == LF) {
				state_ = static_cast<uint16_t>(state::s_header_field_start);
				break;
			}
			goto error;

		case state::s_headers_done:
			if (c == LF) {
				CALLBACK_NOTIFY(headers_complete);
				state_ = static_cast<uint16_t>(state::s_data);
				break;
			}
			goto error;

		case state::s_data:
			mark = p;
			while (p != data + size) {
				c = *p;
				if (c == CR) {
					state_ = static_cast<uint16_t>(state::s_data_cr);
					break;
				}
				++p;
			}
			if (p > mark) {
				CALLBACK_DATA(data, mark, p - mark);
			}
			break;

		case state::s_data_cr:
			if (c == LF) {
				state_ = static_cast<uint16_t>(state::s_data_cr_lf);
				break;
			}
			CALLBACK_DATA(data, "\r", 1);
			state_ = static_cast<uint16_t>(state::s_data);
			goto reexecute;

		case state::s_data_cr_lf:
			if (c == HYPHEN) {
				state_ = static_cast<uint16_t>(state::s_data_cr_lf_hy);
				break;
			}
			CALLBACK_DATA(data, "\r\n", 2);
			state_ = static_cast<uint16_t>(state::s_data);
			goto reexecute;

		case state::s_data_cr_lf_hy:
			if (c == HYPHEN) {
				state_ = static_cast<uint16_t>(state::s_data_boundary_start);
				break;
			}
			CALLBACK_DATA(data, "\r\n-", 3);
			state_ = static_cast<uint16_t>(state::s_data);
			goto reexecute;

		case state::s_data_boundary_start:
			index_ = 0;
			state_ = static_cast<uint16_t>(state::s_data_boundary);
			// fallthrough;

		case state::s_data_boundary:
			if (index_ == boundary_len_) {
				index_ = 0;
				state_ = static_cast<uint16_t>(state::s_data_boundary_done);
				goto reexecute;
			}
			if (c == boundary_[index_]) {
				index_++;
				break;
			}
			CALLBACK_DATA(data,boundary_.c_str(),index_);
			state_ = static_cast<uint16_t>(state::s_data);
			goto reexecute;

		case state::s_data_boundary_done:
			if (c == CR) {
				state_ = static_cast<uint16_t>(state::s_data_boundary_done_cr_lf);
				break;
			}
			if (c == HYPHEN) {
				state_ = static_cast<uint16_t>(state::s_data_boundary_done_hy_hy);
				break;
			}
			goto error;

		case state::s_data_boundary_done_cr_lf:
			if (c == LF) {
				CALLBACK_NOTIFY(part_end);
				CALLBACK_NOTIFY(part_begin);
				state_ = static_cast<uint16_t>(state::s_header_field_start);
				break;
			}
			goto error;

		case state::s_data_boundary_done_hy_hy:
			if (c == HYPHEN) {
				CALLBACK_NOTIFY(part_end);
				CALLBACK_NOTIFY(body_end);
				state_ = static_cast<uint16_t>(state::s_epilogue);
				break;
			}
			goto error;

		case state::s_epilogue:
			// Must be ignored according to rfc 1341.
			break;
		}
	}
	return size;

error:
	return p - data;
}

std::map<std::string, std::string> multipart_parser::get_multipart_data()
{
	std::map<std::string, std::string>multipart_datas;
	auto iter_begin = parts_.begin();
	for (;iter_begin != parts_.end();++iter_begin){
		std::string key;
		auto iter_find = iter_begin->headers.find("Content-Disposition");
		if (iter_find != iter_begin->headers.end()){
			std::string value = iter_find->second;
			auto beg_pos = value.find("\"");
			if (beg_pos  != std::string::npos){
				beg_pos++;
				auto end_pos = value.rfind("\"");
				if (end_pos != std::string::npos) {
					key = value.substr(beg_pos, end_pos - beg_pos);
				}
			}
		}
		std::string value = iter_begin->body;
		multipart_datas.emplace(key, value);
	}

	return std::move(multipart_datas);
}
