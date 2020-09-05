#include "ring3.hpp"

namespace ring3
{

	NTSTATUS KeVirtualAllocEx(PEPROCESS Process, PVOID* BaseAddress, ULONG_PTR ZeroBits, PSIZE_T RegionSize, ULONG AllocationType, ULONG Protect)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		NTSTATUS ret = STATUS_ACCESS_DENIED;
		if (NULL != Process)
		{
			KeAttachProcess(Process);

			ret = ZwAllocateVirtualMemory(NtCurrentProcess(), BaseAddress, ZeroBits, RegionSize, AllocationType, Protect);

			KeDetachProcess();
		}

		if (!NT_SUCCESS(ret))
		{
			DbgPrint(" [-] KeVirtualAllocEx failed!"	"\n");
			DbgPrint(" [ ] pEPROCESS:       %p"			"\n", Process);
			DbgPrint(" [ ] ppBase:          %p"			"\n", BaseAddress);
			DbgPrint(" [ ] pBase:           %p"			"\n", *BaseAddress);
			DbgPrint(" [ ] ullZeroBits:     %llu"		"\n", ZeroBits);
			DbgPrint(" [ ] ullSize:         %llu"		"\n", *RegionSize);
			DbgPrint(" [ ] uAllocationType: %u"			"\n", AllocationType);
			DbgPrint(" [ ] uProtect:        %u"			"\n", Protect);
			DbgPrint(" [ ] ret:				%#lx"		"\n", ret);
		}

		return ret;
	}

	NTSTATUS LockAndReadProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		auto st = STATUS_SUCCESS;

		DbgPrint(" [ ] pEPROCESS: %p"   "\n", Process);
		DbgPrint(" [ ] pSource:   %p"   "\n", SourceAddress);
		DbgPrint(" [ ] pTarget:   %p"   "\n", TargetAddress);
		DbgPrint(" [ ] uSize:     %llu" "\n", Size);

		if (
			!Process ||
			!SourceAddress ||
			!TargetAddress ||
			!Size
			)
		{
			DbgPrint(" [-] KeWriteProcessMemory failed!" "\n");
			return STATUS_ACCESS_DENIED;
		}

		auto pMdl = IoAllocateMdl(SourceAddress, (ULONG)Size, FALSE, TRUE, NULL);
		DbgPrint(" [ ] pMdl:      %p" "\n", pMdl);
		if (!pMdl)
		{
			DbgPrint(" [-] KeWriteProcessMemory failed!" "\n");
			return STATUS_INSUFFICIENT_RESOURCES;
		}
		
		TRY
		{
			// >
			KeAttachProcess(Process);
			MmProbeAndLockPages(pMdl, UserMode, IoReadAccess);
			KeDetachProcess();
			// >
		}
		CATCH 
		{
			DbgPrint(" [-] MmProbeAndLockPages failed!" "\n");
			st = GetExceptionCode();
			DbgPrint(" [ ] ExceptionCode: 0X%08X" "\n", st);
			IoFreeMdl(pMdl);
			return st;
		}

		PVOID pMapped = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority | MdlMappingNoExecute );
		if (!pMapped) {
			DbgPrint(" [-] MmGetSystemAddressForMdlSafe failed!" "\n");
			st = STATUS_INSUFFICIENT_RESOURCES;
			MmUnlockPages(pMdl);
			IoFreeMdl(pMdl);
			return st;
		}
		DbgPrint(" [ ] pMapped:      %p" "\n", pMapped);

		//
		// Now you can 'safely' <~(0_,0<~)  read the data from the buffer.
		//
		TRY
		{
			RtlCopyMemory(TargetAddress, pMapped, Size);
		}
		CATCH
		{
			DbgPrint(" [-] RtlCopyMemory failed!" "\n");
			st = GetExceptionCode();;
			MmUnlockPages(pMdl);
			IoFreeMdl(pMdl);
			return st;
		}

		//
		// Once the read is over unmap and unlock the pages.
		//

		MmUnlockPages(pMdl);
		IoFreeMdl(pMdl);

		return STATUS_SUCCESS;
	}

	NTSTATUS LockAndWriteProcessMemory(PEPROCESS Process, PVOID SourceAddress, PVOID TargetAddress, SIZE_T Size)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		NTSTATUS ret = STATUS_ACCESS_DENIED;
		if (NULL != Process)
		{
			KeAttachProcess(Process);

			PEPROCESS SourceProcess = PsGetCurrentProcess();
			PEPROCESS TargetProcess = Process;
			SIZE_T Result;
			ret = MmCopyVirtualMemory(SourceProcess, SourceAddress, TargetProcess, TargetAddress, Size, KernelMode, &Result);

			KeDetachProcess();
		}

		if (!NT_SUCCESS(ret))
		{
			DbgPrint(" [-] KeWriteProcessMemory failed!" "\n");
			DbgPrint(" [ ] pEPROCESS: %p"   "\n", Process);
			DbgPrint(" [ ] pSource:   %p"   "\n", SourceAddress);
			DbgPrint(" [ ] pTarget:   %p"   "\n", TargetAddress);
			DbgPrint(" [ ] uSize:     %llu" "\n", Size);
			DbgPrint(" [ ] ret:       %#lx" "\n", ret);
		}

		return ret;
	}

	NTSTATUS FindProcessByName(char* process_name, PEPROCESS* process)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		char clipped_orig[15] = { 0 };
		memcpy(clipped_orig, process_name, min(strlen(process_name), 14));

		PEPROCESS sys_process = PsInitialSystemProcess;
		PEPROCESS cur_entry = sys_process;

		DbgPrint(" [ ] Searching for Process \"%s\" (\"%s\")..." "\n", process_name, clipped_orig);

		do
		{
			char copy[15] = { 0 };
			memcpy(copy, (char*)((uintptr_t)(cur_entry)+0x450) /*EPROCESS->ImageFileName*/, 15);

			if (strstr(copy, clipped_orig))
			{
				*process = cur_entry;
				DbgPrint(" [+] \"%s\": %p" "\n", copy, cur_entry);
				return STATUS_SUCCESS;
			}

			PLIST_ENTRY list = (PLIST_ENTRY)((uintptr_t)(cur_entry)+0x2F0) /*EPROCESS->ActiveProcessLinks*/;
			cur_entry = (PEPROCESS)((uintptr_t)list->Flink - 0x2F0) /*EPROCESS->ActiveProcessLinks*/;

		} while (cur_entry != sys_process);

		DbgPrint(" [-] Process not found!" "\n");
		return STATUS_NOT_FOUND;
	}

	PVOID KeGetUserModule(PEPROCESS Process, PWCH wch)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		ASSERT(Process != NULL);
		if (Process == NULL)
			return NULL;

		DbgPrint(" [ ] Searching for Module \"%ls\"..." "\n", wch);

		UNICODE_STRING ModuleName = { 0 };
		RtlInitUnicodeString(&ModuleName, wch);

		KeAttachProcess(Process);

		auto pPeb = PsGetProcessPeb(Process);

		if (!pPeb)
		{
			KeDetachProcess();
			DbgPrint(" [-] PEB not found!" "\n");
			DbgPrint(" [-] Export not found!" "\n");
			return NULL;
		}

		if (!pPeb->LoaderData)
		{
			KeDetachProcess();
			DbgPrint(" [-] Loader was not initialized in time!" "\n");
			DbgPrint(" [-] Export not found!" "\n");
			return NULL;
		}

		for (PLIST_ENTRY pListEntry = pPeb->LoaderData->InLoadOrderModuleList.Flink;
			pListEntry != &pPeb->LoaderData->InLoadOrderModuleList;
			pListEntry = pListEntry->Flink)
		{
			PLDR_DATA_TABLE_ENTRY pEntry = CONTAINING_RECORD(pListEntry, LDR_DATA_TABLE_ENTRY, InLoadOrderModuleList);

			if (RtlCompareUnicodeString(&pEntry->BaseDllName, &ModuleName, TRUE) == 0)
			{
				KeDetachProcess();
				DbgPrint(" [+] \"%ls\": %p" "\n", wch, pEntry->DllBase);
				return pEntry->DllBase;
			}
		}

		KeDetachProcess();
		DbgPrint(" [-] Export not found!" "\n");
		return NULL;
	}

	PVOID KeGetModuleExport(IN PVOID pBase, IN PCCHAR name_ord, IN PEPROCESS pProcess)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)pBase;
		PIMAGE_NT_HEADERS32 pNtHdr32 = NULL;
		PIMAGE_NT_HEADERS64 pNtHdr64 = NULL;
		PIMAGE_EXPORT_DIRECTORY pExport = NULL;
		ULONG expSize = 0;
		ULONG_PTR pAddress = 0;

		ASSERT(pBase != NULL);
		if (pBase == NULL)
			return NULL;

		DbgPrint(" [ ] Searching for Export \"%s\"..." "\n", name_ord);

		KeAttachProcess(pProcess);

		/// Not a PE file
		if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
		{
			KeDetachProcess();
			DbgPrint(" [-] IMAGE_DOS_SIGNATURE incorrect!" "\n");
			DbgPrint(" [-] Export not found!" "\n");
			return NULL;
		}

		pNtHdr32 = (PIMAGE_NT_HEADERS32)((PUCHAR)pBase + pDosHdr->e_lfanew);
		pNtHdr64 = (PIMAGE_NT_HEADERS64)((PUCHAR)pBase + pDosHdr->e_lfanew);

		// Not a PE file
		if (pNtHdr32->Signature != IMAGE_NT_SIGNATURE)
		{
			KeDetachProcess();
			DbgPrint(" [-] IMAGE_NT_SIGNATURE incorrect!" "\n");
			DbgPrint(" [-] Export not found!" "\n");
			return NULL;
		}

		// 64 bit image
		if (pNtHdr32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
		{
			pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)pBase);
			expSize = pNtHdr64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		}
		// 32 bit image
		else
		{
			pExport = (PIMAGE_EXPORT_DIRECTORY)(pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress + (ULONG_PTR)pBase);
			expSize = pNtHdr32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		}

		PUSHORT pAddressOfOrds = (PUSHORT)(pExport->AddressOfNameOrdinals + (ULONG_PTR)pBase);
		PULONG  pAddressOfNames = (PULONG)(pExport->AddressOfNames + (ULONG_PTR)pBase);
		PULONG  pAddressOfFuncs = (PULONG)(pExport->AddressOfFunctions + (ULONG_PTR)pBase);

		for (ULONG i = 0; i < pExport->NumberOfFunctions; ++i)
		{

			if (i < pExport->NumberOfNames)
			{

				PCHAR pName = (PCHAR)(pAddressOfNames[i] + (ULONG_PTR)pBase);
				USHORT oIndex = pAddressOfOrds[i];

				if (strcmp(pName, name_ord) == 0)
				{
					pAddress = pAddressOfFuncs[oIndex] + (ULONG_PTR)pBase;
					KeDetachProcess();

					DbgPrint(" [+] \"%s\": %p" "\n", name_ord, (PVOID)pAddress);

					return (PVOID)pAddress;
				}

			}
			else
			{
				//Ordinals
				DbgPrint(" [-] Ordinals not implemented!" "\n");
				break;
			}
		}

		KeDetachProcess();

		DbgPrint(" [-] Export not found!" "\n");

		return NULL;
	}

	NTSTATUS BBExecuteInNewThread(
		IN PVOID pBaseAddress,
		IN PVOID pParam,
		IN ULONG flags,
		IN BOOLEAN wait,
		OUT PNTSTATUS pExitStatus
	)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		NTSTATUS status = STATUS_CANCELLED;
		__try
		{
			HANDLE hThread = NULL;
			OBJECT_ATTRIBUTES ob = { 0 };

			InitializeObjectAttributes(&ob, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

			pRtlCreateUserThread RtlCreateUserThread = NULL;
			if (RtlCreateUserThread == NULL)
			{
				UNICODE_STRING	RtlCreateUserThreadStr = RTL_CONSTANT_STRING(L"RtlCreateUserThread");
				RtlCreateUserThread = (pRtlCreateUserThread)MmGetSystemRoutineAddress((PUNICODE_STRING)&RtlCreateUserThreadStr);
			}

			if (NULL == RtlCreateUserThread)
			{
				DbgPrint(" [-] RtlCreateUserThread not found!" "\n");
				DbgPrint(" [-] Thread not executed!" "\n");
				return STATUS_INTERNAL_ERROR;
			}

			CLIENT_ID           ClientID = { 0 };
			status = RtlCreateUserThread(NtCurrentProcess(),
				NULL,
				FALSE,
				NULL,
				NULL,
				NULL,
				pBaseAddress,
				pParam,
				&hThread,
				&ClientID);

			// Wait for completion
			if (NT_SUCCESS(status) && wait != FALSE)
			{
				// Force 60 sec timeout
				LARGE_INTEGER timeout = { 0 };
				timeout.QuadPart = -(60ll * 10 * 1000 * 1000);

				status = ZwWaitForSingleObject(hThread, TRUE, &timeout);
				if (NT_SUCCESS(status))
				{
					THREAD_BASIC_INFORMATION info = { 0 };
					ULONG bytes = 0;

					status = ZwQueryInformationThread(hThread, ThreadBasicInformation, &info, sizeof(info), &bytes);
					if (NT_SUCCESS(status) && pExitStatus)
					{
						*pExitStatus = info.ExitStatus;
					}
					else if (!NT_SUCCESS(status))
					{
						DbgPrint(" [-] ZwQueryInformationThread failed with status 0x%X\n", status);
					}
				}
				else
					DbgPrint(" [-] ZwWaitForSingleObject failed with status 0x%X\n", status);
			}
			else
				DbgPrint(" [-] ZwCreateThreadEx failed with status 0x%X\n", status);

			if (hThread)
				ZwClose(hThread);

		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{

		}

		if (!NT_SUCCESS(status))
		{
			DbgPrint(" [-] BBExecuteInNewThread failed!" "\n");
			DbgPrint(" [ ] pBaseAddress: %p"    "\n", pBaseAddress);
			DbgPrint(" [ ] pParam:       %p"    "\n", pParam);
			DbgPrint(" [ ] uFlags:       %u"    "\n", flags);
			DbgPrint(" [ ] bWait:        %u"    "\n", wait);
			DbgPrint(" [ ] status:       %#lx" "\n", status);
		}

		return status;
	}

	NTSTATUS KeExecuteTargetRoutine(IN PEPROCESS Process, IN PVOID pRoutine, IN PVOID pParam, OUT PNTSTATUS pExitStatus)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		KeAttachProcess(Process);
		auto st = ring3::BBExecuteInNewThread(pRoutine, pParam, 0, TRUE, (PNTSTATUS)pExitStatus);
		KeDetachProcess();

		return st;
	}

	VOID _AttachProcess(PRKPROCESS Process)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
#undef  KeAttachProcess()
		KeAttachProcess(Process);
#define KeAttachProcess(Process) ring3::_AttachProcess(Process)
	}

	VOID _DetachProcess(VOID)
	{
#undef  KeDetachProcess()
		KeDetachProcess();
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
#define KeDetachProcess() ring3::_DetachProcess()
	}

	NTSTATUS MapRing0MemoryToRing3(IN PEPROCESS Process, SIZE_T uSize, OUT PVOID* ppRing3, OUT PMDL pMdl)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		return STATUS_SUCCESS;
	}

	NTSTATUS UnMapRing0MemoryInRing3(IN PEPROCESS Process)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		return STATUS_SUCCESS;
	}
}