#include "DeviceIoControlFunc.h"
BOOL IsAlreadyInit=FALSE;
DWORD KernelBaseInDll=NULL;//Dll�еĸ����ں˴����ַ������COPYһ�ݵ��ں�
DWORD KernelSizeInMemory=NULL;//�ں����ڴ��еĴ�С
DWORD PoolType=-1;

LPVOID KernelCopyAddr=NULL;//�ں˵�ַ�и����ں˵ĵ�ַ
DWORD KernelAddr=0;//��ʵ�ں˵ĵ�ַ

VOID CtrlCodeFunc_IoRequest(\
	PIRP pIrp,\
	PIO_STACK_LOCATION pstack,\
	PULONG pinfo\
	)
{           

    *pinfo=4;//OutBuf�������С��OutBuf[0]=�ں˺���ִ�з��صĽ����ֻ��Ϊ0����1
    NTSTATUS status=0xeeeeeeee;//���ⶨ����һ������
	DWORD FuncAddr=0;//�ҵ�copy���ں˺����ĵ�ַ��
	int* InBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
	int* OutBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
	if(!IsAlreadyInit)
	{
		DbgPrint("��ʼ����δ��ɻ���ʧ����");
		OutBuf[0]=status;
		return;
	}
	DWORD ArgNum=InBuf[0];//���������ĸ���
	DWORD FuncOffset=InBuf[ArgNum+1];//���һ��INBUF�Ǻ�����ƫ��
	BOOL ShouldBeInKernelMode=InBuf[ArgNum+2];

//	DbgPrint("ArgNum=%d",ArgNum);


	if(FuncOffset==NULL)
	{
		DbgPrint("����ƫ��Ϊ0");
		OutBuf[0]=FALSE;
		return;
	}
	//��ʵ�ĵ�ַ����ƫ�Ƽ��ϸ����ں˵ĵ�ַ
	FuncAddr=FuncOffset+(DWORD)KernelCopyAddr;
//	DbgPrint("FuncAddr=0x%08x");
	//�������ã�����ȡ�ں˺����ķ��ص�status
	PMYTHREAD ptk;
	char Cur_Mode;
	ptk=(PMYTHREAD)::KeGetCurrentThread();
	Cur_Mode=ptk->PreviousMode;//���浱ǰ״̬
	if(ShouldBeInKernelMode)
	{
		ptk->PreviousMode=KernelMode;//��Ϊ�ں�̬
	}
	else
	{
		ptk->PreviousMode=UserMode;//��Ϊ�û�̬
	}

           __asm pushad
		   __asm
		   {
			   mov ecx,ArgNum
			   mov eax,InBuf
PushNextArg:
			   mov edx,ecx
			   shl edx,2
			   add eax,edx
			   mov ebx,[eax]
			   push ebx
			   sub eax,edx
			   dec ecx
			   cmp ecx,0
			   jne PushNextArg
			   call FuncAddr
			   mov status,eax

		   }
		   __asm popad

		   ptk->PreviousMode=Cur_Mode;//�ָ�֮ǰ��״̬

		   if(NT_ERROR(status))
		   {
			   DbgPrint("ָ���ĺ�������ʧ�����ں��� error=0x%x",status);
		       
		   }
	       OutBuf[0]=status;

}

VOID CtrlCodeFunc_IoInit(\
	PIRP pIrp,\
	PIO_STACK_LOCATION pstack,\
	PULONG pinfo\
	)
{
	BOOL bRet=FALSE;
	if(IsAlreadyInit)
	{
		DbgPrint("��ʼ������ֻ��Ҫ����һ��");
		return ;
	}
	int* InBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
	KernelBaseInDll=InBuf[0];
	KernelSizeInMemory=InBuf[1];


//	DbgPrint("kernelBaseindll=0x%08x",KernelBaseInDll);
//	DbgPrint("KernelSizeInMemory=0x%08x",KernelSizeInMemory);
	if(KernelCopyAddr==NULL||KernelAddr==NULL)
	{
		DbgPrint("Init��ǲ������KernelCopyAddrΪ�ջ����ں˻���ַΪ��");
		return ;
	}

	KIRQL c_irql=::KeGetCurrentIrql();
	//PKIRQL p_OldIrql;
//	DbgPrint("current irql ��ǲ����=0x%x",c_irql);

	//��ʼ�ں˵�COPY
  if(c_irql>=DISPATCH_LEVEL)
  {
	  DbgPrint("current_irql>=DISPATCH_LEVEL in INIT Function");
	  goto endcopy;
  }

  RtlCopyBytes(KernelCopyAddr,(VOID*)KernelBaseInDll,KernelSizeInMemory);
	//����ʵ�ں˵Ļ�ַ��KernelAddr���ض�λ������KernelCopyAddr��������
	bRet=::DataRelocated((DWORD)KernelCopyAddr,KernelAddr);
	if(bRet)
	{
	  ::IsAlreadyInit=TRUE;
	}
	else
	{
		DbgPrint("�ض�λ���޸Ĵ���");
	}  
endcopy:
	DbgPrint("��ʼ��״̬=%d   (1��ʾ�ɹ� 0��ʾʧ��)",IsAlreadyInit);

	int* OutBuf=(int*)pIrp->AssociatedIrp.SystemBuffer;
}