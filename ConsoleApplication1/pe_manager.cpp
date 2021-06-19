#include "pe_manger.h"

bool pe_manger::adjust_privilege()
{
	bool ret = false;
	TOKEN_PRIVILEGES tp = { 0 };//令牌权限结构
	HANDLE token_handle = nullptr;//令牌句柄

	do
	{
		//打开当前进程令牌,并且获取它              //令牌权限修改和查询
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |
			TOKEN_QUERY, &token_handle)) {
			break;
		}
			
		//获取关机注销重启的LUID(Locally Unique Identifier),局部唯一标识
		if (!LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid)){
			break;
		}
			
		tp.PrivilegeCount = 1;//修改权限的个数
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;//激活SE_SHUTDOWN_NAME这个权限
														   //提升权限//FALSE表示可以修改权限//把需要修改的权限传进来
		if (!AdjustTokenPrivileges(token_handle, false, &tp, 0, (PTOKEN_PRIVILEGES)nullptr, 0)){
			break;
		}
			
		ret = true;
	} while (false);


	if (token_handle) {
		CloseHandle(token_handle);
	}
		
	return ret;
}

bool pe_manger::is_admin(HANDLE process_handle)
{
	HANDLE token_handle = nullptr;
	OpenProcessToken(process_handle, TOKEN_QUERY, &token_handle);

	//用于接收令牌类型
	TOKEN_ELEVATION_TYPE token_type = TokenElevationTypeDefault;

	DWORD ret_size = 0;	//用于接收函数输出信息的字节数

	//查询进程令牌中的权限提升值（这个值记录当前的令牌是何种类型
	GetTokenInformation(
		token_handle,
		TokenElevationType,		//获取令牌的当前提升等级
		&token_type,
		sizeof(token_type),
		&ret_size	//所需缓冲区的字节数
	);

	//分局令牌的类型来输出相应的信息
	if (TokenElevationTypeFull == token_type){
		//如果令牌是TokenElevationTypeFull,则拥有最高权限，可以给令牌添加任何特权,返回第0步执行代码.
		return true;
	}
	
	if (TokenElevationTypeDefault == token_type){
		//默认要哪个虎可能是一个没有权限的标准用户
		//也可能是被UAC关闭
		//直接调用系统的函数IsUserAbAdmin来判断用户是否是管理员
		return IsUserAnAdmin();
	}
	
	if (TokenElevationTypeLimited == token_type){
		return false;
	}

	return false;
}

void pe_manger::on_shutdown()
{
  if (adjust_privilege()){
	  ExitWindowsEx(EWX_SHUTDOWN, 0);
  }
}

void pe_manger::on_logout()
{
	if (adjust_privilege()) {
		ExitWindowsEx(EWX_LOGOFF, 0);
	}
}

void pe_manger::on_reboot()
{
	if (adjust_privilege()) {
		ExitWindowsEx(EWX_REBOOT, 0);
	}
}

