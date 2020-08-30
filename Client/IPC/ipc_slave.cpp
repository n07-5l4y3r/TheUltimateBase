#include "ipc_slave.hpp"

// map of commands and thier promise
ts_map<sParam*, std::promise<sParam>> ipc_map;
// queue of commands still pending
ts_queue<sParam*> ipc_queue;

// add command to map and queue
// alloc sParam
// alloc CMDpBuf
sParam* queueCmd(eCmdID CmdId, unsigned __int8* CMDpBuf, unsigned __int64 ui64CMDsize)
{
	auto pParam = (sParam*)malloc(sizeof(sParam));

	pParam->ui64CMDid = (unsigned __int64) CmdId;

	pParam->ui64CMDsize = ui64CMDsize;
	pParam->ui64CMDpBuf = (unsigned __int64)malloc(pParam->ui64CMDpBuf);
	memcpy((void*)pParam->ui64CMDpBuf, CMDpBuf, pParam->ui64CMDsize);

	pParam->ui64RPLpBuf = 0ui64;
	pParam->ui64RPLsize = 0ui64;

	ipc_queue.push(pParam);
	ipc_map.add(pParam);

	return pParam;
}

// get future (from map)
std::future<sParam> getFuture(sParam* pParam)
{
	return ipc_map.get(pParam).value().get().get_future();
}

// remove command (from map)
// free CMDpBuf
// free RPLpBuf
// free pParam
void freeCmd(sParam* pParam)
{
	ipc_map.remove(pParam);
	free((void*)pParam->ui64CMDpBuf);
	free((void*)pParam->ui64RPLpBuf);
	free(pParam);
}

// get & remove command (from queue)
unsigned __int32 __cdecl getCmdCB(unsigned __int64 ui64pui64pParam)
{
	auto pParam = ipc_queue.pop();
	auto ui64pParam = (unsigned __int64)pParam;
	auto pui64pParam = (unsigned __int64*)ui64pui64pParam;
	return memcpy_s(pui64pParam, sizeof(unsigned __int64), &ui64pParam, sizeof(unsigned __int64));
}

// get promise (from map)
// fullfill promise
unsigned __int32 __cdecl setRplCB(unsigned __int64 ui64pParam)
{
	auto pParam = (sParam*)ui64pParam;
	if (!ipc_map.get(pParam).has_value())
		return 1;
	ipc_map.get(pParam).value().get().set_value(*pParam);
	return 0;
}

// alloc RPLpBuf
unsigned __int32 __cdecl alcRplCB(unsigned __int64 ui64pParam)
{
	auto pParam = (sParam*)ui64pParam;
	pParam->ui64RPLpBuf = (unsigned __int64)malloc(pParam->ui64RPLsize);
	return bool(!pParam->ui64RPLpBuf);
}
