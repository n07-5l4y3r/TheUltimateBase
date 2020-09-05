#pragma once

#include "pre.hpp"

#include <ntifs.h>
#include <wdf.h>
#pragma comment(lib, "Ntoskrnl.lib")

#include "exceptionhandler.hpp"

void* __cdecl operator new(size_t size, POOL_TYPE pool, ULONG tag = 0);

#include "WORK_QUEUE_ITEM/WORK_QUEUE_ITEM.hpp"
#include "RING_3/wapi.hpp"
#include "RING_3/ring3.hpp"