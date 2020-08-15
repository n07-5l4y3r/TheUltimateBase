#include "DX11_Renderer.hpp"

DX11_BackgroundRenderer* DX11_Renderer::GetDX11_BackgroundRenderer()
{
	return this->pDX11_BackgroundRenderer;
}

ID2D1_Renderer* DX11_Renderer::GetID2D1_Renderer()
{
	return this->pID2D1_Renderer;
}

DX11_Renderer::DX11_Renderer(unsigned uWidth, unsigned uHeight, HWND hWnd) : uWidth(uWidth), uHeight(uHeight), hWnd(hWnd)
{
	this->pDX11_BackgroundRenderer = new DX11_BackgroundRenderer(this->uWidth, this->uHeight, this->hWnd);
	this->pShared_3d_2d_texture = this->pDX11_BackgroundRenderer->GetBackBufferTexturePtr();
	this->pID2D1_Renderer = new ID2D1_Renderer(this->uWidth, this->uHeight, this->hWnd, this->pShared_3d_2d_texture);
}

DX11_Renderer::~DX11_Renderer()
{
	delete this->pDX11_BackgroundRenderer;
	delete this->pID2D1_Renderer;
}
