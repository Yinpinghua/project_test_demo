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

 //所有磁盘的状态
typedef struct tagAllDISKSTATUS
{
	UINT	_DiskCount;				//磁盘数量
	DWORD	_Total;					//所有磁盘总容量MB
	DWORD	_OfFree;				//所有磁盘剩余容量MB
}AllDISKSTATUS, * LPAllDISKSTATUS;

typedef struct tagEACHDISKSTATUS
{
	std::string _strdir;			//磁盘名称
	float	_Total;					//磁盘总容量MB
	float	_OfFree;				//磁盘剩余容量MB
}EACHDISKSTATUS, * LPEACHDISKSTATUS;

typedef struct tagNETCARDINFO
{
	std::string Name;				//网卡名称
	std::string Description;		//网卡描述
	std::string Local_IP;			//IP地址
	std::string Local_Mac;			//MAC地址
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
	void		SystemInit(DWORD object = SYSSTATE_CPU_USAGE);							//系统初始化(初始化多个项目时使用或运算连接)
	void		SystemUnInit();															//释放资源
	double		GetSystemNetDownloadRate();												//获取网络下载速度
	double		GetSystemNetUploadRate();												//获取网络上传速度
	double		GetSystemDiskReadRate();												//获取当前磁盘读速率
	double		GetSystemDiskWriteRate();												//获取当前磁盘写速率
	double		GetSystemCpuCurrentUsage();												//系统CPU使用率

	void		GetSystemDiskStatus(std::vector<EACHDISKSTATUS>& vectorDisk);           //获取各个磁盘使用状态
	void		GetSystemDiskStatus(ULONGLONG& AllDiskTotal, ULONGLONG& AllDiskFree);	//获取系统总得磁盘使用状态
	void		GetSystemCurrentDiskStatus(ULONGLONG& TatolMB, ULONGLONG& FreeCaller);	//获取当前磁盘使用状态
	double		GetSystemCurrentDiskUsage();											//获取当前磁盘使用率

	BOOL		GetPhysicalMemoryState(ULONGLONG& totalPhysMem, ULONGLONG& physMemUsed);//获取物理内存状态
	double		GetTotalPhysicalMemory();												//获取可用内存大小
	double		GetTotalPhysicalMemoryFree();											//获取空闲内存
	double		GetTotalPhysicalMemoryUsed();											//获取已使用内存大小
	double		GetPhysicalMemoryUsage();												//获取内存使用率

	void		GetNetCardInfo(std::vector<NETCARDINFO>& vectorNetCard);				//获取网卡信息
	void		GetOsInfo(std::string& osinfo);                                         //获取操作系统信息 
	void		GetCPUid(std::string& CPUid);											//获取CPUid
private:
	PDH_HQUERY		m_Query =nullptr;
	PDH_HCOUNTER	m_CpuTotal = nullptr;
	PDH_HCOUNTER    m_DiskRead = nullptr;
	PDH_HCOUNTER    m_DiskWrite = nullptr;
	PDH_HCOUNTER    m_NetDownload = nullptr;
	PDH_HCOUNTER    m_NetUpload = nullptr;
};
#endif