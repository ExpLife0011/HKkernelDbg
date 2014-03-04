#include "IrpFunc.h"
#include "Struct.h"

NTSTATUS OtherDispatchDeviceControl(PDEVICE_OBJECT  pDeviceObject,PIRP  pIrp)
{
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DeviceIoControlFunc(PDEVICE_OBJECT  pDeviceObject,PIRP  pIrp)
{
	/*
	Ϊ�˱�֤ÿ���ں˺����ܹ���ȷִ�С�������0XC0000005��STATUS������
	PMYTHREAD Ϊ�Զ����KTHREAD������windowsû�о��嶨�塣����windbg�鵽��
	ͬʱΪ�˲�����̫��Ľṹ�壬�Ұ�һЩ�ṹ��ָ���Ϊ��DWORD������ֻ��ҪPreviousMode
	*/


//	DbgPrint("����IRP DeviceIoControl��ǲ����");
	ULONG info=0;
	PIO_STACK_LOCATION pStackLocation=IoGetCurrentIrpStackLocation(pIrp);

	switch(pStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
		/*
		���ݲ�ͬ��CODE���벻ͬ�ĺ���ִ����Ӧ����ǲ����
		*/
	case IoRequest_code:
		CtrlCodeFunc_IoRequest(pIrp,pStackLocation,&info);
		break;
	case IoInit_code:
		CtrlCodeFunc_IoInit(pIrp,pStackLocation,&info);
		break;
	}

	pIrp->IoStatus.Information=info;
	pIrp->IoStatus.Status=STATUS_SUCCESS;
	IoCompleteRequest(pIrp,IO_NO_INCREMENT);



	return STATUS_SUCCESS;
}