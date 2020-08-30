#pragma once

#include "ipc.h"

extern unsigned __int32 (*fallocFarMemImpl)(unsigned __int64* pui64pDest, unsigned __int64 ui64Size);
extern unsigned __int32 (*ffreeFarMemImpl)(unsigned __int64 ui64pDest, unsigned __int64 ui64Size);
//
extern unsigned __int32 (*freadFarMemImpl)(unsigned __int64 ui64pSrc, void* pDest, unsigned __int64 ui64Size);
extern unsigned __int32 (*fwriteFarMemImpl)(void* pSrc, unsigned __int64 ui64pDest, unsigned __int64 ui64Size);
//
extern unsigned __int32 (*fexecThreadImpl)(unsigned __int64 ui64pDest, unsigned __int64 ui64pParam);
//
extern unsigned __int64 (*fmallocImpl)(unsigned __int64 ui64size);
extern void (*fmemcpyImpl)(unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64size);
extern void (*fmemfreeImpl)(unsigned __int64 ui64pDest);
//
unsigned __int32 single_command
(
    unsigned __int64 ui64fgetCmdCB,
    unsigned __int64 ui64falcRplCB,
    unsigned __int64 ui64fsetRplCB
);