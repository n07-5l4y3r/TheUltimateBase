#pragma once

#include <inc.hpp>

namespace ring3
{
	NTSTATUS FindProcessByName(char* process_name, PEPROCESS* process);

	NTSTATUS KeVirtualAllocEx(PEPROCESS Process, PVOID* BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect);
	NTSTATUS KeVirtualFreeEx(PEPROCESS Process, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG FreeType);

	NTSTATUS LockAndReadProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size);
	NTSTATUS LockAndWriteProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size);

#   define KeReadProcessMemory LockAndReadProcessMemory
#   define KeWriteProcessMemory LockAndWriteProcessMemory

	PVOID KeGetUserModule(PEPROCESS Process, PWCH wch);
	PVOID KeGetModuleExport(IN PVOID pBase, IN PCCHAR name_ord, IN PEPROCESS pProcess);

	NTSTATUS KeExecuteTargetRoutine(
		IN PEPROCESS Process,
		IN PVOID pRoutine,
		IN PVOID pParam,
		OUT PNTSTATUS pExitStatus);

#define KeAttachProcess(Process) ring3::_AttachProcess(Process)
	VOID _AttachProcess( PRKPROCESS Process );
#define KeDetachProcess() ring3::_DetachProcess()
	VOID _DetachProcess( VOID );

	NTSTATUS   MapRing0MemoryToRing3(IN CONST PEPROCESS Process, IN PVOID pRing0, OUT PVOID* ppRing3, OUT PMDL pMdl, IN CONST SIZE_T uSize);
	NTSTATUS UnMapRing0MemoryInRing3(IN CONST PEPROCESS Process);

}