#pragma once

#define _CRT_SECURE_NO_WARNINGS 1

#include <Windows.h>

#include <stdint.h>

#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <thread>

#include "kdmapper/portable_executable/portable_executable.hpp"
#include "kdmapper/utils/utils.hpp"
#include "kdmapper/nt/nt.hpp"
#include "kdmapper/intel_driver/intel_driver.hpp"
#include "kdmapper/kdmapper.hpp"

#include <queue>
#include <future>
#include <chrono>

#define DEBUG_VAR(v) printf(" %s: %#llx" "\n", #v, v )

#include <array>