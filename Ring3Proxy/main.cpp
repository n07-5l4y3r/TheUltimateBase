#include "inc.hpp"

HANDLE hProcess = INVALID_HANDLE_VALUE;

unsigned __int32 GetProcessByName(std::wstring name)
{
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
	auto pid = GetProcessByName(std::wstring(L"Client.exe"));

	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);
		if (!hProcess)
			return 1;
	}

	fallocFarMemImpl = [](unsigned __int64* pui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		*pui64pDest = (unsigned __int64)VirtualAllocEx(hProcess, (LPVOID)*pui64pDest, ui64Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		return bool(!*pui64pDest);
	};
	ffreeFarMemImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		return VirtualFreeEx(hProcess, (LPVOID)ui64pDest, 0, MEM_RELEASE);
	};
	//
	freadFarMemImpl = [](unsigned __int64 ui64pSrc, void* pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		return ReadProcessMemory(hProcess, (LPCVOID)ui64pSrc, pDest, ui64Size, 0);
	};
	fwriteFarMemImpl = [](void* pSrc, unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{
		return WriteProcessMemory(hProcess, (LPVOID)ui64pDest, pSrc, ui64Size, 0);
	};
	//
	fexecThreadImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pParam) -> unsigned __int32
	{
		auto hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)ui64pDest, (LPVOID)ui64pParam, 0, 0);
		auto uRetval = WaitForSingleObject(hThread, INFINITE);
		if (uRetval)
			printf("   ""   ""uRetval:%u""\n", uRetval);
		return uRetval;
	};
	//
	fmallocImpl = [](unsigned __int64 ui64size) -> unsigned __int64
	{
		return (unsigned __int64)malloc(ui64size);
	};
	fmemcpyImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64size) -> void
	{
		memcpy((void*)ui64pDest, (void*)ui64pSrc, ui64size);
	};
	fmemfreeImpl = [](unsigned __int64 ui64pDest) -> void
	{
		free((void*)ui64pDest);
	};

	unsigned __int64 ui64fgetCmdCB = (unsigned __int64)GetProcAddress(LoadLibraryA("Client.exe"), "getCmdCB");
	unsigned __int64 ui64falcRplCB = (unsigned __int64)GetProcAddress(LoadLibraryA("Client.exe"), "alcRplCB");
	unsigned __int64 ui64fsetRplCB = (unsigned __int64)GetProcAddress(LoadLibraryA("Client.exe"), "setRplCB");

	unsigned __int32 uResult = 0ui32;
	while (!uResult)
	{
		uResult = single_command(ui64fgetCmdCB, ui64falcRplCB, ui64fsetRplCB);
	}
	return uResult;
}