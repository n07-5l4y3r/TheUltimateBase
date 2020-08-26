#pragma once

#include <inc.hpp>

class ID2D1_Renderer {
	unsigned uWidth, uHeight;
	IDXGISurface* pDestSurface;
	//
public:
	ID2D1Factory* m_pDirect2dFactory = nullptr;
	ID2D1RenderTarget* m_pRenderTarget = nullptr;
	IDWriteFactory* dwrite_factory = nullptr;
	// 
private:
	std::map< DWORD, ID2D1SolidColorBrush* > m_mSolidBrushes;
private:
	HRESULT CreateFactory();
	HRESULT CreateRenderTarget();
public:
	ID2D1SolidColorBrush* GetBrush(RGBA rgba);
	HRESULT Draw(HRESULT(*pCallback)(ID2D1_Renderer* pInst, ID2D1RenderTarget* pRenderTarget));
public:
	ID2D1_Renderer(unsigned uWidth, unsigned uHeight, IDXGISurface* pDest);
	~ID2D1_Renderer();
};