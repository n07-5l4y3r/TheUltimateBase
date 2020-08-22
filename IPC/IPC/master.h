#pragma once

typedef struct _sPacket
{
	//psPacket       pBefore
	unsigned __int64 ui64pBefore;
	//uint64_t       nSize
	unsigned __int64 ui64Size;
	//unsigned char* pData
	unsigned __int64 ui64pData;
	//psPacket       pAfter
	unsigned __int64 ui64pAfter;
} sPacket, * psPacket;
//
unsigned __int32 (*read_memory_impl)(unsigned __int32 ui32PID, unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64Size);
unsigned __int32 (*write_memory_impl)(unsigned __int32 ui32PID, unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64Size);
unsigned __int32 (*alloc_memory_impl)(unsigned __int32 ui32PID, unsigned __int64 ui64ppDest, unsigned __int64 ui64Size);
unsigned __int32 (*free_memory_impl)(unsigned __int32 ui32PID, unsigned __int64 ui64pDest, unsigned __int64 ui64Size);
unsigned __int32 (*create_thread_impl)(unsigned __int32 ui32PID, unsigned __int32 ui64pFunc, unsigned __int64 ui64pParam);
//
unsigned __int32 HandleCmd
(
	unsigned __int32 ui32PID,
	unsigned __int64 ui64Size,
	unsigned __int64 ui64pData,
	unsigned __int64 ui64pReplySize,
	unsigned __int64 ui64ppReply
);
//
unsigned __int32 handle_packet
(
	unsigned __int32 ui32PID,
	psPacket pPacket,
	unsigned __int64 ui64pPacket,
	unsigned __int64 ui64fPromiseCB
);
unsigned __int32 querry
(
	unsigned __int32 ui32PID,
	unsigned __int64 ui64fQuerryCB,
	unsigned __int64 ui64fPromiseCB
);