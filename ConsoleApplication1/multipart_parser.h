#ifndef multipart_parser_h__
#define multipart_parser_h__

#include <iostream>
#include <string>
#include <list>
#include <map>

#define CR '\r'
#define LF '\n'
#define SP ' '
#define HT '\t'
#define HYPHEN '-'

#define CALLBACK_NOTIFY(NAME)                           \
        if (this->on_##NAME() != 0)                     \
            goto error;                                 

#define CALLBACK_DATA(NAME, P, S)                       \
        if (this->on_##NAME(P, S) != 0)                 \
            goto error;                                 

/* Header field name as defined by rfc 2616. Also lowercases them.
 *     field-name   = token
 *     token        = 1*<any CHAR except CTLs or tspecials>
 *     CTL          = <any US-ASCII control character (octets 0 - 31) and DEL (127)>
 *     tspecials    = "(" | ")" | "<" | ">" | "@"
 *                  | "," | ";" | ":" | "\" | DQUOTE
 *                  | "/" | "[" | "]" | "?" | "="
 *                  | "{" | "}" | SP | HT
 *     DQUOTE       = <US-ASCII double-quote mark (34)>
 *     SP           = <US-ASCII SP, space (32)>
 *     HT           = <US-ASCII HT, horizontal-tab (9)>
 */
static const char g_header_field_chars[256] = {
	/*  0 nul   1 soh   2 stx   3 etx   4 eot   5 enq   6 ack   7 bel   */
		0,      0,      0,      0,      0,      0,      0,      0,
	/*  8 bs    9 ht    10 nl   11 vt   12 np   13 cr   14 so   15 si   */
		0,      0,      0,      0,      0,      0,      0,      0,
		/*  16 dle  17 dc1  18 dc2  19 dc3  20 dc4  21 nak  22 syn  23 etb  */
		0,      0,      0,      0,      0,      0,      0,      0,
		/*  24 can  25 em   26 sub  27 esc  28 fs   29 gs   30 rs   31 us   */
		0,      0,      0,      0,      0,      0,      0,      0,
		/*  32 sp   33 !    34 "    35 #    36 $    37 %    38 &    39 '    */
		0,      '!',    0,      '#',    '$',    '%',    '&',    '\'',
		/*  40 (    41 )    42 *    43 +    44 ,    45 -    46 .    47 /    */
		0,      0,      '*',    '+',    0,      '-',    '.',    0,
		/*  48 0    49 1    50 2    51 3    52 4    53 5    54 6    55 7    */
		'0',    '1',    '2',    '3',    '4',    '5',    '6',    '7',
		/*  56 8    57 9    58 :    59 ;    60 <    61 =    62 >    63 ?    */
		'8',    '9',    0,      0,      0,      0,      0,      0,
		/*  64 @    65 A    66 B    67 C    68 D    69 E    70 F    71 G    */
		0,      'A',    'B',    'C',    'D',    'E',    'F',    'G',
		/*  72 H    73 I    74 J    75 K    76 L    77 M    78 N    79 O    */
		'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
		/*  80 P    81 Q    82 R    83 S    84 T    85 U    86 V    87 W    */
		'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
		/*  88 X    89 Y    90 Z    91 [    92 \    93 ]    94 ^    95 _    */
		'X',    'Y',    'Z',     0,     0,      0,      '^',    '_',
		/*  96 `    97 a    98 b    99 c    100 d   101 e   102 f   103 g   */
		'`',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
		/*  104 h   105 i   106 j   107 k   108 l   109 m   110 n   111 o   */
		'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
		/*  112 p   113 q   114 r   115 s   116 t   117 u   118 v   119 w   */
		'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
		/*  120 x   121 y   122 z   123 {   124 |   125 }   126 ~   127 del */
		'x',    'y',    'z',    0,      '|',     0,     '~',    0
};

enum class state {
	s_preamble,
	s_preamble_hy_hy,
	s_first_boundary,
	s_header_field_start,
	s_header_field,
	s_header_value_start,
	s_header_value,
	s_header_value_cr,
	s_headers_done,
	s_data,
	s_data_cr,
	s_data_cr_lf,
	s_data_cr_lf_hy,
	s_data_boundary_start,
	s_data_boundary,
	s_data_boundary_done,
	s_data_boundary_done_cr_lf,
	s_data_boundary_done_hy_hy,
	s_epilogue,
};

class multipart_parser
{
	typedef struct part {
		std::map<std::string, std::string> headers;
		std::string body;
	} part;
public:
	explicit multipart_parser(const std::string & boundary_str);
	~multipart_parser() = default;
	size_t parser_data(const char* data,size_t size);
	std::map<std::string, std::string> get_multipart_data();
private:
	int on_body_begin()
	{
		body_beg_called_ = true;
		return 0;
	}

	int on_part_begin()
	{
		parts_.push_back(part());
		return 0;
	}
	
	int on_header_field(const char* data, size_t size)
	{
		if (header_value_.size() > 0)
			on_header_done();
		header_name_.append(data, size);
		return 0;
	}

	void on_header_done()
	{
		parts_.back().headers[header_name_] = header_value_;
		header_name_.clear();
		header_value_.clear();
	}

	int on_header_value( const char* data, size_t size)
	{
		header_value_.append(data, size);
		return 0;
	}

	int on_headers_complete()
	{
		if (header_value_.size() > 0)
			on_header_done();
		return 0;
	}

	int on_data(const char* data, size_t size)
	{
		parts_.back().body.append(data, size);
		return 0;
	}

	int on_part_end()
	{
		return 0;
	}

	int on_body_end()
	{
		body_end_called_ = true;
		return 0;
	}
private:
	bool            body_beg_called_=false;
	bool            body_end_called_ = false;
	size_t          boundary_len_;
	size_t          index_ =0;
	uint16_t        state_ =static_cast<uint16_t>(state::s_preamble);
	std::string     data_;
	std::string     boundary_;
	std::string     header_value_;
	std::string     header_name_;
	std::list<part> parts_;
};
#endif // multipart_parser_h__
