#pragma once

#include <iostream>
#include <map>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")
#include <ddraw.h>

#include <d3d11.h>
#include <dcommon.h>
#pragma comment(lib, "d3d11.lib")

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")

#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")

//#pragma comment(lib, "gdiplus.lib")
//#include <gdiplus.h>
//using namespace Gdiplus;

//ddraw
#include "DD_Overlay/DD_Overlay.hpp"

//d3d
#include "DX11_BackgroundRenderer/DX11_BackgroundRenderer.hpp"

#include "ID2D1_Renderer/render.h"
//d2d
#include "ID2D1_Renderer/ID2D1_Renderer.hpp"