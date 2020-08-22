#include "slave.hpp"

unsigned __int32 cSlave::QuerryCB(unsigned __int64 ui64pPacket)
{
	auto pPacket = (psPacket)ui64pPacket;
	
	pPacket->ui64pAfter = cSlave::Init()->ui64pQueueBegin;
	cSlave::Init()->ui64pQueueBegin = cSlave::Init()->ui64pQueueEnd;

	return 0;
}

unsigned __int32 cSlave::PromiseCB(unsigned __int64 ui64pPacket)
{
	cSlave::Init()->mPacketQueue.at(ui64pPacket).set_value(ui64pPacket);
	cSlave::Init()->mPacketQueue.erase(ui64pPacket);
	return 0;
}

cSlave* cSlave::pInst = nullptr;

cSlave::cSlave() {}

std::future<unsigned __int64> cSlave::queue_cmd(psPacket pPacket)
{
	this->ui64pQueueEnd = ((psPacket)this->ui64pQueueEnd)->ui64pAfter = (unsigned __int64)pPacket;
	this->mPacketQueue.insert(std::make_pair(this->ui64pQueueEnd, std::promise<unsigned __int64>()));
	return this->mPacketQueue.at(this->ui64pQueueEnd).get_future();
}

cSlave* cSlave::Init()
{
	return cSlave::pInst ? cSlave::pInst : cSlave::pInst = new cSlave();
}

extern "C" __declspec(dllexport) unsigned __int32 (*QuerryCB)(unsigned __int64 ui64pPacket) = cSlave::QuerryCB;
extern "C" __declspec(dllexport) unsigned __int32 (*PromiseCB)(unsigned __int64 ui64pPacket) = cSlave::PromiseCB;