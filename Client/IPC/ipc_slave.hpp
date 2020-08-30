#pragma once

#include "ipc.h"

#include "IPC/ts_map/ts_map.hpp"
#include "IPC/ts_queue/ts_queue.hpp"

#include <future>

// --

extern "C" __declspec(dllexport) unsigned __int32 __cdecl getCmdCB(unsigned __int64 ui64pui64pParam);

extern "C" __declspec(dllexport) unsigned __int32  __cdecl alcRplCB(unsigned __int64 ui64pParam);

extern "C" __declspec(dllexport) unsigned __int32  __cdecl setRplCB(unsigned __int64 ui64pParam);

// --

sParam* queueCmd(eCmdID CmdId, unsigned __int8* CMDpBuf, unsigned __int64 ui64CMDsize);

std::future<sParam> getFuture(sParam* pParam);

void freeCmd(sParam* pParam);