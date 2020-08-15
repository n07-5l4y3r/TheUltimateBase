#pragma once

#include <inc.hpp>

class DX11_BackgroundRenderer;
class ID2D1_Renderer;

class DX11_Renderer {
	unsigned uWidth, uHeight;
	HWND hWnd;
	//
	ID3D11Texture2D* pShared_3d_2d_texture = nullptr;
	//
	DX11_BackgroundRenderer* pDX11_BackgroundRenderer = nullptr;
	ID2D1_Renderer* pID2D1_Renderer = nullptr;
public:
	DX11_BackgroundRenderer* GetDX11_BackgroundRenderer();
	ID2D1_Renderer* GetID2D1_Renderer();
public:
	DX11_Renderer(unsigned uWidth, unsigned uHeight, HWND hWnd);
	~DX11_Renderer();
};

#include "DX11_BackgroundRenderer.hpp"
#include "ID2D1_Renderer.hpp"