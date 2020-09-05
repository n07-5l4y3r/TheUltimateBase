#include "inc.hpp"

void* __cdecl operator new(size_t size, POOL_TYPE pool, ULONG tag) {
	DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
	return ExAllocatePoolWithTag(pool, size, tag);
}
