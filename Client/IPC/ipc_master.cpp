#include "ipc_master.hpp"

#include <cstdio>

unsigned __int32 (*fallocFarMemImpl)(unsigned __int64* pui64pDest, unsigned __int64 ui64Size) = 0;
unsigned __int32 (*ffreeFarMemImpl)(unsigned __int64 ui64pDest, unsigned __int64 ui64Size) = 0;
//
unsigned __int32 (*freadFarMemImpl)(unsigned __int64 ui64pSrc, void* pDest, unsigned __int64 ui64Size) = 0;
unsigned __int32 (*fwriteFarMemImpl)(void* pSrc, unsigned __int64 ui64pDest, unsigned __int64 ui64Size) = 0;
//
unsigned __int32 (*fexecThreadImpl)(unsigned __int64 ui64pDest, unsigned __int64 ui64pParam) = 0;
//
unsigned __int64 (*fmallocImpl)(unsigned __int64 ui64size) = 0;
void (*fmemcpyImpl)(unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64size) = 0;
void (*fmemfreeImpl)(unsigned __int64 ui64pDest) = 0;
//
void (*finterpret_command)(
    unsigned __int64  CMDid,
    unsigned __int64  CMDpBuf,
    unsigned __int64  CMDsize,
    unsigned __int64& RPLpBuf,
    unsigned __int64& RPLsize) = 0;
//
unsigned __int32 single_command(
    unsigned __int64 ui64fgetCmdCB,
    unsigned __int64 ui64falcRplCB,
    unsigned __int64 ui64fsetRplCB
)
{
    printf(" > " __FUNCTION__ "\n");

    // [1] far_ui64pui64pParam = malloc
    unsigned __int64 far_ui64pui64pParam = 0ui64;
    fallocFarMemImpl(&far_ui64pui64pParam, sizeof(unsigned __int64));
    printf(" + " "far_ui64pui64pParam: %#p" "\n", far_ui64pui64pParam);

    // * far_ui64pui64pParam = (ui64) pParam
    fexecThreadImpl(ui64fgetCmdCB, far_ui64pui64pParam);

    // far_ui64pParam = * far_ui64pui64pParam
    unsigned __int64 far_ui64pParam = 0ui64;
    freadFarMemImpl(far_ui64pui64pParam, &far_ui64pParam, sizeof(unsigned __int64));
    printf(" + " "far_ui64pParam: %#p" "\n", far_ui64pParam);

    // [1] free far_ui64pui64pParam
    ffreeFarMemImpl(far_ui64pui64pParam, sizeof(unsigned __int64));

    if (!far_ui64pParam)
        return 0ui32;

    // [2] pParam = malloc
    sParam* pParam = (sParam*)fmallocImpl(sizeof(sParam));
    printf(" + " "pParam: %#p" "\n", pParam);

    // pParam < far_ui64pParam
    freadFarMemImpl(far_ui64pParam, pParam, sizeof(sParam));

    // [3] ui64CMDpBuf = malloc
    unsigned __int64 ui64CMDpBuf = fmallocImpl(pParam->ui64CMDsize);
    printf(" + " "ui64CMDsize: %u" "\n", pParam->ui64CMDsize);

    //ui64CMDpBuf <- pParam->ui64CMDpBuf
    freadFarMemImpl(pParam->ui64CMDpBuf, (void*)ui64CMDpBuf, pParam->ui64CMDsize);
    printf(" + " "ui64CMDpBuf: %#p '%s'" "\n", ui64CMDpBuf, ui64CMDpBuf);

    // ui64RPLsize = reply size
    // [4] ui64RPLpBuf = malloc < reply buf
    unsigned __int64 ui64RPLsize = 0ui64;
    unsigned __int64 ui64RPLpBuf = 0;
    finterpret_command(pParam->ui64CMDid, ui64CMDpBuf, pParam->ui64CMDsize, ui64RPLpBuf, ui64RPLsize);
    printf(" + " "ui64RPLsize: %u" "\n", ui64RPLsize);
    printf(" + " "ui64RPLpBuf: %#p '%s'" "\n", ui64RPLpBuf, ui64RPLpBuf);

    // [3] free ui64CMDpBuf
    fmemfreeImpl(ui64CMDpBuf);

    // pParam->ui64RPLsize = ui64RPLsize
    pParam->ui64RPLsize = ui64RPLsize;

    // * far_ui64pParam = oParam
    fwriteFarMemImpl(pParam, far_ui64pParam, sizeof(sParam));

    // allocate far response buffer
    fexecThreadImpl(ui64falcRplCB, far_ui64pParam);

    // pParam < far_ui64pParam
    freadFarMemImpl(far_ui64pParam, pParam, sizeof(sParam));

    // pParam->ui64RPLpBuf < ui64RPLpBuf
    fwriteFarMemImpl((void*)ui64RPLpBuf, pParam->ui64RPLpBuf, pParam->ui64RPLsize);

    // [4] free ui64RPLpBuf
    fmemfreeImpl(ui64RPLpBuf);

    // set far promise
    fexecThreadImpl(ui64fsetRplCB, far_ui64pParam);

    // [2] free pParam
    fmemfreeImpl((unsigned __int64)pParam);

    return 0;
}