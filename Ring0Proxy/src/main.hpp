#pragma once

#include <inc.hpp>

#ifdef USE_MMAP_LOADER
extern "C" NTSTATUS DriverEntry(
	_In_ PDRIVER_OBJECT  driver_object,
	_In_ PUNICODE_STRING registry_path
);
#endif

VOID PrintIrql();