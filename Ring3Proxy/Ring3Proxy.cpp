#include <iostream>
#include <Windows.h>
#include <tlhelp32.h>

struct sCmd {
	enum eCmdId : uint64_t
	{
		echo
	}			eCmdId;
	uint64_t    ui64Size;
	uint64_t	ui64pParam;
	uint32_t    ui32Reply;
};

int master(uint32_t ui32PID)
{
	printf(" > " __FUNCTION__ "\n");

	auto hProcess = INVALID_HANDLE_VALUE;
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ui32PID);
		if (!hProcess)
			return 1;
	}

	while (true)
	{
		printf("   "" > ""alloc cmd""\n");
		uint64_t ui64pCmd = 0ui64;
		{
			ui64pCmd = (uint64_t)VirtualAllocEx(hProcess, 0, sizeof(sCmd), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (!ui64pCmd)
			{
				printf("   ""   ""ui64pCmd:%u""\n", ui64pCmd);
				return 2;
			}
		}
		printf("   "" > ""get cmd""\n");
		uint32_t uRetval = 0ui32;
		{
			auto farGetCmdCB = GetProcAddress(LoadLibraryA("Client.exe"), "getCmdCB");
			auto hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)farGetCmdCB, (LPVOID)ui64pCmd, 0, 0);
			uRetval = WaitForSingleObject(hThread, INFINITE);
			if (uRetval)
			{
				printf("   ""   ""uRetval:%u""\n", uRetval);
				return uRetval;
			}
		}
		printf("   "" > ""read cmd""\n");
		sCmd oCmd = { };
		{
			if (!ReadProcessMemory(hProcess, (LPCVOID)ui64pCmd, &oCmd, sizeof(sCmd), 0))
				return 3;
		}
		printf("   "" > ""read param""\n");
		LPVOID pBuf = nullptr;
		if (oCmd.ui64pParam && oCmd.ui64Size)
		{
			if (!ReadProcessMemory(hProcess, (LPCVOID)oCmd.ui64pParam, pBuf = malloc(oCmd.ui64Size), oCmd.ui64Size, 0))
				return 4;
		}
		if (pBuf)
		{
			printf(" > " "pBuf: [%#p] '%s'" "\n", pBuf, pBuf);
			free(pBuf);
		}
		printf("   "" > ""write reply""\n");
		{
			oCmd.ui32Reply = 0ui32;
			if (!WriteProcessMemory(hProcess, (LPVOID)ui64pCmd, &oCmd, sizeof(sCmd), 0))
				return 5;
		}
		printf("   "" > ""set promise""\n");
		{
			auto farPromiseCB = GetProcAddress(LoadLibraryA("Client.exe"), "setPromiseCB");
			auto hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)farPromiseCB, (LPVOID)ui64pCmd, 0, 0);
			uRetval = WaitForSingleObject(hThread, INFINITE);
			if (uRetval)
			{
				printf("   ""   ""uRetval:%u""\n", uRetval);
				return uRetval;
			}
		}
		printf("   "" > ""free cmd""\n");
		{
			VirtualFreeEx(hProcess, (LPVOID)ui64pCmd, 0, MEM_RELEASE);
		}
		system("pause");
	}

	CloseHandle(hProcess);

	return 0;
}

uint32_t GetProcessByName(std::wstring name)
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
	if (!pid)
	{
		std::cout << "not found!" << std::endl;
		return 1;
	}

	return master(pid);
}