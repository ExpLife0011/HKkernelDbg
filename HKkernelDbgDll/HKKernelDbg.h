// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� HKKERNELDBG_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// HKKERNELDBG_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once
#ifdef HKKERNELDBG_EXPORTS
#define HKKERNELDBG_API __declspec(dllexport)
#else
#define HKKERNELDBG_API __declspec(dllimport)
#endif

#include "CallInKernel.h"
#pragma warning(disable:4273)
extern "C" HKKERNELDBG_API HANDLE  HKOpenProcess(  \
	                  DWORD dwDesiredAccess,\
					  BOOL bInheritHandle,\
					  DWORD dwProcessId\
					  );
extern "C" HKKERNELDBG_API DWORD  HKNtDebugActiveProcess(HANDLE hProcess,HANDLE DbgHandle);

extern "C" HKKERNELDBG_API HANDLE  HKOpenThread(\
	DWORD dwDesiredAccess,\
	BOOL bInheritHandle,\
	DWORD dwThreadId\
	);
extern "C" HKKERNELDBG_API BOOL  HKNtReadVirtualMemory( \
	HANDLE hProcess,\
	LPCVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesRead\
	);

extern "C" HKKERNELDBG_API BOOL  HKNtWriteVirtualMemory(\
	HANDLE hProcess,\
	LPVOID lpBaseAddress,\
	LPVOID lpBuffer,\
	DWORD nSize,\
	LPDWORD lpNumberOfBytesWritten
	);

extern "C" HKKERNELDBG_API DWORD  HKNtQueryInformationProcess(\
	HANDLE hProcess,\
	ULONG EnumClass,\
	PVOID ProcessInfo,\
	ULONG InfoLength,\
	PULONG ReturnLength);

extern "C" HKKERNELDBG_API DWORD  HKNtQuerySystemInformation(\
	DWORD SystemInfoClass,\
	PVOID SystemInfo,\
	ULONG InfoLength,\
	ULONG ReturnLength);

extern "C" HKKERNELDBG_API DWORD  HKNtQueryObject(\
	HANDLE hanlde,\
	DWORD EnumClass,\
	PVOID ObjInfo,\
	ULONG length,\
	PULONG ReturnLength);

extern "C" HKKERNELDBG_API DWORD  HKNtSetContextThread(\
	HANDLE hThread,\
	LPCONTEXT lpContext
	);
extern "C" HKKERNELDBG_API DWORD  HKNtGetContextThread( \
	HANDLE hThread,\
	LPCONTEXT lpContext
	);

extern "C" HKKERNELDBG_API DWORD  HKNtSetInformationThread(\
	HANDLE hThread,\
	DWORD EnumThreadClass,\
	PVOID ThreadInfo,\
	ULONG Length);