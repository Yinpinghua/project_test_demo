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
	//关机
	void on_shutdown();
	//注销
	void on_logout();
	//重启
	void on_reboot();
private:
	//提权函数
	bool adjust_privilege();
};
#endif // pe_manger_h__
