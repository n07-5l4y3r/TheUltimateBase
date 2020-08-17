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

HRESULT ID2D1_Renderer::CreateRenderTarget()
{
	printf(" > Creating ID2D1RenderTarget...\n");
	D2D1_RENDER_TARGET_PROPERTIES oProps = { };
	{
		oProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		oProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		oProps.dpiX = (FLOAT)this->uWidth;
		oProps.dpiY = (FLOAT)this->uHeight;
	}
	{
		auto hResult = this->m_pDirect2dFactory->CreateDxgiSurfaceRenderTarget(this->pDestSurface, oProps, &this->m_pRenderTarget);
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

ID2D1SolidColorBrush* ID2D1_Renderer::GetBrush(RGBA rgba)
{
	if (m_mSolidBrushes.find(rgba.dwRGBA) != m_mSolidBrushes.end())
		return m_mSolidBrushes.at(rgba.dwRGBA);
	else
	{
		printf(" > Creating ID2D1SolidColorBrush...\n");
		ID2D1SolidColorBrush* pBrush = nullptr;
		{
			auto hResult = this->m_pRenderTarget->CreateSolidColorBrush({(FLOAT)rgba.sRGBA.r,(FLOAT)rgba.sRGBA.g,(FLOAT)rgba.sRGBA.b,1.f/255.f*(FLOAT)rgba.sRGBA.a}, &pBrush);
			if (hResult != S_OK)
			{
				printf(" !      m_pRenderTarget->CreateSolidColorBrush\n");
				printf("    [-] hResult = %#x\n", hResult);
				return nullptr;
			}
			printf("    [+] pBrush = %#p\r\n", pBrush);
		}
		return m_mSolidBrushes[rgba.dwRGBA] = pBrush;
	}
}

HRESULT ID2D1_Renderer::Draw(HRESULT(*pCallback)(ID2D1_Renderer* pInst, ID2D1RenderTarget* pRenderTarget))
{
	{
		auto hResult = pCallback(this, this->m_pRenderTarget);
		if (hResult != S_OK)
		{
			printf(" !      ID2D1_Renderer::Draw->pCallback\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return hResult;
		}
	}
	return S_OK;
}

ID2D1_Renderer::ID2D1_Renderer(unsigned uWidth, unsigned uHeight, IDXGISurface* pDest) : uWidth(uWidth), uHeight(uHeight), pDestSurface(pDest)
{
	printf("\n > Creating ID2D1_Renderer...\n");
	if (this->CreateFactory() != S_OK)
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
			if (i.second != nullptr)
			{
				auto color = i.second->GetColor();
				printf(" > Cleaning up Brush with Color RGBA(%f,%f,%f,%f)\n", color.r, color.g, color.b, color.a);
				i.second->Release();
				i.second = nullptr;
			}
		}
		this->m_mSolidBrushes.clear();
	}
	if (this->m_pRenderTarget)
	{
		printf(" > Cleaning up RenderTarget \n");
		this->m_pRenderTarget->Release();
	}
	if (this->m_pDirect2dFactory)
	{
		printf(" > Cleaning up Factory \n");
		this->m_pDirect2dFactory->Release();
	}
}