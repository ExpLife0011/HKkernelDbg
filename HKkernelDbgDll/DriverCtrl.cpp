#include "DriverCtrl.h"
BOOL LoadNtDriver(LPCTSTR lpServiceName,LPCTSTR lpFullPathSys)
{
	BOOL bRet=FALSE;
	SC_HANDLE h_Service=NULL;
	SC_HANDLE h_Scm=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(h_Scm==NULL)
	{
	   MyDbgString("OpenSCManager���� error=%d",ERROR_CODE);
	   goto Loadend;
	}
	h_Service=CreateService( \
		h_Scm,       // handle to SCM database 
		lpServiceName,      // name of service to start
		lpServiceName,      // display name
		SERVICE_ALL_ACCESS,      // type of access to service
		SERVICE_KERNEL_DRIVER,        // type of service
		SERVICE_DEMAND_START,          // when to start service
		SERVICE_ERROR_NORMAL,       // severity of service failure
		lpFullPathSys,   // name of binary file
		NULL,   // name of load ordering group
		NULL,          // tag identifier
		NULL,     // array of dependency names
		NULL, // account name 
		NULL          // account password
		);
	if(h_Service==NULL)
	{
		DWORD dwError=ERROR_CODE;
		if(dwError!=ERROR_SERVICE_EXISTS)
		{
			MyDbgString("��������ʧ�� error=%d",dwError);
			goto Loadend;
		}
		OutputDebugStringA("������ڣ�ֻ��Ҫ��");
		h_Service=OpenService(h_Scm,lpServiceName,SERVICE_ALL_ACCESS);
		if(h_Service==NULL)
		{
			MyDbgString("�򿪷���ʧ�� error=%d",ERROR_CODE);
			goto Loadend;
		}
	}

	bRet=StartService(h_Service,NULL,NULL);
	if(!bRet)
	{
		MyDbgString("��������ʧ��error=%d",ERROR_CODE);
	}
	else
	{
		OutputDebugStringA("�ɹ���������");
	}
		bRet=TRUE;
Loadend:

	if(h_Scm)
	{
		CloseServiceHandle(h_Scm);
	}
	if(h_Service)
	{
		CloseServiceHandle(h_Service);
	}
	return bRet;
};
BOOL UnLoadNtDriver(LPCTSTR lpServiceName)
{
	BOOL bRet=FALSE;
	SC_HANDLE h_Service=NULL;
	SC_HANDLE h_Scm=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(h_Scm==NULL)
	{
		MyDbgString("OpenSCManager���� error=%d",ERROR_CODE);
		goto UnLoadend;
	}
	h_Service=OpenService(h_Scm,lpServiceName,SERVICE_ALL_ACCESS);
	if(h_Service==NULL)
	{
		MyDbgString("�򿪷������ UNLOAD error=%d",ERROR_CODE);
		goto UnLoadend;
	}
	SERVICE_STATUS status; 
	bRet=ControlService(h_Service,SERVICE_CONTROL_STOP,&status);
	if(!bRet)
	{
		MyDbgString("ֹͣ����ʧ��Unload %d",ERROR_CODE);
		goto UnLoadend;
	}
	bRet=DeleteService(h_Service);
	if(!bRet)
	{
		MyDbgString("ɾ������ʧ��Unload %d",ERROR_CODE);
		goto UnLoadend;
	}
	bRet=TRUE;
	OutputDebugStringA("ж�������ɹ�");
UnLoadend:
	if(h_Scm)
	{
		CloseServiceHandle(h_Scm);
	}
	if(h_Service)
	{
		CloseServiceHandle(h_Service);
	}
	return bRet;
};