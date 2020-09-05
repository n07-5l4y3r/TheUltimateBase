#include "main.hpp"

NTSTATUS quit(PSTR sReason, NTSTATUS reason)
{
	DbgPrint("%s" "\n", sReason);
	return reason;
}

NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  driver_object,
	_In_ PUNICODE_STRING registry_path
)
{
	UNREFERENCED_PARAMETER(driver_object);
	UNREFERENCED_PARAMETER(registry_path);

	DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
	PrintIrql();

	//// Find Target
	//PEPROCESS pEPROCESS = NULL;
	//if (!NT_SUCCESS(ring3::FindProcessByName("Client.exe", &pEPROCESS)))
	//	return STATUS_SUCCESS;

	//// Find Module
	//auto pLoader = ring3::KeGetUserModule(pEPROCESS, L"Client.exe");
	//if (!pLoader)
	//	return STATUS_SUCCESS;

	//// Find Export
	//auto pupParam = ring3::KeGetModuleExport(pLoader, "upParam", pEPROCESS);
	//if (!pupParam)
	//	return STATUS_SUCCESS;

	//// Find Export
	//auto pring3_callgate = ring3::KeGetModuleExport(pLoader, "ring3_callgate", pEPROCESS);
	//if (!pring3_callgate)
	//	return STATUS_SUCCESS;

	//init our worker
	PVOID pParam = nullptr;
	auto pWorker = nWORK_QUEUE_ITEM::cWorker::Init_cWorker([](PVOID pParam, nWORK_QUEUE_ITEM::cWorker* pcWorker) -> void
	{
			DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
			PrintIrql();
			DbgPrint(" [ ] pParam: %p\n", pParam);
			DbgPrint(" [ ] pcWorker: %p\n", pcWorker);

			//stop our worker
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