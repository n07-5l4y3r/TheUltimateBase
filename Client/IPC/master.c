#include "master.h"

unsigned __int32 handle_packet(unsigned __int32 ui32PID, psPacket pPacket, unsigned __int64 ui64pPacket, unsigned __int64 ui64fPromiseCB)
{
	unsigned __int32 uResult = 0ui32;
	// rebase packet
	pPacket->ui64pBefore = 0ui64;
	pPacket->ui64pAfter = 0ui64;
	// free cmd data
	if (uResult = free_memory_impl(ui32PID, pPacket->ui64pData, pPacket->ui64Size))
	{
		return uResult;
	}
	// handle cmd
	unsigned __int64 ui64pReply = 0ui64, ui64SizeReply = 0ui64;
	{

	}
	// rebase data
	pPacket->ui64pData = ui64pReply;
	pPacket->ui64Size = ui64SizeReply;
	// writeback reply
	if (uResult = write_memory_impl(ui32PID, ui64pPacket, pPacket, sizeof(sPacket)))
	{
		return uResult;
	}
	// set promise
	if (uResult = create_thread_impl(ui32PID, ui64fPromiseCB, ui64pPacket))
	{
		return uResult;
	}
	// return last error (hopefully none)
	return uResult;
}

unsigned __int32 querry(unsigned __int32 ui32PID, unsigned __int64 ui64fQuerryCB, unsigned __int64 ui64fPromiseCB)
{
	unsigned __int32 uResult = 0ui32;
	// allocate header packet
	unsigned __int64 ui64pDest = 0ui64;
	if (uResult = alloc_memory_impl(ui32PID, (unsigned __int64)&ui64pDest, sizeof(sPacket)))
	{
		return uResult;
	}
	// get queue
	if (uResult = create_thread_impl(ui32PID, ui64fQuerryCB, ui64pDest))
	{
		free_memory_impl(ui32PID, ui64pDest, sizeof(sPacket));
		return uResult;
	}
	// readback header packet
	sPacket Packet = { 0 };
	if (uResult = read_memory_impl(ui32PID, (unsigned __int64)&Packet, ui64pDest, sizeof(sPacket)))
	{
		free_memory_impl(ui32PID, ui64pDest, sizeof(sPacket));
		return uResult;
	}
	// free header packet
	if (uResult = free_memory_impl(ui32PID, ui64pDest, sizeof(sPacket)))
	{
		return uResult;
	}
	// is queue empty
	if (!Packet.ui64pAfter)
	{
		return uResult;
	}
	// handle packet queue
	do
	{
		unsigned __int64 ui64pPacket = Packet.ui64pAfter;
		// readback packet
		if (uResult = read_memory_impl(ui32PID, (unsigned __int64)&Packet, ui64pPacket, sizeof(sPacket)))
		{
			return uResult;
		}
		// handle packet
		if (uResult = handle_packet(ui32PID, &Packet, ui64pPacket, ui64fPromiseCB))
		{
			return uResult;
		}
	} while (Packet.ui64pAfter);
	// return last error (hopefully none)
	return uResult;
}