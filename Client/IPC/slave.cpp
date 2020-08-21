#include "slave.hpp"

unsigned __int32 QuerryCB(unsigned __int64 ui64pPacket)
{
	auto pPacket = (psPacket)ui64pPacket;

	//pPacket->ui64pAfter = queue begin
	return 0;
}

unsigned __int32 PromiseCB(unsigned __int64 ui64pPacket)
{
	//free ui64pPacket
	return 0;
}

cSlave::cSlave() {}

cSlave* cSlave::Init()
{
	return cSlave::pInst ? cSlave::pInst : cSlave::pInst = new cSlave();
}
