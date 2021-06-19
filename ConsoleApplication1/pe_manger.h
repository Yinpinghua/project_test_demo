#ifndef pe_manger_h__
#define pe_manger_h__

#include<windows.h>
#include<Winuser.h>
#include <shlobj.h>

class pe_manger
{
public:
	pe_manger() = default;
	~pe_manger() = default;

	bool is_admin(HANDLE process_handle);
	//�ػ�
	void on_shutdown();
	//ע��
	void on_logout();
	//����
	void on_reboot();
private:
	//��Ȩ����
	bool adjust_privilege();
};
#endif // pe_manger_h__
