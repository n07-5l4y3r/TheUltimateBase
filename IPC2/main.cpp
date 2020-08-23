#include "inc.hpp"

#include <future>

#include <Windows.h>

struct sCmd {
	enum eCmdId : uint64_t
	{
		echo
	}			eCmdId;
	uint64_t    ui64Size;
	uint64_t	ui64pParam;
	uint32_t    ui32Reply;
};

ts_map<uint64_t> cmds_active;
ts_queue<sCmd> cmds_pending;

extern "C" __declspec(dllexport) uint32_t __cdecl getCmdCB(uint64_t ui64pCmd)
{
	printf("   " "   " " > " __FUNCTION__ "\n");
	if (cmds_pending.is_empty())
		return 0ui32;
	*(sCmd*)ui64pCmd = cmds_pending.pop();
	return 0ui32;
}
extern "C" __declspec(dllexport) uint32_t __cdecl setPromiseCB(uint64_t ui64pCmd)
{
	printf("   " "   " " > " __FUNCTION__ "\n");
	auto pCmd = (sCmd*)ui64pCmd;
	if (!cmds_active.get(pCmd->ui64pParam).has_value())
		return 0ui32;
	cmds_active.get(pCmd->ui64pParam).value().get().set_value(pCmd->ui32Reply);
	return 0ui32;
}

std::future<uint32_t> queue_cmd(const sCmd Cmd)
{
	printf("   " " > " __FUNCTION__ "\n");
	cmds_pending.push(Cmd);
	cmds_active.add(Cmd.ui64pParam);
	return cmds_active.get(Cmd.ui64pParam).value().get().get_future();
}

int slave()
{
	printf(" > " __FUNCTION__ "\n");
	while (true)
	{
		auto f1 = queue_cmd(sCmd({ sCmd::eCmdId::echo,6ui64,(uint64_t)"hello",0ui64 }));
		auto f2 = queue_cmd(sCmd({ sCmd::eCmdId::echo,6ui64,(uint64_t)"world",0ui64 }));
		auto f3 = queue_cmd(sCmd({ sCmd::eCmdId::echo,5ui64,(uint64_t)"test",0ui64 }));
		auto f4 = queue_cmd(sCmd({ sCmd::eCmdId::echo,4ui64,(uint64_t)"123",0ui64 }));
		printf(" > " "f1:%u", f1.get());
		printf(" > " "f2:%u", f2.get());
		printf(" > " "f3:%u", f3.get());
		printf(" > " "f4:%u", f4.get());
		system("pause");
	}
	return 0;
}

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
			auto farGetCmdCB = GetProcAddress(GetModuleHandle(NULL), "getCmdCB");
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
		if (oCmd.eCmdId == sCmd::echo && oCmd.ui64pParam && oCmd.ui64Size)
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
			auto farPromiseCB = GetProcAddress(GetModuleHandle(NULL), "setPromiseCB");
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

int main(int argc, char* argv[])
{
	printf(" > " __FUNCTION__ "\n");
	printf(" > ""PID: %u""\n", GetCurrentProcessId());
	if (argc > 1)
		return master(atoi(argv[1]));
	else
		return slave();
	return 0;
}