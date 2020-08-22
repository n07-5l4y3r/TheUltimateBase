#pragma once

#include "master.h"

#include <map>
#include <future>

class cSlave {
	static cSlave* pInst;
	//
	std::map<unsigned __int64 /*ui64pPacket*/, std::promise<unsigned __int64 /*ui64pPacketResponse*/>> mPacketQueue;
	//
	unsigned __int64 ui64pQueueBegin = 0ui64, ui64pQueueEnd = 0ui64;
private:
	cSlave();
public:
	std::future<unsigned __int64> queue_cmd(psPacket pPacket);
public:
	static cSlave* Init();
public:
	static unsigned __int32 QuerryCB(unsigned __int64 ui64pPacket);
	static unsigned __int32 PromiseCB(unsigned __int64 ui64pPacket);
};