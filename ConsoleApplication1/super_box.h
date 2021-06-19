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
	//���شӷ���˽��ܵ�buff�ĳ���,����0��ʾ����
	SUPERBOX_API unsigned int CALL_METHOD recv_msg();
	//buff-���شӷ�������������
	//len-����ӷ��������յ��ĳ���
	SUPERBOX_API void CALL_METHOD get_data(char* buff, unsigned int len);
	//�ͷ�socket��Դ
	SUPERBOX_API void CALL_METHOD close_socket();

	SUPERBOX_API void CALL_METHOD user_regedit(const char* user_name, const char* password);
#ifdef __cplusplus
}
#endif

#endif // super_box_h__
