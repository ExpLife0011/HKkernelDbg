#include "DriverFunc.h"
NTSTATUS MyCreateDevice(PDRIVER_OBJECT pDriverObj)
{
	NTSTATUS status;

	UNICODE_STRING uDeviceName;
	UNICODE_STRING uLinkName;
	::RtlInitUnicodeString(&uDeviceName,DeviceName);
	::RtlInitUnicodeString(&uLinkName,LinkName);

	PDEVICE_OBJECT pDeviceObject;

	status=IoCreateDevice(\
		pDriverObj,\
		NULL,\
		&uDeviceName,\
		FILE_DEVICE_UNKNOWN,\
		NULL,\
		FALSE,\
		&pDeviceObject);
	if(!NT_SUCCESS(status))
	{
		DbgPrint("�����豸ʧ��");
		return status;
	}

	status=IoCreateSymbolicLink(&uLinkName,&uDeviceName);
	if(!NT_SUCCESS(status))
	{
		DbgPrint("������������ʧ��");
		IoDeleteDevice(pDeviceObject);

	}
		return status;
}
VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING uLinkName;
	::RtlInitUnicodeString(&uLinkName,LinkName);
	DbgPrint("��������ж�غ���");
	IoDeleteDevice(pDriverObj->DeviceObject);
	KIRQL irql=::KeGetCurrentIrql();
	
	if((DWORD)KernelCopyAddr)
	{
		//�ͷ�������ڴ�;
		   if(irql<=DISPATCH_LEVEL)
		     ExFreePool(KernelCopyAddr);
		   else
		   {
			   DbgPrint("irql<DISPATCH_LEVEL ( UNLOAD )irql=0x%x",irql);
		   }
	};
	IoDeleteSymbolicLink(&uLinkName);
}