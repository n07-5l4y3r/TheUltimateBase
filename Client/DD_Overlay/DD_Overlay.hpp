#pragma once

#include <inc.hpp>

class DD_Overlay
{
private:
	HWND					hWnd;
	unsigned				uWidth, uHeight;
	//
	LPDDPIXELFORMAT			pPixelFormat = nullptr;
	IDirectDraw7*			pIDirectDraw7 = nullptr;
	//
	LPDIRECTDRAWSURFACE7	pPrimary = nullptr;
	LPDIRECTDRAWSURFACE7	pBuffer = nullptr;
	LPDIRECTDRAWSURFACE7	pBackBuffer = nullptr;
private:
	//						0xAARRGGBB
	LPDDPIXELFORMAT			DefinePixelFormat(); 
	IDirectDraw7*			CreateDevice();
	LPDIRECTDRAWSURFACE7	CreatePrimary();
	LPDIRECTDRAWSURFACE7	CreateBuffer();
	LPDIRECTDRAWSURFACE7	CreateBackBuffer();
	//
	IDXGISurface*			pDXGISurface = nullptr;
public:
	IDXGISurface*			QueryIDXGISurface();
public:
	DD_Overlay(unsigned width = 1920u, unsigned height = 1080u, HWND hWnd = GetConsoleWindow());
	~DD_Overlay();
public:
	HRESULT Draw(HRESULT(*pCallback)(IDirectDraw7* pDevice, LPDIRECTDRAWSURFACE7 pSurface));
	HRESULT Flip();
	HRESULT UpdateOverlay();
public:
	HRESULT Restore();
};