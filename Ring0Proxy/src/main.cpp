#include "main.hpp"

NTSTATUS quit(PSTR sReason, NTSTATUS reason)
{
	DbgPrint("%s" "\n", sReason);
	return reason;
}

unsigned __int64 ui64fgetCmdCB = 0ui64;
unsigned __int64 ui64falcRplCB = 0ui64;
unsigned __int64 ui64fsetRplCB = 0ui64;

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
	PrintIrql();

	fallocFarMemImpl = [](unsigned __int64* pui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		DbgPrint(" > " "fallocFarMemImpl" "\n");
		*pui64pDest = (unsigned __int64)VirtualAllocEx(hProcess, (LPVOID)*pui64pDest, ui64Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		return bool(!*pui64pDest);
	};
	DbgPrint(" + " "fallocFarMemImpl: %#p" "\n", fallocFarMemImpl);
	ffreeFarMemImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		DbgPrint(" > " "ffreeFarMemImpl" "\n");
		return VirtualFreeEx(hProcess, (LPVOID)ui64pDest, 0, MEM_RELEASE);
	};
	DbgPrint(" + " "ffreeFarMemImpl: %#p" "\n", ffreeFarMemImpl);
	//
	freadFarMemImpl = [](unsigned __int64 ui64pSrc, void* pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		DbgPrint(" > " "freadFarMemImpl" "\n");
		return ReadProcessMemory(hProcess, (LPCVOID)ui64pSrc, pDest, ui64Size, 0);
	};
	DbgPrint(" + " "freadFarMemImpl: %#p" "\n", freadFarMemImpl);
	fwriteFarMemImpl = [](void* pSrc, unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		DbgPrint(" > " "fwriteFarMemImpl" "\n");
		return WriteProcessMemory(hProcess, (LPVOID)ui64pDest, pSrc, ui64Size, 0);
	};
	DbgPrint(" + " "fwriteFarMemImpl: %#p" "\n", fwriteFarMemImpl);
	//
	fexecThreadImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pParam) -> unsigned __int32
	{
		DbgPrint(" > " "fexecThreadImpl" "\n");
		auto hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)ui64pDest, (LPVOID)ui64pParam, 0, 0);
		auto uRetval = WaitForSingleObject(hThread, INFINITE);
		if (uRetval)
			DbgPrint("   ""   ""uRetval:%u""\n", uRetval);
		return uRetval;
	};
	DbgPrint(" + " "fexecThreadImpl: %#p" "\n", fexecThreadImpl);
	//
	fmallocImpl = [](unsigned __int64 ui64size) -> unsigned __int64
	{
		DbgPrint(" > " "fmallocImpl" "\n");
		return (unsigned __int64)malloc(ui64size);
	};
	DbgPrint(" + " "fmallocImpl: %#p" "\n", fmallocImpl);
	fmemcpyImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64size) -> void
	{
		DbgPrint(" > " "fmemcpyImpl" "\n");
		memcpy((void*)ui64pDest, (void*)ui64pSrc, ui64size);
	};
	DbgPrint(" + " "fmemcpyImpl: %#p" "\n", fmemcpyImpl);
	fmemfreeImpl = [](unsigned __int64 ui64pDest) -> void
	{
		DbgPrint(" > " "fmemfreeImpl" "\n");
		free((void*)ui64pDest);
	};
	DbgPrint(" + " "fmemfreeImpl: %#p" "\n", fmemfreeImpl);
	finterpret_command = [](
		unsigned __int64  CMDid,
		unsigned __int64  CMDpBuf,
		unsigned __int64  CMDsize,
		unsigned __int64& RPLpBuf,
		unsigned __int64& RPLsize
		)
	{
		DbgPrint(" > " "finterpret_command" "\n");
		if (CMDid == (eCmdID)ECHO)
		{
			RPLsize = CMDsize;
			RPLpBuf = fmallocImpl(RPLsize);
			fmemcpyImpl(RPLpBuf, CMDpBuf, RPLsize);
		}
		else if (CMDid == (eCmdID)PING)
		{
			RPLsize = 5;
			RPLpBuf = fmallocImpl(RPLsize);
			fmemcpyImpl(RPLpBuf, (unsigned __int64)"pong", RPLsize);
		}
	};
	DbgPrint(" + " "finterpret_command: %#p" "\n", finterpret_command);

	// Find Target
	PEPROCESS pEPROCESS = NULL;
	if (!NT_SUCCESS(ring3::FindProcessByName("Client.exe", &pEPROCESS)))
		return STATUS_SUCCESS;

	// Find Module
	auto pLoader = ring3::KeGetUserModule(pEPROCESS, L"Client.exe");
	if (!pLoader)
		return STATUS_SUCCESS;

	// Find Export
	ui64fgetCmdCB = (unsigned __int64)ring3::KeGetModuleExport(pLoader, "getCmdCB", pEPROCESS);
	DbgPrint(" [ ] ui64fgetCmdCB: %p\n", ui64fgetCmdCB);
	if (!ui64fgetCmdCB)
		return STATUS_SUCCESS;

	// Find Export
	ui64falcRplCB = (unsigned __int64)ring3::KeGetModuleExport(pLoader, "alcRplCB", pEPROCESS);
	DbgPrint(" [ ] ui64falcRplCB: %p\n", ui64falcRplCB);
	if (!ui64falcRplCB)
		return STATUS_SUCCESS;

	// Find Export
	ui64fsetRplCB = (unsigned __int64)ring3::KeGetModuleExport(pLoader, "setRplCB", pEPROCESS);
	DbgPrint(" [ ] ui64fsetRplCB: %p\n", ui64fsetRplCB);
	if (!ui64fsetRplCB)
		return STATUS_SUCCESS;

	//init our worker
	PVOID pParam = nullptr;
	auto pWorker = nWORK_QUEUE_ITEM::cWorker::Init_cWorker([](PVOID pParam, nWORK_QUEUE_ITEM::cWorker* pcWorker) -> void
	{
			DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
			PrintIrql();
			DbgPrint(" [ ] pParam: %p\n", pParam);
			DbgPrint(" [ ] pcWorker: %p\n", pcWorker);

			auto uResult = single_command(ui64fgetCmdCB, ui64falcRplCB, ui64fsetRplCB);
			if (uResult)
				pcWorker->finish();

			//stop our worker [this means this function is exec once]
			pcWorker->finish();

			return;
	}, pParam);

	//start out worker
	pWorker->queue();

	DbgPrint(" [ ] reached expected end!" "\n");
	return STATUS_SUCCESS;
}

VOID PrintIrql()
{
	const auto Irql = KeGetCurrentIrql();

	PWSTR sIrql = nullptr;
	switch (Irql) {
	case PASSIVE_LEVEL:
		sIrql = L"PASSIVE_LEVEL";
		break;
	case APC_LEVEL:
		sIrql = L"APC_LEVEL";
		break;
	case DISPATCH_LEVEL:
		sIrql = L"DISPATCH_LEVEL";
		break;
	case CMCI_LEVEL:
		sIrql = L"CMCI_LEVEL";
		break;
	case CLOCK_LEVEL:
		sIrql = L"CLOCK_LEVEL";
		break;
	case IPI_LEVEL:
		sIrql = L"IPI_LEVEL";
		break;
	case HIGH_LEVEL:
		sIrql = L"HIGH_LEVEL";
		break;
	default:
		sIrql = L"Unknown Value";
		break;
	}

	DbgPrint(" [+] KeGetCurrentIrql: %u(%ws)" "\n", Irql, sIrql);
}