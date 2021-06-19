#include "pe_manger.h"

bool pe_manger::adjust_privilege()
{
	bool ret = false;
	TOKEN_PRIVILEGES tp = { 0 };//����Ȩ�޽ṹ
	HANDLE token_handle = nullptr;//���ƾ��

	do
	{
		//�򿪵�ǰ��������,���һ�ȡ��              //����Ȩ���޸ĺͲ�ѯ
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES |
			TOKEN_QUERY, &token_handle)) {
			break;
		}
			
		//��ȡ�ػ�ע��������LUID(Locally Unique Identifier),�ֲ�Ψһ��ʶ
		if (!LookupPrivilegeValue(nullptr, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid)){
			break;
		}
			
		tp.PrivilegeCount = 1;//�޸�Ȩ�޵ĸ���
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;//����SE_SHUTDOWN_NAME���Ȩ��
														   //����Ȩ��//FALSE��ʾ�����޸�Ȩ��//����Ҫ�޸ĵ�Ȩ�޴�����
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

	//���ڽ�����������
	TOKEN_ELEVATION_TYPE token_type = TokenElevationTypeDefault;

	DWORD ret_size = 0;	//���ڽ��պ��������Ϣ���ֽ���

	//��ѯ���������е�Ȩ������ֵ�����ֵ��¼��ǰ�������Ǻ�������
	GetTokenInformation(
		token_handle,
		TokenElevationType,		//��ȡ���Ƶĵ�ǰ�����ȼ�
		&token_type,
		sizeof(token_type),
		&ret_size	//���軺�������ֽ���
	);

	//�־����Ƶ������������Ӧ����Ϣ
	if (TokenElevationTypeFull == token_type){
		//���������TokenElevationTypeFull,��ӵ�����Ȩ�ޣ����Ը���������κ���Ȩ,���ص�0��ִ�д���.
		return true;
	}
	
	if (TokenElevationTypeDefault == token_type){
		//Ĭ��Ҫ�ĸ���������һ��û��Ȩ�޵ı�׼�û�
		//Ҳ�����Ǳ�UAC�ر�
		//ֱ�ӵ���ϵͳ�ĺ���IsUserAbAdmin���ж��û��Ƿ��ǹ���Ա
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

