#include "GetFuncOffsetSSDT.h"
DWORD *SSDTAddress=NULL;
PIMAGE_DOS_HEADER  pDosHeaderSSDT;
PIMAGE_NT_HEADERS32  pNtHeaderSSDT;
PIMAGE_OPTIONAL_HEADER  pOptionalHeaderSSDT;
PIMAGE_DATA_DIRECTORY  pRelocDirectorySSDT;


char* pBaseReloc;
//����NTDLL����
BOOL InitSSDTTableAddr(DWORD ModuleAddress,\
	DWORD CheckOpenProc,DWORD CheckOpenThread)
{	
BOOL bRetT=FALSE;
BOOL bRetP=FALSE;
DWORD ImageBase=NULL;

void* pRealocTableAddr=NULL;
DWORD  RealocTableSize=NULL;

DWORD *AddrOfNTP=NULL;
DWORD *AddrOfNTT=NULL;

pDosHeaderSSDT=(PIMAGE_DOS_HEADER)ModuleAddress;
pNtHeaderSSDT=(PIMAGE_NT_HEADERS32)(pDosHeaderSSDT->e_lfanew+ModuleAddress);
pOptionalHeaderSSDT=&(pNtHeaderSSDT->OptionalHeader);

ImageBase=pOptionalHeaderSSDT->ImageBase;


pRelocDirectorySSDT=&pOptionalHeaderSSDT->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];

RealocTableSize=pRelocDirectorySSDT->Size;
pRealocTableAddr=(void *)(pRelocDirectorySSDT->VirtualAddress+ModuleAddress);


if(RealocTableSize==0||(pRelocDirectorySSDT->VirtualAddress)==0)
{
	OutputDebugStringA("û���ض�λ��IN GET SSDT");
	return FALSE;
}

DWORD ReDataNum=0;
DWORD EveryAddr=0;

pBaseReloc=(char*)pRealocTableAddr;

int count=0;

do
{

	ReDataNum=((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock;//��һ����Ĵ�С
	EveryAddr=((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress;//��һ����������ַ ��û�м��ϻ�ַ)

	ReDataNum=(ReDataNum-sizeof(IMAGE_BASE_RELOCATION))/2;

	WORD* pReData=(WORD*)((DWORD)pBaseReloc+sizeof(IMAGE_BASE_RELOCATION));


	for(DWORD i=0;i<ReDataNum;i++)
	{
		if(((*pReData)>>12)==3)
		{
			count++;
			WORD NewOffset=(*pReData)&0xfff;
			DWORD Value=*(DWORD *)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress);

			if(Value==CheckOpenProc)
			{
				bRetP=TRUE;
				AddrOfNTP=(DWORD*)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress);
		//		OutputDebugStringA("CheckOpenProc OK");
			}
			if(Value==CheckOpenThread)
			{
		//		OutputDebugStringA("CheckOpenThread OK");
				bRetT=TRUE;
				AddrOfNTT=(DWORD*)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress);
			}

		}
		pReData++;

	}
	pBaseReloc=pBaseReloc+((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock;

}while(((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress);

if(bRetT&&bRetP)
{
	HANDLE hNtdll=GetModuleHandle(L"ntdll.dll");
	BYTE *NtDllOP=(BYTE*)GetProcAddress((HMODULE)hNtdll,"NtOpenProcess");
	BYTE *NtDllOT=(BYTE*)GetProcAddress((HMODULE)hNtdll,"NtOpenThread");

	if(NtDllOP==NULL||NtDllOT==NULL)
	{
		OutputDebugStringA("ERROR check  addr ");
		return FALSE;
	}
	DWORD IndexOP=0;
	DWORD IndexOT=0;
	NtDllOP++;
	NtDllOT++;
//	MyDbgString("NtDllOP=0x%08x",NtDllOP);
//	MyDbgString("NtDllOT=0x%08x",NtDllOT);
	IndexOP=*(DWORD*)(NtDllOP);
	IndexOT=*(DWORD*)(NtDllOT);

	if(IndexOP<0||IndexOP>0x200)
	{
		MyDbgString("IndexOP 's Index ERROR?? (Index=%d)",IndexOP);
		return FALSE;
	}

	if(IndexOT<0||IndexOT>0x200)
	{
		MyDbgString("IndexOT 's Index ERROR?? (Index=%d)",IndexOT);
		return FALSE;
	}

//	MyDbgString("openprocess index=0x%08x",IndexOP);
//	MyDbgString("openthread index=0x%08x",IndexOT);
	if((AddrOfNTP-IndexOP)==(AddrOfNTT-IndexOT))
	{
		SSDTAddress=(AddrOfNTP-IndexOP);
//		MyDbgString("SSDT���ַΪ0x%08x",SSDTAddress);
		return TRUE;
	}
	else
	{
		OutputDebugStringA("У�鲻һ��");
		return FALSE;
	}
	

}
OutputDebugStringA("û�з�����ȵĵ�ַ");
return FALSE;
};

DWORD GetAddressFromSSDTByName(LPCSTR FuncName)
{
	if(SSDTAddress==NULL)
	{
		OutputDebugStringA("SSDTADDRΪ��");
		return NULL;
	}
	HANDLE hNtdll=GetModuleHandle(L"ntdll.dll");
	if(hNtdll==NULL)
	{
		OutputDebugStringA("GetModulehanlde");
		return NULL;
	}
	BYTE* FuncAddr=(BYTE*)GetProcAddress((HMODULE)hNtdll,FuncName);
	if(FuncAddr==NULL)
	{
		OutputDebugStringA(FuncName);
		OutputDebugStringA("���Ϻ���û����NTDLL���ҵ�");
		return NULL;
	}
	FuncAddr++;
	DWORD Index=*(DWORD*)FuncAddr;
	DWORD Offset=0;

	if(Index<0||Index>0x200)
	{
		MyDbgString("%s 's Index ERROR?? (Index=%d)",FuncName,Index);
		return NULL;
	}

	Offset=SSDTAddress[Index];
	Offset=Offset-ImageBase;
	return Offset;

};