#include "inc.hpp"

int main(int argc, char* argv[])
{
	fallocFarMemImpl = [](unsigned __int64* pui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{

	};
	ffreeFarMemImpl = [](unsigned __int64* pui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{

	};
	//
	freadFarMemImpl = [](unsigned __int64 ui64pSrc, void* pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{

	};
	fwriteFarMemImpl = [](void* pSrc, unsigned __int64 ui64pDest, unsigned __int64 ui64Size) -> unsigned __int32
	{

	};
	//
	fexecThreadImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pParam) -> unsigned __int32
	{

	};
	//
	fmallocImpl = [](unsigned __int64 ui64size) -> unsigned __int64
	{

	};
	fmemcpyImpl = [](unsigned __int64 ui64pDest, unsigned __int64 ui64pSrc, unsigned __int64 ui64size) -> void
	{

	};
	fmemfreeImpl = [](unsigned __int64 ui64pDest) -> void
	{

	};

	unsigned __int64 ui64fgetCmdCB = 0ui64;
	unsigned __int64 ui64falcRplCB = 0ui64;
	unsigned __int64 ui64fsetRplCB = 0ui64;

	unsigned __int32 uResult = 0ui32;
	while (!uResult)
	{
		uResult = single_command(ui64fgetCmdCB, ui64falcRplCB, ui64fsetRplCB);
	}
	return uResult;
}