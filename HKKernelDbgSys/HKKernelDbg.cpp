extern "C"
{
#include <ntddk.h>
}
#include "IncAll.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObj, PUNICODE_STRING pRegistryString)
{
	DbgPrint("����DriverEntry����");
	NTSTATUS status;
	
	status=MyCreateDevice(pDriverObj);
	pDriverObj->DriverUnload=DriverUnload;
	pDriverObj->MajorFunction[IRP_MJ_CREATE]=OtherDispatchDeviceControl;
	pDriverObj->MajorFunction[IRP_MJ_CLOSE]=OtherDispatchDeviceControl;
	pDriverObj->MajorFunction[IRP_MJ_READ]=OtherDispatchDeviceControl;
	pDriverObj->MajorFunction[IRP_MJ_WRITE]=OtherDispatchDeviceControl;
	//��Ҫ��ͨ�Ŵ�����
	pDriverObj->MajorFunction[IRP_MJ_DEVICE_CONTROL]=DeviceIoControlFunc;
///////
//���»�ȡ�ں˻�ַ��ASM������COPY ���ϵġ���лԭ���ߡ�
/////////

	__asm
	{
		pushad
		mov eax,DWORD PTR FS:[0x34]
		add eax,0x18
		mov eax,[eax]
		mov eax,[eax]
		mov eax,[eax+0x18]
		mov KernelAddr,eax
		popad
	}
	
//	DbgPrint("��ʵ���ں˵�ַΪ=0x%08x",KernelAddr);
	PIMAGE_DOS_HEADER  pDosHeader;
	PIMAGE_NT_HEADERS32  pNtHeader;
	PIMAGE_OPTIONAL_HEADER  pOptionalHeader;

	pDosHeader=(PIMAGE_DOS_HEADER)KernelAddr;
	pNtHeader=(PIMAGE_NT_HEADERS32)(pDosHeader->e_lfanew+KernelAddr);
	pOptionalHeader=&(pNtHeader->OptionalHeader);
	//����ʵ�ʵ��ں˼��غ�Ĵ�С�����ڴ�
	DWORD SizeOfImage=pOptionalHeader->SizeOfImage;
//	DbgPrint("Sizeofimage=0x%08x",SizeOfImage);
	KIRQL c_irql=::KeGetCurrentIrql();
//	DbgPrint("current irql driver entry=0x%x",c_irql);
	if(c_irql==DISPATCH_LEVEL)
	{	
		PoolType=NonPagedPool;
		KernelCopyAddr=ExAllocatePool(NonPagedPool,SizeOfImage);
		RtlZeroMemory(KernelCopyAddr,SizeOfImage);
	}
	else if(c_irql<DISPATCH_LEVEL)
	{
		PoolType=PagedPool;
		KernelCopyAddr=ExAllocatePool(PagedPool,SizeOfImage);
		RtlZeroMemory(KernelCopyAddr,SizeOfImage);
	}
	else
	{
		DbgPrint("Current IRQL>DISPATCH_LEVEL in DriverEntry");
	}

//	DbgPrint("POOLTYPE=%d",PoolType);

//	DbgPrint("���䵽���ڴ�Ϊ0x%08x",KernelCopyAddr);
	if(KernelCopyAddr==NULL)
	{
		DbgPrint("�����ں��ڴ����");
	}
	if(!NT_SUCCESS(status))
	{
		return status;
	}
	return STATUS_SUCCESS;
}
