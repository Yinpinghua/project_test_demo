#ifndef super_box_h__
#define super_box_h__
#ifdef SUPERBOX_EXPORTS
#define SUPERBOX_API __declspec(dllexport)
#else
#define SUPERBOX_API __declspec(dllimport)
#endif

#ifdef _WIN32
#define CALL_METHOD __stdcall
#else
#define CALL_METHOD __fastcall
#endif

#ifdef __cplusplus
extern "C" {
#endif
	SUPERBOX_API int CALL_METHOD connect_server();
	//返回从服务端接受的buff的长度,等于0表示出错
	SUPERBOX_API unsigned int CALL_METHOD recv_msg();
	//buff-返回从服务器接受内容
	//len-传入从服务器接收到的长度
	SUPERBOX_API void CALL_METHOD get_data(char* buff, unsigned int len);
	//释放socket资源
	SUPERBOX_API void CALL_METHOD close_socket();

	SUPERBOX_API void CALL_METHOD user_regedit(const char* user_name, const char* password);
#ifdef __cplusplus
}
#endif

#endif // super_box_h__
