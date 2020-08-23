#include "IPC/master.h"
#include "IPC/slave.hpp"

#include <iostream>
#include <string>

#include <Windows.h>

unsigned __int32 my_read_memory_impl(unsigned __int32 ui32PID, unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64Size)
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ui32PID);
	if (!hProcess)
		return 1;
	auto retval = !ReadProcessMemory(hProcess, (LPCVOID)ui64pSrc, (LPVOID)ui64pDest, ui64Size, 0);
	CloseHandle(hProcess);
	return retval;
}
unsigned __int32 my_write_memory_impl(unsigned __int32 ui32PID, unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64Size)
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ui32PID);
	if (!hProcess)
		return 1;
	auto retval = !WriteProcessMemory(hProcess, (LPVOID)ui64pDest, (LPCVOID)ui64pSrc, ui64Size, 0);
	CloseHandle(hProcess);
	return retval;
}
unsigned __int32 my_alloc_memory_impl(unsigned __int32 ui32PID, unsigned __int64 ui64ppDest, unsigned __int64 ui64Size)
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ui32PID);
	if (!hProcess)
		return 1;
	if ((*(LPVOID*)ui64ppDest = VirtualAllocEx(hProcess, NULL, ui64Size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE)) == 0)
	{
		CloseHandle(hProcess);
		return 2;
	}
	CloseHandle(hProcess);
	return 0;
}
unsigned __int32 my_free_memory_impl(unsigned __int32 ui32PID, unsigned __int64 ui64pDest, unsigned __int64 ui64Size)
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ui32PID);
	if (!hProcess)
		return 1;
	auto retval = !VirtualFreeEx(hProcess, (LPVOID)ui64pDest, ui64Size, MEM_FREE);
	CloseHandle(hProcess);
	return retval;
}
unsigned __int32 my_create_thread_impl(unsigned __int32 ui32PID, unsigned __int32 ui64pFunc, unsigned __int64 ui64pParam)
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ui32PID);
	if (!hProcess)
		return 1;
	auto hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)(LPVOID)ui64pFunc, (LPVOID)ui64pParam, 0, 0);
	if (!hThread)
	{
		CloseHandle(hProcess);
		return 2;
	}
	auto uResult = WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hProcess);
	return uResult;
}
//
unsigned __int32 HandleCmd(unsigned __int32 ui32PID, unsigned __int64 ui64Size, unsigned __int64 ui64pData, unsigned __int64 ui64pReplySize, unsigned __int64 ui64ppReply)
{
	/*
	echo
	*/
	auto uResult = 0ui32;
	// read command data
	auto pBuffer = malloc(ui64Size);
	if (uResult = read_memory_impl(ui32PID, (unsigned __int64)pBuffer, ui64pData, ui64Size))
	{
		return uResult;
	}
	// create reply
	auto uReplySize = 0ui64;
	void* pReplyBuffer = nullptr;
	{
		uReplySize = ui64Size;
		pReplyBuffer = pBuffer;
	}
	// create far reply buffer
	auto pReplySize = (unsigned __int64*)(void*)ui64pReplySize;
	*pReplySize = uReplySize;
	if (uResult = alloc_memory_impl(ui32PID, ui64ppReply, *pReplySize))
	{
		return uResult;
	}
	// writeback far reply
	if (uResult = write_memory_impl(ui32PID, *(unsigned __int64*)ui64ppReply, (unsigned __int64)pReplyBuffer, uReplySize))
	{
		return uResult;
	}
	// 
	free(pBuffer);
	//
	return uResult;
}
int master()
{
	unsigned __int32 ui32PID;
	FARPROC fQuerryCB, fPromiseCB;

	read_memory_impl = my_read_memory_impl;
	write_memory_impl = my_write_memory_impl;
	alloc_memory_impl = my_alloc_memory_impl;
	free_memory_impl = my_free_memory_impl;
	create_thread_impl = my_create_thread_impl;

	while (true)
	{
		if (auto uResult = querry(ui32PID, (unsigned __int64)fQuerryCB, (unsigned __int64)fPromiseCB))
			return uResult;
	}

	return 0;
}

int slave()
{
	auto pSlave = cSlave::Init();
	while (true)
	{
		system("pause");
		std::string cmd;
		std::cout << "CMD > "; std::getline(std::cin, cmd);
		auto pPacket = new sPacket();
		pPacket->ui64Size = cmd.size();
		pPacket->ui64pData = (unsigned __int64)malloc(pPacket->ui64Size);
		memcpy((void*)pPacket->ui64pData, &cmd[0], pPacket->ui64Size);
		auto future = pSlave->queue_cmd(pPacket);
		auto uValue = future.get();
		auto pReply = (psPacket)uValue;
		delete pReply;
	}
}

int main(int argc, char* argv[])
{
	
}