#pragma once

#include "ts_map/ts_map.hpp"
#include "ts_queue/ts_queue.hpp"

struct sCmd {
	enum eCmdId : uint64_t
	{
		echo
	}			eCmdId;
	uint64_t    ui64Size;
	uint64_t	ui64pParam;
	uint32_t    ui32Reply;
};

extern ts_map<uint64_t> cmds_active;
extern ts_queue<sCmd> cmds_pending;

extern "C" __declspec(dllexport) uint32_t __cdecl getCmdCB(uint64_t ui64pCmd);
extern "C" __declspec(dllexport) uint32_t __cdecl setPromiseCB(uint64_t ui64pCmd);

std::future<uint32_t> queue_cmd(const sCmd Cmd);