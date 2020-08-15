#include "ID2D1_Renderer.hpp"

HRESULT ID2D1_Renderer::CreateFactory()
{
	printf(" > Creating D2D1Factory...\n");
	{
		auto hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &this->m_pDirect2dFactory);
		if (hResult != S_OK)
		{
			printf(" !      D2D1CreateFactory\n");
			printf("    [-] hResult = %#x\n", hResult);
			return hResult;
		}
		printf("    [+] m_pDirect2dFactory = %#p\r\n", this->m_pDirect2dFactory);
	}
	return S_OK;
}

HRESULT ID2D1_Renderer::CreateSurface()
{
	printf(" > Creating IDXGISurface...\n");
	{
		auto hResult = this->pDestTexture->QueryInterface< IDXGISurface >(&this->m_pDestSurface);
		if (hResult != S_OK)
		{
			printf(" !      pDestTexture->QueryInterface< IDXGISurface >(&m_pDestSurface)\n");
			printf("    [-] hResult = %#x\n", hResult);
			return hResult;
		}
		printf("    [+] m_pDestSurface = %#p\r\n", this->m_pDestSurface);
	}
	return S_OK;
}

HRESULT ID2D1_Renderer::CreateRenderTarget()
{
	printf(" > Creating ID2D1RenderTarget...\n");
	D2D1_RENDER_TARGET_PROPERTIES oProps = { };
	{
		oProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		oProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
		oProps.dpiX = (FLOAT)this->uWidth;
		oProps.dpiY = (FLOAT)this->uHeight;
	}
	{
		auto hResult = this->m_pDirect2dFactory->CreateDxgiSurfaceRenderTarget(this->m_pDestSurface, oProps, &this->m_pRenderTarget);
		if (hResult != S_OK)
		{
			printf(" !      m_pDirect2dFactory->CreateDxgiSurfaceRenderTarget\n");
			printf("    [-] hResult = %#x\n", hResult);
			return hResult;
		}
		printf("    [+] m_pRenderTarget = %#p\r\n", this->m_pRenderTarget);
	}
	return S_OK;
}

ID2D1SolidColorBrush* ID2D1_Renderer::GetBrush(COLORREF rgb)
{
	if (m_mSolidBrushes.find(rgb) != m_mSolidBrushes.end())
		return m_mSolidBrushes.at(rgb);
	else
	{
		printf(" > Creating ID2D1SolidColorBrush...\n");
		ID2D1SolidColorBrush* pBrush = nullptr;
		{
			auto hResult = this->m_pRenderTarget->CreateSolidColorBrush({ (FLOAT)GetRValue(rgb),(FLOAT)GetGValue(rgb),(FLOAT)GetBValue(rgb),255.f }, &pBrush);
			if (hResult != S_OK)
			{
				printf(" !      m_pRenderTarget->CreateSolidColorBrush\n");
				printf("    [-] hResult = %#x\n", hResult);
				return nullptr;
			}
			printf("    [+] pBrush = %#p\r\n", pBrush);
		}
		return m_mSolidBrushes[rgb] = pBrush;
	}
}

HRESULT ID2D1_Renderer::Update(UpdateCallback pCallback)
{
	return pCallback(this, this->m_pRenderTarget);
}

ID2D1_Renderer::ID2D1_Renderer(unsigned uWidth, unsigned uHeight, HWND hWnd, ID3D11Texture2D* pDest) : uWidth(uWidth), uHeight(uHeight), hWnd(hWnd), pDestTexture(pDest)
{
	printf("\n > Creating ID2D1_Renderer...\n");
	if (this->CreateFactory() != S_OK)
		return;
	if (this->CreateSurface() != S_OK)
		return;
	if (this->CreateRenderTarget() != S_OK)
		return;
}

ID2D1_Renderer::~ID2D1_Renderer()
{
	printf("\n > Cleaning up ID2D1_Renderer...\n");
	if (this->m_mSolidBrushes.size())
	{
		for (auto& i : this->m_mSolidBrushes)
		{
			printf(" > Cleaning up Brush with Color RGB(%u,%u,%u)\n", GetRValue(i.first), GetGValue(i.first), GetBValue(i.first));
			i.second->Release();
			i.second = nullptr;
		}
		this->m_mSolidBrushes.clear();
	}
	if (this->m_pRenderTarget)
	{
		printf(" > Cleaning up RenderTarget \n");
		this->m_pRenderTarget->Release();
	}
	if (this->m_pDestSurface)
	{
		printf(" > Cleaning up Surface \n");
		this->m_pDestSurface->Release();
	}
	if (this->m_pDirect2dFactory)
	{
		printf(" > Cleaning up Factory \n");
		this->m_pDirect2dFactory->Release();
	}
}