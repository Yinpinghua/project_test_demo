/*
 * Binary Protocol Serialize and Parse Library, Version 1.2.2,
 * Copyright (C) 2012-2014, Ren Bin (ayrb13@gmail.com)
 *
 * This library is free software. Permission to use, copy, modify,
 * and/or distribute this software for any purpose with or without fee
 * is hereby granted, provided that the above copyright notice and
 * this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * a.k.a. as Open BSD license
 * (http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/share/misc/license.template)
 *
 * You can get latest version of this library from github
 * (https://github.com/ayrb13/binproto)
 */

#ifndef __BINPROTO_HPP__
#define __BINPROTO_HPP__

#include <stdint.h>
#include <assert.h>
#include <string>
#include <vector>

#if defined(_WIN32) || defined(WIN32)

#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
 //warning C4244: 'argument' : conversion from 'const uint64_t' to 'u_long', possible loss of data
#pragma warning(disable : 4290)
//warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#pragma warning(disable : 4244)

#else
#include <arpa/inet.h>
#include <string.h>
#endif

 //comma macro
#define BINPROTO_COMMA ,

//static assert;
#define BINPROTO_STATIC_ASSERT(expr,comment) \
	typedef char _binproto_static_assert_type##__LINE__[((expr)?1:-1)];

//true and false type
//only use when overloading
struct _binproto_true
{
};
struct _binproto_false
{
};

//value to type
template<int value>
struct _binproto_bool_value_to_bool_type {
	BINPROTO_STATIC_ASSERT((value != 0) && (value != 1), "_binproto_bool_value_to_bool_type value must be 0 or 1");
};
template<>
struct _binproto_bool_value_to_bool_type<1> {
	typedef _binproto_true type;
};
template<>
struct _binproto_bool_value_to_bool_type<0> {
	typedef _binproto_false type;
};

//is same type
template<typename A, typename B>
struct _binproto_is_same_type
{
	static const int value = 0;
};
template<typename A>
struct _binproto_is_same_type<A, A>
{
	static const int value = 1;
};
#define BINPROTO_IS_SAME_TYPE(A,B) _binproto_is_same_type<A,B>::value

//judge a class is a packet object by SFINAE
template<typename T>
struct _binproto_is_binproto_obj
{
	template<typename U
		, uint32_t(U::*)(char*, uint32_t) const
		, uint32_t(U::*)(const char*, uint32_t)
		, uint32_t(U::*)(std::string&) const
		, uint32_t(U::*)(const std::string&)
		, uint32_t(U::*)(std::vector<char>&) const
		, uint32_t(U::*)(const std::vector<char>&)
		, uint32_t(U::*)() const
	>
		struct _binproto_is_binproto_obj_matcher {};
	template <typename U> static char deduce(_binproto_is_binproto_obj_matcher<U
		, &U::serialize_to_buffer
		, &U::parse_from_buffer
		, &U::serialize_to_buffer
		, &U::parse_from_buffer
		, &U::serialize_to_buffer
		, &U::parse_from_buffer
		, &U::get_binary_len
	>*);
	template <typename U> static int deduce(...);
	enum { value = sizeof(deduce<T>(0)) == sizeof(char) };
};

//max value traits
#if defined(linux) && !defined(__STDC_LIMIT_MACROS)
# error "you should predefine __STDC_LIMIT_MACROS on linux"
#endif

template<typename num_type>
struct _binproto_num_type_traits
{
	BINPROTO_STATIC_ASSERT(
		((BINPROTO_IS_SAME_TYPE(num_type, uint8_t)) ||
			(BINPROTO_IS_SAME_TYPE(num_type, uint16_t)) ||
			(BINPROTO_IS_SAME_TYPE(num_type, uint32_t)) ||
			(BINPROTO_IS_SAME_TYPE(num_type, uint64_t))),
		"typename must be uint8_t or uint16_t or uint32_t or uint64_t");
};
template<> struct _binproto_num_type_traits<uint8_t> {
	static const uint8_t num_type_max = UINT8_MAX;
	static const int num_type_size = 1;
};
template<> struct _binproto_num_type_traits<uint16_t> {
	static const uint16_t num_type_max = UINT16_MAX;
	static const int num_type_size = 2;
};
template<> struct _binproto_num_type_traits<uint32_t> {
	static const uint32_t num_type_max = UINT32_MAX;
	static const int num_type_size = 4;
};
template<> struct _binproto_num_type_traits<uint64_t> {
	static const uint64_t num_type_max = UINT64_MAX;
	static const int num_type_size = 8;
};
#define BINPROTO_UINT_MAX_VALUE(uint_type) (_binproto_num_type_traits<uint_type>::num_type_max)
#define BINPROTO_UINT_SIZE(uint_type) (_binproto_num_type_traits<uint_type>::num_type_size)

//size to type traits
template<int uint_size>
struct _binproto_uint_size_traits { BINPROTO_STATIC_ASSERT(uint_size == 1 || uint_size == 2 || uint_size == 4 || uint_size == 8, "len_size must be 1,2,4,8"); };

template<> struct _binproto_uint_size_traits<1> {
	typedef uint8_t  uint_type;
};
template<> struct _binproto_uint_size_traits<2> {
	typedef uint16_t uint_type;
};
template<> struct _binproto_uint_size_traits<4> {
	typedef uint32_t uint_type;
};
template<> struct _binproto_uint_size_traits<8> {
	typedef uint64_t uint_type;
};
#define BINPROTO_UINT_TYPE_FROM_SIZE(size) typename _binproto_uint_size_traits<size>::uint_type

//assert;
#ifdef BINPROTO_DISABLE_ASSERT
#	define BINPROTO_ASSERT(expr,err_msg) ((void)0)
#else
#	define BINPROTO_ASSERT(expr,err_msg) assert(expr)
#endif

//throw exception;
#define BINPROTO_THROW(err_msg) throw(binproto::exception(err_msg))

//must use try catch mode when parse;
#define _BINPROTO_PARSE_ENSURE(expr,err_msg) \
	if(!(expr)){throw(exception(err_msg));}
#define _BINPROTO_PARSE_TRY \
	try{
#define _BINPROTO_PARSE_CATCH(levelname) \
	}catch(const binproto::exception& ex){ex.throw_to_high_level(levelname);return 0;}

//new serialize and parse on std::string function defination
#define BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
uint32_t serialize_to_buffer(std::vector<char>& vecbuff) const \
{ \
	vecbuff.resize(get_binary_len()); \
	return serialize_to_buffer(&vecbuff[0],vecbuff.size()); \
} \
uint32_t parse_from_buffer(const std::vector<char>& vecbuff) throw(binproto::exception) \
{ \
	return parse_from_buffer(&vecbuff[0],vecbuff.size()); \
} \
uint32_t serialize_to_buffer(std::string& strbuff) const \
{ \
	std::vector<char> vecbuff(get_binary_len()); \
	serialize_to_buffer(&vecbuff[0],vecbuff.size()); \
	strbuff.assign(&vecbuff[0],vecbuff.size()); \
	return vecbuff.size(); \
} \
uint32_t parse_from_buffer(const std::string& strbuff) throw(binproto::exception) \
{ \
	return parse_from_buffer(strbuff.c_str(),strbuff.size()); \
}

namespace binproto
{
	class exception
	{
	public:
		exception(const std::string& comment)
			:_comment(comment)
		{
		}
		exception(const char* comment)
			:_comment(comment)
		{
		}
		const std::string& what() const
		{
			return _comment;
		}
		void throw_to_high_level(const char* levelname) const
		{
			BINPROTO_THROW(_comment + " in " + levelname);
		}
	private:
		std::string _comment;
	};

	template<typename numtype>
	class num_obj
	{
	public:
		typedef numtype uint_type;
		static const int STATIC_BINARY_LENGTH = BINPROTO_UINT_SIZE(numtype);
	public:
		num_obj()
			:_num(0)
		{
		}
		num_obj(numtype num)
			:_num(num)
		{
		}
		num_obj(const num_obj& num)
			:_num(num._num)
		{
		}
		num_obj& operator=(numtype num)
		{
			_num = num;
			return *this;
		}
		num_obj& operator=(const num_obj& num)
		{
			_num = num._num;
			return *this;
		}
		numtype to_int() const
		{
			return _num;
		}
		bool operator<(const num_obj& other) const
		{
			return _num < other._num;
		}
		bool operator==(const num_obj& other) const
		{
			return _num == other._num;
		}
		bool operator>(const num_obj& other) const
		{
			return _num > other._num;
		}
		bool operator!=(const num_obj& other) const
		{
			return _num != other._num;
		}
		inline uint32_t serialize_to_buffer(char* buffer, uint32_t bufflen) const;
		inline uint32_t parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception);
		BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER
			uint32_t get_binary_len() const
		{
			return STATIC_BINARY_LENGTH;
		}
	private:
		numtype _num;
	};

	typedef num_obj<uint8_t> uint8_obj;
	typedef num_obj<uint16_t> uint16_obj;
	typedef num_obj<uint32_t> uint32_obj;
	typedef num_obj<uint64_t> uint64_obj;

	template<>
	inline uint32_t uint8_obj::parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
	{
		_BINPROTO_PARSE_ENSURE(bufflen >= STATIC_BINARY_LENGTH, "uint8_obj parse error");
		_num = buffer[0];
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint8_obj::serialize_to_buffer(char* buffer, uint32_t bufflen) const
	{
		BINPROTO_ASSERT(bufflen >= STATIC_BINARY_LENGTH, "uint8_obj serialize error");
		buffer[0] = _num;
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint16_obj::parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
	{
		_BINPROTO_PARSE_ENSURE(bufflen >= STATIC_BINARY_LENGTH, "uint16_obj parse error");
		memcpy(&_num, buffer, STATIC_BINARY_LENGTH);
		_num = ntohs(_num);
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint16_obj::serialize_to_buffer(char* buffer, uint32_t bufflen) const
	{
		BINPROTO_ASSERT(bufflen >= STATIC_BINARY_LENGTH, "uint16_obj serialize error");
		uint16_t net_uint = htons(_num);
		memcpy(buffer, &net_uint, STATIC_BINARY_LENGTH);
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint32_obj::parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
	{
		_BINPROTO_PARSE_ENSURE(bufflen >= STATIC_BINARY_LENGTH, "uint32_obj parse error");
		memcpy(&_num, buffer, STATIC_BINARY_LENGTH);
		_num = ntohl(_num);
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint32_obj::serialize_to_buffer(char* buffer, uint32_t bufflen) const
	{
		BINPROTO_ASSERT(bufflen >= STATIC_BINARY_LENGTH, "uint32_obj serialize error");
		uint32_t net_uint = htonl(_num);
		memcpy(buffer, &net_uint, STATIC_BINARY_LENGTH);
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint64_obj::parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
	{
		_BINPROTO_PARSE_ENSURE(bufflen >= STATIC_BINARY_LENGTH, "uint64_obj parse error");
		uint32_t net_uint;
		memcpy(&net_uint, buffer, BINPROTO_UINT_SIZE(uint32_t));
		_num = ntohl(net_uint);
		_num = _num << 32;
		memcpy(&net_uint, buffer + 4, BINPROTO_UINT_SIZE(uint32_t));
		_num += ntohl(net_uint);
		return STATIC_BINARY_LENGTH;
	}
	template<>
	inline uint32_t uint64_obj::serialize_to_buffer(char* buffer, uint32_t bufflen) const
	{
		BINPROTO_ASSERT(bufflen >= STATIC_BINARY_LENGTH, "uint64_obj serialize error");
		uint32_t net_uint = htonl(_num >> 32);
		memcpy(buffer, &net_uint, BINPROTO_UINT_SIZE(uint32_t));
		net_uint = htonl(_num);
		memcpy(buffer + 4, &net_uint, BINPROTO_UINT_SIZE(uint32_t));
		return STATIC_BINARY_LENGTH;
	}

	template<int len_size>
	class variable_len_string
	{
	public:
		typedef num_obj<BINPROTO_UINT_TYPE_FROM_SIZE(len_size)> len_type;
		static const int STATIC_BINARY_LENGTH = 0;
	public:
		variable_len_string() {}
		variable_len_string(const char* str)
		{
			BINPROTO_ASSERT(strlen(str) <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type), "str len must not larger than len_size max value");
			_str = str;
		}
		variable_len_string(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type), "str len must not larger than len_size max value");
			_str.assign(str, size);
		}
		variable_len_string(const std::string& str) :_str(str) {
			BINPROTO_ASSERT(str.size() <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type), "str len must not larger than len_size max value");
		}
		variable_len_string(const variable_len_string& str) :_str(str._str) {}
		variable_len_string& operator=(const std::string& str)
		{
			BINPROTO_ASSERT(str.size() <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type), "str len must not larger than len_size max value");
			_str = str;
			return *this;
		}
		variable_len_string& operator=(const char* str)
		{
			BINPROTO_ASSERT(strlen(str) <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type), "str len must not larger than len_size max value");
			_str = str;
			return *this;
		}
		variable_len_string& operator=(const variable_len_string& str)
		{
			_str = str._str;
			return *this;
		}
		variable_len_string& assign(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= BINPROTO_UINT_MAX_VALUE(typename len_type::uint_type), "str len must not larger than len_size max value");
			_str.assign(str, size);
			return *this;
		}
		const std::string& to_string() const
		{
			return _str;
		}
		std::string trim() const
		{
			return _str.substr(0, strlen(c_str()));
		}
		const char* c_str() const
		{
			return _str.c_str();
		}
		uint32_t size() const
		{
			return _str.size();
		}
		bool operator<(const variable_len_string& other) const
		{
			return _str < other._str;
		}
		bool operator==(const variable_len_string& other) const
		{
			return _str == other._str;
		}
		bool operator>(const variable_len_string& other) const
		{
			return _str > other._str;
		}
		bool operator!=(const variable_len_string& other) const
		{
			return _str != other._str;
		}
		uint32_t serialize_to_buffer(char* buffer, uint32_t bufflen) const
		{
			BINPROTO_ASSERT(get_binary_len() <= bufflen, "variable_len_string serialize error");
			uint32_t temp_len = 0;
			temp_len += len_type(_str.length()).serialize_to_buffer(buffer, bufflen);
			memcpy(buffer + temp_len, _str.c_str(), _str.length());
			temp_len += _str.size();
			return temp_len;
		}
		uint32_t parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
		{
			uint32_t temp_len = 0;
			len_type temp;
			_BINPROTO_PARSE_ENSURE(bufflen >= temp.get_binary_len(), "variable_len_string parse error");
			temp_len += temp.parse_from_buffer(buffer, bufflen);
			_BINPROTO_PARSE_ENSURE(bufflen >= temp_len + temp.to_int(), "variable_len_string parse error");
			_str.assign(buffer + temp_len, temp.to_int());
			temp_len += temp.to_int();
			return temp_len;
		}
		BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER
			uint32_t get_binary_len() const
		{
			return len_size + _str.length();
		}
	private:
		std::string _str;
	};

	template<int str_len>
	class fixed_len_string
	{
	public:
		static const int STATIC_BINARY_LENGTH = str_len;
	public:
		fixed_len_string()
		{
			memset(_str, 0, STATIC_BINARY_LENGTH + 1);
		}
		fixed_len_string(const char* str)
		{
			_str[STATIC_BINARY_LENGTH] = 0;
			*this = str;
		}
		fixed_len_string(const char* str, uint32_t size)
		{
			_str[STATIC_BINARY_LENGTH] = 0;
			assign(str, size);
		}
		fixed_len_string(const std::string& str)
		{
			_str[STATIC_BINARY_LENGTH] = 0;
			*this = str;
		}
		fixed_len_string(const fixed_len_string& str)
		{
			_str[STATIC_BINARY_LENGTH] = 0;
			*this = str;
		}
		fixed_len_string& operator=(const std::string& str)
		{
			if (str.size() >= STATIC_BINARY_LENGTH)
			{
				memcpy(_str, str.c_str(), STATIC_BINARY_LENGTH);
			}
			else
			{
				memcpy(_str, str.c_str(), str.size());
				memset(_str + str.size(), 0, STATIC_BINARY_LENGTH - str.size());
			}
			return *this;
		}
		fixed_len_string& operator=(const char* str)
		{
			uint32_t paralen = strlen(str);
			if (paralen >= STATIC_BINARY_LENGTH)
			{
				memcpy(_str, str, STATIC_BINARY_LENGTH);
			}
			else
			{
				memcpy(_str, str, paralen);
				memset(_str + paralen, 0, STATIC_BINARY_LENGTH - paralen);
			}
			return *this;
		}
		fixed_len_string& operator=(const fixed_len_string& packstring)
		{
			memcpy(_str, packstring._str, STATIC_BINARY_LENGTH);
			return *this;
		}
		fixed_len_string& assign(const char* str, uint32_t size)
		{
			BINPROTO_ASSERT(size <= STATIC_BINARY_LENGTH, "str len must not larger than template str_len value");
			memcpy(_str, str, size);
			memset(_str + size, 0, STATIC_BINARY_LENGTH - size);
			return *this;
		}
		std::string to_string() const
		{
			return std::string(_str, STATIC_BINARY_LENGTH);
		}
		std::string trim() const
		{
			return std::string(_str, strlen(_str));
		}
		const char* c_str() const
		{
			return _str;
		}
		bool operator<(const fixed_len_string& other) const
		{
			return memcmp(_str, other._str, STATIC_BINARY_LENGTH) < 0;
		}
		bool operator==(const fixed_len_string& other) const
		{
			return memcmp(_str, other._str, STATIC_BINARY_LENGTH) == 0;
		}
		bool operator>(const fixed_len_string& other) const
		{
			return memcmp(_str, other._str, STATIC_BINARY_LENGTH) > 0;
		}
		bool operator!=(const fixed_len_string& other) const
		{
			return memcmp(_str, other._str, STATIC_BINARY_LENGTH) != 0;
		}
		uint32_t serialize_to_buffer(char* buffer, uint32_t bufflen) const
		{
			BINPROTO_ASSERT(STATIC_BINARY_LENGTH <= bufflen, "fixed_len_string serialize error");
			memcpy(buffer, _str, STATIC_BINARY_LENGTH);
			return STATIC_BINARY_LENGTH;
		}
		uint32_t parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
		{
			_BINPROTO_PARSE_ENSURE(STATIC_BINARY_LENGTH <= bufflen, "fixed_len_string parse error");
			memcpy(_str, buffer, STATIC_BINARY_LENGTH);
			return STATIC_BINARY_LENGTH;
		}
		BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER
			uint32_t get_binary_len() const
		{
			return STATIC_BINARY_LENGTH;
		}
	private:
		mutable char _str[STATIC_BINARY_LENGTH + 1];
	};

	template<typename obj_type, int list_len_size>
	class binary_obj_list
	{
	public:
		BINPROTO_STATIC_ASSERT((_binproto_is_binproto_obj<obj_type>::value), "binary_obj_list obj type must be a binproto object type");
	public:
		static const int STATIC_BINARY_LENGTH = 0;
		static const int list_size_len = list_len_size;
		typedef num_obj<BINPROTO_UINT_TYPE_FROM_SIZE(list_len_size)> list_size_type;
	public:
		typedef std::vector<obj_type> container;
		typedef typename container::iterator iterator;
		typedef typename container::const_iterator const_iterator;
		typedef typename container::reverse_iterator reverse_iterator;
		typedef typename container::const_reverse_iterator const_reverse_iterator;
	public:
		binary_obj_list() :_array()
		{
		}
		binary_obj_list(const binary_obj_list& other)
			:_array(other._array)
		{
		}
		iterator begin()
		{
			return _array.begin();
		}
		iterator end()
		{
			return _array.end();
		}
		const_iterator begin() const
		{
			return _array.begin();
		}
		const_iterator end() const
		{
			return _array.end();
		}
		reverse_iterator rbegin()
		{
			return _array.rbegin();
		}
		reverse_iterator rend()
		{
			return _array.rend();
		}
		const_reverse_iterator rbegin() const
		{
			return _array.rbegin();
		}
		const_reverse_iterator rend() const
		{
			return _array.rend();
		}
		void push_back(const obj_type& _Val)
		{
			_array.push_back(_Val);
			BINPROTO_ASSERT(_array.size() <= BINPROTO_UINT_MAX_VALUE(typename list_size_type::uint_type), "list size must not larger than list_size_type max value");
		}
		void pop_back()
		{
			_array.pop_back();
		}
		iterator erase(iterator _Where)
		{
			return _array.erase(_Where);
		}
		iterator erase(iterator _First_arg, iterator _Last_arg)
		{
			return _array.erase(_First_arg, _Last_arg);
		}
		obj_type& front()
		{
			return _array.front();
		}
		obj_type& back()
		{
			return _array.back();
		}
		const obj_type& front() const
		{
			return _array.front();
		}
		const obj_type& back() const
		{
			return _array.back();
		}
		obj_type& operator[](uint32_t _Pos)
		{
			return _array[_Pos];
		}
		const obj_type& operator[](uint32_t _Pos) const
		{
			return _array[_Pos];
		}
		iterator insert(iterator _Where, const obj_type& _Val)
		{
			iterator it = _array.insert(_Where, _Val);
			BINPROTO_ASSERT(_array.size() <= BINPROTO_UINT_MAX_VALUE(typename list_size_type::uint_type), "list size must not larger than list_size_type max value");
			return it;
		}
		uint32_t size() const
		{
			return _array.size();
		}
		void clear()
		{
			_array.clear();
		}
		binary_obj_list& operator=(const binary_obj_list& other)
		{
			_array = other._array;
		}
		void swap(binary_obj_list& other)
		{
			_array.swap(other._array);
		}
		list_size_type get_size_obj() const
		{
			return list_size_type(size());
		}
	public:
		uint32_t serialize_to_buffer(char* buffer, uint32_t bufflen) const
		{
			uint32_t temp_len = 0;
			BINPROTO_ASSERT(list_len_size <= bufflen, "binary_obj_list length serialize error");
			temp_len += get_size_obj().serialize_to_buffer(buffer, bufflen);
			uint32_t s = size();
			for (uint32_t i = 0; i < s; i++)
			{
				temp_len += _array[i].serialize_to_buffer(buffer + temp_len, bufflen - temp_len);
			}
			return temp_len;
		}
		uint32_t parse_from_buffer(const char* buffer, uint32_t bufflen) throw(exception)
		{
			clear();
			uint32_t temp_len = 0;
			list_size_type temp;
			_BINPROTO_PARSE_ENSURE(list_len_size <= bufflen, "binary_obj_list length parse error");
			temp_len += temp.parse_from_buffer(buffer, bufflen);

			_BINPROTO_PARSE_TRY;
			_array.resize(temp.to_int());
			for (uint32_t i = 0; i < temp.to_int(); i++)
			{
				temp_len += _array[i].parse_from_buffer(buffer + temp_len, bufflen - temp_len);
			}
			return temp_len;
			_BINPROTO_PARSE_CATCH("binary_obj_list");
		}
		BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER
			uint32_t get_binary_len() const
		{
			return _get_binary_len_is_static(typename _binproto_bool_value_to_bool_type<obj_type::STATIC_BINARY_LENGTH != 0>::type());
		}
	private:
		uint32_t _get_binary_len_is_static(_binproto_true) const
		{
			size_t iLength = list_size_type::STATIC_BINARY_LENGTH;
			iLength += obj_type::STATIC_BINARY_LENGTH * size();
			return iLength;
		}
		uint32_t _get_binary_len_is_static(_binproto_false) const
		{
			size_t iLength = list_size_type::STATIC_BINARY_LENGTH;

			uint32_t s = size();
			for (uint32_t i = 0; i < s; i++)
			{
				iLength += _array[i].get_binary_len();
			}

			return iLength;
		}
	private:
		container _array;
	};

	struct base_packet
	{
		virtual uint32_t parse_from_buffer(const char* buffer, uint32_t bufflen) throw(binproto::exception) = 0;
		virtual uint32_t serialize_to_buffer(char* buffer, uint32_t bufflen) const = 0;
		virtual uint32_t parse_from_buffer(const std::string& strbuff) throw(binproto::exception) = 0;
		virtual uint32_t serialize_to_buffer(std::string& strbuff) const = 0;
		virtual uint32_t get_binary_len() const = 0;
	};
}

//macro defination
#define _BINPROTO_FUNCTION_PARSE_START uint32_t parse_from_buffer(const char* buffer,uint32_t bufflen) throw(binproto::exception) {uint32_t temp_len = 0;
#define _BINPROTO_FUNCTION_PARSE(object) temp_len += (object).parse_from_buffer(buffer + temp_len, bufflen - temp_len);
#define _BINPROTO_FUNCTION_PARSE_END return temp_len;}

#define _BINPROTO_FUNCTION_SERIALIZE_START uint32_t serialize_to_buffer(char* buffer,uint32_t bufflen) const {uint32_t temp_len = 0;
#define _BINPROTO_FUNCTION_SERIALIZE(object) temp_len += (object).serialize_to_buffer(buffer + temp_len, bufflen - temp_len);
#define _BINPROTO_FUNCTION_SERIALIZE_END return temp_len;}

#define _BINPROTO_FUNCTION_GETLEN_START \
	uint32_t get_binary_len() const{return _get_binary_len_is_static(_binproto_bool_value_to_bool_type<STATIC_BINARY_LENGTH != 0>::type());} \
	uint32_t _get_binary_len_is_static(_binproto_true) const{return STATIC_BINARY_LENGTH;} \
	uint32_t _get_binary_len_is_static(_binproto_false) const{uint32_t temp_len = 0;
#define _BINPROTO_FUNCTION_GETLEN(object) temp_len += (object).get_binary_len();
#define _BINPROTO_FUNCTION_GETLEN_END return temp_len;}

#define _BINPROTO_PACKET_DEFINE_START(classname) struct classname{
#define _BINPROTO_PACKET_DEFINE_END };

#define _BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START static const int STATIC_BINARY_LENGTH = (1
#define _BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE )*(0
#define _BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END );

#define BINPROTO_DEFINE_PACKET_P00(classname) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P01(classname, type01, name01) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P02(classname, type01, name01, type02, name02) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P03(classname, type01, name01, type02, name02, type03, name03) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P04(classname, type01, name01, type02, name02, type03, name03, type04, name04) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P05(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P06(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P07(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P08(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P09(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P10(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P11(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P12(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P13(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P14(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P15(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14, type15, name15) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	&&(type15::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	+(type15::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	type15 name15; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_FUNCTION_PARSE(name15) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE(name15) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN(name15) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P16(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14, type15, name15, type16, name16) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	&&(type15::STATIC_BINARY_LENGTH) \
	&&(type16::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	+(type15::STATIC_BINARY_LENGTH) \
	+(type16::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	type15 name15; \
	type16 name16; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_FUNCTION_PARSE(name15) \
	_BINPROTO_FUNCTION_PARSE(name16) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE(name15) \
	_BINPROTO_FUNCTION_SERIALIZE(name16) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN(name15) \
	_BINPROTO_FUNCTION_GETLEN(name16) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P17(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14, type15, name15, type16, name16, type17, name17) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	&&(type15::STATIC_BINARY_LENGTH) \
	&&(type16::STATIC_BINARY_LENGTH) \
	&&(type17::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	+(type15::STATIC_BINARY_LENGTH) \
	+(type16::STATIC_BINARY_LENGTH) \
	+(type17::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	type15 name15; \
	type16 name16; \
	type17 name17; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_FUNCTION_PARSE(name15) \
	_BINPROTO_FUNCTION_PARSE(name16) \
	_BINPROTO_FUNCTION_PARSE(name17) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE(name15) \
	_BINPROTO_FUNCTION_SERIALIZE(name16) \
	_BINPROTO_FUNCTION_SERIALIZE(name17) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN(name15) \
	_BINPROTO_FUNCTION_GETLEN(name16) \
	_BINPROTO_FUNCTION_GETLEN(name17) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P18(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14, type15, name15, type16, name16, type17, name17, type18, name18) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	&&(type15::STATIC_BINARY_LENGTH) \
	&&(type16::STATIC_BINARY_LENGTH) \
	&&(type17::STATIC_BINARY_LENGTH) \
	&&(type18::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	+(type15::STATIC_BINARY_LENGTH) \
	+(type16::STATIC_BINARY_LENGTH) \
	+(type17::STATIC_BINARY_LENGTH) \
	+(type18::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	type15 name15; \
	type16 name16; \
	type17 name17; \
	type18 name18; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_FUNCTION_PARSE(name15) \
	_BINPROTO_FUNCTION_PARSE(name16) \
	_BINPROTO_FUNCTION_PARSE(name17) \
	_BINPROTO_FUNCTION_PARSE(name18) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE(name15) \
	_BINPROTO_FUNCTION_SERIALIZE(name16) \
	_BINPROTO_FUNCTION_SERIALIZE(name17) \
	_BINPROTO_FUNCTION_SERIALIZE(name18) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN(name15) \
	_BINPROTO_FUNCTION_GETLEN(name16) \
	_BINPROTO_FUNCTION_GETLEN(name17) \
	_BINPROTO_FUNCTION_GETLEN(name18) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P19(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	&&(type15::STATIC_BINARY_LENGTH) \
	&&(type16::STATIC_BINARY_LENGTH) \
	&&(type17::STATIC_BINARY_LENGTH) \
	&&(type18::STATIC_BINARY_LENGTH) \
	&&(type19::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	+(type15::STATIC_BINARY_LENGTH) \
	+(type16::STATIC_BINARY_LENGTH) \
	+(type17::STATIC_BINARY_LENGTH) \
	+(type18::STATIC_BINARY_LENGTH) \
	+(type19::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	type15 name15; \
	type16 name16; \
	type17 name17; \
	type18 name18; \
	type19 name19; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_FUNCTION_PARSE(name15) \
	_BINPROTO_FUNCTION_PARSE(name16) \
	_BINPROTO_FUNCTION_PARSE(name17) \
	_BINPROTO_FUNCTION_PARSE(name18) \
	_BINPROTO_FUNCTION_PARSE(name19) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE(name15) \
	_BINPROTO_FUNCTION_SERIALIZE(name16) \
	_BINPROTO_FUNCTION_SERIALIZE(name17) \
	_BINPROTO_FUNCTION_SERIALIZE(name18) \
	_BINPROTO_FUNCTION_SERIALIZE(name19) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN(name15) \
	_BINPROTO_FUNCTION_GETLEN(name16) \
	_BINPROTO_FUNCTION_GETLEN(name17) \
	_BINPROTO_FUNCTION_GETLEN(name18) \
	_BINPROTO_FUNCTION_GETLEN(name19) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#define BINPROTO_DEFINE_PACKET_P20(classname, type01, name01, type02, name02, type03, name03, type04, name04, type05, name05, type06, name06, type07, name07, type08, name08, type09, name09, type10, name10, type11, name11, type12, name12, type13, name13, type14, name14, type15, name15, type16, name16, type17, name17, type18, name18, type19, name19, type20, name20) \
	_BINPROTO_PACKET_DEFINE_START(classname) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_START \
	&&(type01::STATIC_BINARY_LENGTH) \
	&&(type02::STATIC_BINARY_LENGTH) \
	&&(type03::STATIC_BINARY_LENGTH) \
	&&(type04::STATIC_BINARY_LENGTH) \
	&&(type05::STATIC_BINARY_LENGTH) \
	&&(type06::STATIC_BINARY_LENGTH) \
	&&(type07::STATIC_BINARY_LENGTH) \
	&&(type08::STATIC_BINARY_LENGTH) \
	&&(type09::STATIC_BINARY_LENGTH) \
	&&(type10::STATIC_BINARY_LENGTH) \
	&&(type11::STATIC_BINARY_LENGTH) \
	&&(type12::STATIC_BINARY_LENGTH) \
	&&(type13::STATIC_BINARY_LENGTH) \
	&&(type14::STATIC_BINARY_LENGTH) \
	&&(type15::STATIC_BINARY_LENGTH) \
	&&(type16::STATIC_BINARY_LENGTH) \
	&&(type17::STATIC_BINARY_LENGTH) \
	&&(type18::STATIC_BINARY_LENGTH) \
	&&(type19::STATIC_BINARY_LENGTH) \
	&&(type20::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_MIDDLE \
	+(type01::STATIC_BINARY_LENGTH) \
	+(type02::STATIC_BINARY_LENGTH) \
	+(type03::STATIC_BINARY_LENGTH) \
	+(type04::STATIC_BINARY_LENGTH) \
	+(type05::STATIC_BINARY_LENGTH) \
	+(type06::STATIC_BINARY_LENGTH) \
	+(type07::STATIC_BINARY_LENGTH) \
	+(type08::STATIC_BINARY_LENGTH) \
	+(type09::STATIC_BINARY_LENGTH) \
	+(type10::STATIC_BINARY_LENGTH) \
	+(type11::STATIC_BINARY_LENGTH) \
	+(type12::STATIC_BINARY_LENGTH) \
	+(type13::STATIC_BINARY_LENGTH) \
	+(type14::STATIC_BINARY_LENGTH) \
	+(type15::STATIC_BINARY_LENGTH) \
	+(type16::STATIC_BINARY_LENGTH) \
	+(type17::STATIC_BINARY_LENGTH) \
	+(type18::STATIC_BINARY_LENGTH) \
	+(type19::STATIC_BINARY_LENGTH) \
	+(type20::STATIC_BINARY_LENGTH) \
	_BINPROTO_DEFINE_STATIC_BINARY_LENGTH_END \
	type01 name01; \
	type02 name02; \
	type03 name03; \
	type04 name04; \
	type05 name05; \
	type06 name06; \
	type07 name07; \
	type08 name08; \
	type09 name09; \
	type10 name10; \
	type11 name11; \
	type12 name12; \
	type13 name13; \
	type14 name14; \
	type15 name15; \
	type16 name16; \
	type17 name17; \
	type18 name18; \
	type19 name19; \
	type20 name20; \
	_BINPROTO_FUNCTION_PARSE_START \
	_BINPROTO_PARSE_TRY \
	_BINPROTO_FUNCTION_PARSE(name01) \
	_BINPROTO_FUNCTION_PARSE(name02) \
	_BINPROTO_FUNCTION_PARSE(name03) \
	_BINPROTO_FUNCTION_PARSE(name04) \
	_BINPROTO_FUNCTION_PARSE(name05) \
	_BINPROTO_FUNCTION_PARSE(name06) \
	_BINPROTO_FUNCTION_PARSE(name07) \
	_BINPROTO_FUNCTION_PARSE(name08) \
	_BINPROTO_FUNCTION_PARSE(name09) \
	_BINPROTO_FUNCTION_PARSE(name10) \
	_BINPROTO_FUNCTION_PARSE(name11) \
	_BINPROTO_FUNCTION_PARSE(name12) \
	_BINPROTO_FUNCTION_PARSE(name13) \
	_BINPROTO_FUNCTION_PARSE(name14) \
	_BINPROTO_FUNCTION_PARSE(name15) \
	_BINPROTO_FUNCTION_PARSE(name16) \
	_BINPROTO_FUNCTION_PARSE(name17) \
	_BINPROTO_FUNCTION_PARSE(name18) \
	_BINPROTO_FUNCTION_PARSE(name19) \
	_BINPROTO_FUNCTION_PARSE(name20) \
	_BINPROTO_PARSE_CATCH(#classname) \
	_BINPROTO_FUNCTION_PARSE_END \
	_BINPROTO_FUNCTION_SERIALIZE_START \
	_BINPROTO_FUNCTION_SERIALIZE(name01) \
	_BINPROTO_FUNCTION_SERIALIZE(name02) \
	_BINPROTO_FUNCTION_SERIALIZE(name03) \
	_BINPROTO_FUNCTION_SERIALIZE(name04) \
	_BINPROTO_FUNCTION_SERIALIZE(name05) \
	_BINPROTO_FUNCTION_SERIALIZE(name06) \
	_BINPROTO_FUNCTION_SERIALIZE(name07) \
	_BINPROTO_FUNCTION_SERIALIZE(name08) \
	_BINPROTO_FUNCTION_SERIALIZE(name09) \
	_BINPROTO_FUNCTION_SERIALIZE(name10) \
	_BINPROTO_FUNCTION_SERIALIZE(name11) \
	_BINPROTO_FUNCTION_SERIALIZE(name12) \
	_BINPROTO_FUNCTION_SERIALIZE(name13) \
	_BINPROTO_FUNCTION_SERIALIZE(name14) \
	_BINPROTO_FUNCTION_SERIALIZE(name15) \
	_BINPROTO_FUNCTION_SERIALIZE(name16) \
	_BINPROTO_FUNCTION_SERIALIZE(name17) \
	_BINPROTO_FUNCTION_SERIALIZE(name18) \
	_BINPROTO_FUNCTION_SERIALIZE(name19) \
	_BINPROTO_FUNCTION_SERIALIZE(name20) \
	_BINPROTO_FUNCTION_SERIALIZE_END \
	_BINPROTO_FUNCTION_GETLEN_START \
	_BINPROTO_FUNCTION_GETLEN(name01) \
	_BINPROTO_FUNCTION_GETLEN(name02) \
	_BINPROTO_FUNCTION_GETLEN(name03) \
	_BINPROTO_FUNCTION_GETLEN(name04) \
	_BINPROTO_FUNCTION_GETLEN(name05) \
	_BINPROTO_FUNCTION_GETLEN(name06) \
	_BINPROTO_FUNCTION_GETLEN(name07) \
	_BINPROTO_FUNCTION_GETLEN(name08) \
	_BINPROTO_FUNCTION_GETLEN(name09) \
	_BINPROTO_FUNCTION_GETLEN(name10) \
	_BINPROTO_FUNCTION_GETLEN(name11) \
	_BINPROTO_FUNCTION_GETLEN(name12) \
	_BINPROTO_FUNCTION_GETLEN(name13) \
	_BINPROTO_FUNCTION_GETLEN(name14) \
	_BINPROTO_FUNCTION_GETLEN(name15) \
	_BINPROTO_FUNCTION_GETLEN(name16) \
	_BINPROTO_FUNCTION_GETLEN(name17) \
	_BINPROTO_FUNCTION_GETLEN(name18) \
	_BINPROTO_FUNCTION_GETLEN(name19) \
	_BINPROTO_FUNCTION_GETLEN(name20) \
	_BINPROTO_FUNCTION_GETLEN_END \
	BINPROTO_PARSE_AND_SERIALIZE_ON_STD_CONTAINER \
	_BINPROTO_PACKET_DEFINE_END

#endif//__BINPROTO_HPP__