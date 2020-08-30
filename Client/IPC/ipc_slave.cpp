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
	printf(" > " __FUNCTION__ "\n");

	auto pParam = (sParam*)malloc(sizeof(sParam));
	printf(" + " "pParam: %#p" "\n", pParam);

	pParam->ui64CMDid = (unsigned __int64) CmdId;
	printf(" + " "pParam->ui64CMDid: %u" "\n", pParam->ui64CMDid);

	pParam->ui64CMDsize = ui64CMDsize;
	printf(" + " "pParam->ui64CMDsize: %u" "\n", pParam->ui64CMDsize);

	pParam->ui64CMDpBuf = (unsigned __int64)malloc(pParam->ui64CMDsize);
	memcpy((void*)pParam->ui64CMDpBuf, CMDpBuf, pParam->ui64CMDsize);
	printf(" + " "pParam->ui64CMDpBuf: %#p '%s'" "\n", pParam->ui64CMDpBuf, pParam->ui64CMDpBuf);

	pParam->ui64RPLpBuf = 0ui64;
	printf(" + " "pParam->ui64RPLpBuf: %#p" "\n", pParam->ui64RPLpBuf);

	pParam->ui64RPLsize = 0ui64;
	printf(" + " "pParam->ui64RPLsize: %u" "\n", pParam->ui64RPLsize);

	ipc_queue.push(pParam);
	ipc_map.add(pParam);

	return pParam;
}

// get future (from map)
std::future<sParam> getFuture(sParam* pParam)
{
	printf(" > " __FUNCTION__ "\n");

	return ipc_map.get(pParam).value().get().get_future();
}

// remove command (from map)
// free CMDpBuf
// free RPLpBuf
// free pParam
void freeCmd(sParam* pParam)
{
	printf(" > " __FUNCTION__ "\n");

	ipc_map.remove(pParam);
	free((void*)pParam->ui64CMDpBuf);
	free((void*)pParam->ui64RPLpBuf);
	free(pParam);
}

// get & remove command (from queue)
unsigned __int32 __cdecl getCmdCB(unsigned __int64 ui64pui64pParam)
{
	printf(" > " __FUNCTION__ "\n");

	auto pParam = ipc_queue.pop();
	auto ui64pParam = (unsigned __int64)pParam;
	auto pui64pParam = (unsigned __int64*)ui64pui64pParam;
	return memcpy_s(pui64pParam, sizeof(unsigned __int64), &ui64pParam, sizeof(unsigned __int64));
}

// get promise (from map)
// fullfill promise
unsigned __int32 __cdecl setRplCB(unsigned __int64 ui64pParam)
{
	printf(" > " __FUNCTION__ "\n");

	auto pParam = (sParam*)ui64pParam;
	if (!ipc_map.get(pParam).has_value())
		return 1;
	ipc_map.get(pParam).value().get().set_value(*pParam);
	return 0;
}

// alloc RPLpBuf
unsigned __int32 __cdecl alcRplCB(unsigned __int64 ui64pParam)
{
	printf(" > " __FUNCTION__ "\n");

	auto pParam = (sParam*)ui64pParam;
	pParam->ui64RPLpBuf = (unsigned __int64)malloc(pParam->ui64RPLsize);
	return bool(!pParam->ui64RPLpBuf);
}
