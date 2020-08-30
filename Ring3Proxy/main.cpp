#include "inc.hpp"

HANDLE hProcess = INVALID_HANDLE_VALUE;

unsigned __int32 GetProcessByName(std::wstring name)
{
	printf(" > " __FUNCTION__ "\n");

	DWORD pid = 0;

	// Create toolhelp snapshot.
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 process;
	ZeroMemory(&process, sizeof(process));
	process.dwSize = sizeof(process);

	// Walkthrough all processes.
	if (Process32First(snapshot, &process))
	{
		do
		{
			std::wcout << L" > " << process.szExeFile << std::endl;
			if (lstrcmpW(process.szExeFile, name.c_str()) == 0)
			{
				pid = process.th32ProcessID;
				break;
			}
		} while (Process32Next(snapshot, &process));
	}

	CloseHandle(snapshot);

	return pid;
}

int main(int argc, char* argv[])
{
	printf(" > " __FUNCTION__ "\n");

	auto pid = GetProcessByName(std::wstring(L"Client.exe"));
	printf(" + " "pid: %u" "\n", pid);

	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
		if (!hProcess)
			return 1;
	}
	printf(" + " "hProcess: %#p" "\n", hProcess);

	fallocFarMemImpl = [](unsigned __int64* pui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		printf(" > " "fallocFarMemImpl" "\n");
		*pui64pDest = (unsigned __int64)VirtualAllocEx(hProcess, (LPVOID)*pui64pDest, ui64Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		return bool(!*pui64pDest);
	};
	printf(" + " "fallocFarMemImpl: %#p" "\n", fallocFarMemImpl);
	ffreeFarMemImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		printf(" > " "ffreeFarMemImpl" "\n");
		return VirtualFreeEx(hProcess, (LPVOID)ui64pDest, 0, MEM_RELEASE);
	};
	printf(" + " "ffreeFarMemImpl: %#p" "\n", ffreeFarMemImpl);
	//
	freadFarMemImpl = [](unsigned __int64 ui64pSrc, void* pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		printf(" > " "freadFarMemImpl" "\n");
		return ReadProcessMemory(hProcess, (LPCVOID)ui64pSrc, pDest, ui64Size, 0);
	};
	printf(" + " "freadFarMemImpl: %#p" "\n", freadFarMemImpl);
	fwriteFarMemImpl = [](void* pSrc, unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		printf(" > " "fwriteFarMemImpl" "\n");
		return WriteProcessMemory(hProcess, (LPVOID)ui64pDest, pSrc, ui64Size, 0);
	};
	printf(" + " "fwriteFarMemImpl: %#p" "\n", fwriteFarMemImpl);
	//
	fexecThreadImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pParam) -> unsigned __int32
	{
		printf(" > " "fexecThreadImpl" "\n");
		auto hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)ui64pDest, (LPVOID)ui64pParam, 0, 0);
		auto uRetval = WaitForSingleObject(hThread, INFINITE);
		if (uRetval)
			printf("   ""   ""uRetval:%u""\n", uRetval);
		return uRetval;
	};
	printf(" + " "fexecThreadImpl: %#p" "\n", fexecThreadImpl);
	//
	fmallocImpl = [](unsigned __int64 ui64size) -> unsigned __int64
	{
		printf(" > " "fmallocImpl" "\n");
		return (unsigned __int64)malloc(ui64size);
	};
	printf(" + " "fmallocImpl: %#p" "\n", fmallocImpl);
	fmemcpyImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64size) -> void
	{
		printf(" > " "fmemcpyImpl" "\n");
		memcpy((void*)ui64pDest, (void*)ui64pSrc, ui64size);
	};
	printf(" + " "fmemcpyImpl: %#p" "\n", fmemcpyImpl);
	fmemfreeImpl = [](unsigned __int64 ui64pDest) -> void
	{
		printf(" > " "fmemfreeImpl" "\n");
		free((void*)ui64pDest);
	};
	printf(" + " "fmemfreeImpl: %#p" "\n", fmemfreeImpl);

	unsigned __int64 ui64fgetCmdCB = (unsigned __int64)GetProcAddress(LoadLibraryA("Client.exe"), "getCmdCB");
	printf(" + " "ui64fgetCmdCB: %#p" "\n", ui64fgetCmdCB);
	unsigned __int64 ui64falcRplCB = (unsigned __int64)GetProcAddress(LoadLibraryA("Client.exe"), "alcRplCB");
	printf(" + " "ui64falcRplCB: %#p" "\n", ui64falcRplCB);
	unsigned __int64 ui64fsetRplCB = (unsigned __int64)GetProcAddress(LoadLibraryA("Client.exe"), "setRplCB");
	printf(" + " "ui64fsetRplCB: %#p" "\n", ui64fsetRplCB);

	unsigned __int32 uResult = 0ui32;
	while (!uResult)
	{
		printf("------------------------------------------------------" "\n");

		uResult = single_command(ui64fgetCmdCB, ui64falcRplCB, ui64fsetRplCB);

		system("pause");
	}
	return uResult;
}