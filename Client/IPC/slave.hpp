#pragma once

#include "master.h"

#include <map>
#include <future>

extern "C" unsigned __int32 QuerryCB(unsigned __int64 ui64pPacket);
extern "C" unsigned __int32 PromiseCB(unsigned __int64 ui64pPacket);

class cSlave {
	static cSlave* pInst;
	static std::map<unsigned __int64 /*ui64pPacket*/, std::promise<unsigned __int64 /*ui64pPacketResponse*/>> mPacketQueue;
private:
	cSlave();
public:
	static cSlave* Init();
};