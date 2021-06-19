// CSystemStatus.h: interface for the CSystemStatus class.---chenxiang---2019-08-08----
#ifndef __CSystemStatus_h__
#define __CSystemStatus_h__
#include <vector>
#include <string>
#include <pdh.h>
#include <Shlwapi.h>
#include <windows.h>
#include <string>
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") 
#pragma comment(lib, "Pdh.lib")
#pragma comment(lib, "Shlwapi.lib")

 //���д��̵�״̬
typedef struct tagAllDISKSTATUS
{
	UINT	_DiskCount;				//��������
	DWORD	_Total;					//���д���������MB
	DWORD	_OfFree;				//���д���ʣ������MB
}AllDISKSTATUS, * LPAllDISKSTATUS;

typedef struct tagEACHDISKSTATUS
{
	std::string _strdir;			//��������
	float	_Total;					//����������MB
	float	_OfFree;				//����ʣ������MB
}EACHDISKSTATUS, * LPEACHDISKSTATUS;

typedef struct tagNETCARDINFO
{
	std::string Name;				//��������
	std::string Description;		//��������
	std::string Local_IP;			//IP��ַ
	std::string Local_Mac;			//MAC��ַ
}NETCARDINFO, * LPNETCARDINFO;


#define SYSSTATE_NONE			0x00000000
#define SYSSTATE_CPU_USAGE		0x00000001
#define SYSSTATE_DISK_READ		0x00000002
#define SYSSTATE_DISK_WRITE		0x00000004
#define SYSSTATE_NET_DOWNLOAD	0x00000008
#define SYSSTATE_NET_UPLOAD		0x00000010

#define DFP_GET_VERSION			0x00074080 
#define DFP_SEND_DRIVE_COMMAND	0x0007c084 
#define DFP_RECEIVE_DRIVE_DATA	0x0007c088 

class CSystemStatus
{
public:
	CSystemStatus();
	~CSystemStatus();
public:
	void		SystemInit(DWORD object = SYSSTATE_CPU_USAGE);							//ϵͳ��ʼ��(��ʼ�������Ŀʱʹ�û���������)
	void		SystemUnInit();															//�ͷ���Դ
	double		GetSystemNetDownloadRate();												//��ȡ���������ٶ�
	double		GetSystemNetUploadRate();												//��ȡ�����ϴ��ٶ�
	double		GetSystemDiskReadRate();												//��ȡ��ǰ���̶�����
	double		GetSystemDiskWriteRate();												//��ȡ��ǰ����д����
	double		GetSystemCpuCurrentUsage();												//ϵͳCPUʹ����

	void		GetSystemDiskStatus(std::vector<EACHDISKSTATUS>& vectorDisk);           //��ȡ��������ʹ��״̬
	void		GetSystemDiskStatus(ULONGLONG& AllDiskTotal, ULONGLONG& AllDiskFree);	//��ȡϵͳ�ܵô���ʹ��״̬
	void		GetSystemCurrentDiskStatus(ULONGLONG& TatolMB, ULONGLONG& FreeCaller);	//��ȡ��ǰ����ʹ��״̬
	double		GetSystemCurrentDiskUsage();											//��ȡ��ǰ����ʹ����

	BOOL		GetPhysicalMemoryState(ULONGLONG& totalPhysMem, ULONGLONG& physMemUsed);//��ȡ�����ڴ�״̬
	double		GetTotalPhysicalMemory();												//��ȡ�����ڴ��С
	double		GetTotalPhysicalMemoryFree();											//��ȡ�����ڴ�
	double		GetTotalPhysicalMemoryUsed();											//��ȡ��ʹ���ڴ��С
	double		GetPhysicalMemoryUsage();												//��ȡ�ڴ�ʹ����

	void		GetNetCardInfo(std::vector<NETCARDINFO>& vectorNetCard);				//��ȡ������Ϣ
	void		GetOsInfo(std::string& osinfo);                                         //��ȡ����ϵͳ��Ϣ 
	void		GetCPUid(std::string& CPUid);											//��ȡCPUid
private:
	PDH_HQUERY		m_Query =nullptr;
	PDH_HCOUNTER	m_CpuTotal = nullptr;
	PDH_HCOUNTER    m_DiskRead = nullptr;
	PDH_HCOUNTER    m_DiskWrite = nullptr;
	PDH_HCOUNTER    m_NetDownload = nullptr;
	PDH_HCOUNTER    m_NetUpload = nullptr;
};
#endif