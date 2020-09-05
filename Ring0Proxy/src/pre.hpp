#pragma once

#define USE_MMAP_LOADER

#ifdef _DEBUG
#define DEBUGPRINT
#endif

#ifdef _KERNEL_MODE
#define NONPAGESECTION __declspec(code_seg("$kerneltext$"))
#else
#define NONPAGESECTION
#endif

#define TRY   __try

#define CATCH __except(fGlobalExceptionHandler(GetExceptionCode(), GetExceptionInformation()))