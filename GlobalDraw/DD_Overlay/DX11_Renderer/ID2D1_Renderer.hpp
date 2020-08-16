#pragma once

#include <inc.hpp>

union RGBA {
	struct {
		BYTE r;
		BYTE g;
		BYTE b;
		BYTE a;
	} sRGBA;
	DWORD dwRGBA;
};

class ID2D1_Renderer {
	unsigned uWidth, uHeight;
	HWND hWnd;
	ID3D11Texture2D* pDestTexture;
	//
	ID2D1Factory* m_pDirect2dFactory = nullptr;
	IDXGISurface* m_pDestSurface = nullptr;
	ID2D1RenderTarget* m_pRenderTarget = nullptr;
	//
	std::map< DWORD, ID2D1SolidColorBrush* > m_mSolidBrushes;
private:
	HRESULT CreateFactory();
	HRESULT CreateSurface();
	HRESULT CreateRenderTarget();
public:
	ID2D1SolidColorBrush* GetBrush(RGBA rgba);
	typedef HRESULT(* UpdateCallback)(ID2D1_Renderer* pInst, ID2D1RenderTarget* pRenderTarget);
	HRESULT Update(UpdateCallback pCallback);
public:
	ID2D1_Renderer(unsigned uWidth, unsigned uHeight, HWND hWnd, ID3D11Texture2D* pDest);
	~ID2D1_Renderer();
};