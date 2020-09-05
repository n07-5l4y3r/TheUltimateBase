#pragma once

#include <inc.hpp>

namespace kdmapper
{
	uint64_t MapDriver(HANDLE iqvw64e_device_handle, const std::string& driver_path);
	void RelocateImageByDelta(portable_executable::vec_relocs relocs, const uint64_t delta);
	bool ResolveImports(HANDLE iqvw64e_device_handle, portable_executable::vec_imports imports);
}