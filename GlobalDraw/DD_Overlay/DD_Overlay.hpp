#pragma once

#include <inc.hpp>

class DD_Overlay
{
private:
	HWND hWnd;
	unsigned uWidth;
	unsigned uHeight;
	//
	IDirectDraw7* pIDirectDraw7 = nullptr;
	//
	LPDIRECTDRAWSURFACE7 pPrimary = nullptr;
	LPDIRECTDRAWSURFACE7 pBuffer = nullptr;
	LPDIRECTDRAWSURFACE7 pBackBuffer = nullptr;
private:
	IDirectDraw7* CreateDevice();
	//
	LPDIRECTDRAWSURFACE7 CreatePrimary();
	LPDIRECTDRAWSURFACE7 CreateBuffer();
	LPDIRECTDRAWSURFACE7 CreateBackBuffer();
public:
	DD_Overlay(unsigned width = 1920u, unsigned height = 1080u, HWND hWnd = GetConsoleWindow());
	~DD_Overlay();
private:
	//HRESULT BlitRGB32(Bitmap* src, LPDIRECTDRAWSURFACE7 dest);
	HRESULT BlitDX11Resource(/*DX11_BackgroundRenderer*/void* pRenderer);
public:
	HRESULT Update(/*DX11_BackgroundRenderer*/void* pRenderer);
public:
	HRESULT Restore();
};