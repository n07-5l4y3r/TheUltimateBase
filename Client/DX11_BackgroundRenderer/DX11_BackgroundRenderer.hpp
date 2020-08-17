#pragma once

#include <inc.hpp>

class DX11_BackgroundRenderer {
	unsigned uWidth;
	unsigned uHeight;
	HWND hWnd;
	//
	ID3D11Device*				pDevice = nullptr;
	ID3D11DeviceContext*		pImmediateContext = nullptr;
	D3D_FEATURE_LEVEL			curFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
	IDXGISwapChain*				swapchain = nullptr;
	//
	D3D11_TEXTURE2D_DESC		BackBufferTextureDesc = { 0 };
	ID3D11Texture2D*			pBackBufferTexture = nullptr;
	ID3D11RenderTargetView*		pBackBuffer = nullptr;
	//
	IDXGISurface*				pIDXGISurface = nullptr;
private:
	HRESULT CreateDevice();
	HRESULT CreateBackBuffer();
	HRESULT CreateRenderTarget();
	void	SetViewPort();
public:
	IDXGISurface* QuerypDXGISurface();
	HRESULT CaptureTexture(D3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D*& pStaging);
public:
	DX11_BackgroundRenderer(unsigned uWidth, unsigned uHeight, HWND hWnd);
	~DX11_BackgroundRenderer();
public:
	HRESULT Draw(HRESULT(*pCallback)(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView));
	HRESULT Present();
};