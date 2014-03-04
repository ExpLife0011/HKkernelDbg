#include "ReLoc.h"


PIMAGE_DOS_HEADER  pDosHeader;
PIMAGE_NT_HEADERS32  pNtHeader;
PIMAGE_OPTIONAL_HEADER  pOptionalHeader;
PIMAGE_DATA_DIRECTORY  pRelocDirectory;


char* pBaseReloc;

BOOL DataRelocated(DWORD ModuleAddress,DWORD RealModuleAddress)
{
	BOOL bRet=FALSE;
	DWORD ImageBase=NULL;

	void* pRelocTableAddr=NULL;
	DWORD  RelocTableSize=NULL;

	pDosHeader=(PIMAGE_DOS_HEADER)ModuleAddress;
	pNtHeader=(PIMAGE_NT_HEADERS32)(pDosHeader->e_lfanew+ModuleAddress);
	pOptionalHeader=&(pNtHeader->OptionalHeader);

	ImageBase=pOptionalHeader->ImageBase;


    pRelocDirectory=&pOptionalHeader->DataDirectory[5];//�ض�λ��ṹ�ǵ����

	RelocTableSize=pRelocDirectory->Size;
	pRelocTableAddr=(void *)(pRelocDirectory->VirtualAddress+ModuleAddress);

//	DbgPrint("�ض�λ���ַΪ=0x%08x",pRelocTableAddr);

	if(RelocTableSize==0||(pRelocDirectory->VirtualAddress)==0)
	{
		DbgPrint("û����Ҫ�ض�λ������");
		return FALSE;
	}

	DWORD ReDataNum=0;//ÿ�������Ķ�λ�����Ҫ�ض�λ�����ݵĸ���
	DWORD EveryAddr=0;

	pBaseReloc=(char*)pRelocTableAddr;





	do
	{
		
		ReDataNum=((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock;//��һ����Ĵ�С
	    EveryAddr=((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress;//��һ����������ַ ��û�м��ϻ�ַ)


		//����õ�ÿ����������Ҫ�ض�λ�����ݵĸ���
		ReDataNum=(ReDataNum-sizeof(IMAGE_BASE_RELOCATION))/2;

		WORD* pReData=(WORD*)((DWORD)pBaseReloc+8);

	

           for(DWORD i=0;i<ReDataNum;i++)
		   {
                if(((*pReData)>>12)==3)
				{
                    WORD NewOffset=(*pReData)&0xfff;
					DWORD Value=*(DWORD *)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress);
					Value=Value-ImageBase+RealModuleAddress;
					*(DWORD *)(NewOffset+ModuleAddress+((PIMAGE_BASE_RELOCATION)pBaseReloc)->VirtualAddress)=Value;

				}
				pReData++;

		   }
		   pBaseReloc=pBaseReloc+((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock;

	}while(((PIMAGE_BASE_RELOCATION)pBaseReloc)->SizeOfBlock);

	return TRUE;
};