#include "DX11_BackgroundRenderer.hpp"

const D3D_FEATURE_LEVEL cpFeatureLevels[] = 
{
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
};

HRESULT DX11_BackgroundRenderer::CreateDevice()
{
	printf(" > Creating DirectX11Device...\n");

	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = this->hWnd;                          // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.SampleDesc.Quality = 0;                             // multisample quality level
	scd.Windowed = TRUE;                                    // windowed/full-screen mode

	// create a device, device context and swap chain using the information in the scd struct
	auto hResult = D3D11CreateDeviceAndSwapChain(
		NULL,                                                   //Pass NULL to use the default adapter, which is the first adapter that is enumerated by IDXGIFactory1::EnumAdapters.
		D3D_DRIVER_TYPE_HARDWARE,                               //hardware abstraction layer or HAL.
		NULL,                                                   //If DriverType is D3D_DRIVER_TYPE_SOFTWARE, Software must not be NULL
		D3D11_CREATE_DEVICE_DEBUG |                             //Creates a device that supports the debug layer.
		D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		cpFeatureLevels,                                        //If the Direct3D 11.1 runtime is present on the computer and pFeatureLevels is set to NULL, this function won't create a D3D_FEATURE_LEVEL_11_1 device.
		sizeof(cpFeatureLevels) / sizeof(D3D_FEATURE_LEVEL),    //The number of elements in pFeatureLevels.
		D3D11_SDK_VERSION,                                      //The SDK version; use D3D11_SDK_VERSION.
		&scd,
		//------------------------------
		&this->swapchain,
		&this->pDevice,                                         //Returns the address of a pointer to an ID3D11Device object that represents the device created.
		&this->curFeatureLevel,                                 //If successful, returns the first D3D_FEATURE_LEVEL from the cpFeatureLevels array which succeeded.
		&this->pImmediateContext                                //Returns the address of a pointer to an ID3D11DeviceContext object that represents the device context.
	);
	if (hResult != S_OK)
	{
		printf(" !      D3D11CreateDeviceAndSwapChain\n");
		printf("    [-] hResult = %#x\n", hResult);
		return hResult;
	}
	printf("    [+] swapchain = %#p\r\n", this->swapchain);
	printf("    [+] pDevice = %#p\r\n", this->pDevice);
	printf("    [+] curFeatureLevel = %#x\r\n", this->curFeatureLevel);
	printf("    [+] pImmediateContext = %#p\r\n", this->pImmediateContext);
	return S_OK;
}

HRESULT DX11_BackgroundRenderer::CreateBackBuffer()
{
	printf(" > Creating BackBuffer...\n");

	// get the address of the back buffer
	auto hResult = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&this->pBackBufferTexture);
	if (hResult != S_OK)
	{
		printf(" !      swapchain->GetBuffer\n");
		printf("    [-] hResult = %#x\n", hResult);
		return hResult;
	}
	printf("    [+] pBackBufferTexture = %#p\r\n", this->pBackBufferTexture);

	//Get the properties of the texture resource.
	this->pBackBufferTexture->GetDesc(&this->BackBufferTextureDesc);

	return S_OK;
}

HRESULT DX11_BackgroundRenderer::CreateRenderTarget()
{
	printf(" > Creating Render Target...\n");

	// use the back buffer address to create the render target
	auto hResult = this->pDevice->CreateRenderTargetView(this->pBackBufferTexture, NULL, &this->pBackBuffer);
	if (hResult != S_OK)
	{
		printf(" !      pDevice->CreateRenderTargetView\n");
		printf("    [-] hResult = %#x\n", hResult);
		return hResult;
	}
	printf("    [+] pBackBuffer = %#p\r\n", this->pBackBuffer);

	this->pBackBufferTexture->Release();

	// set the render target as the back buffer
	this->pImmediateContext->OMSetRenderTargets(1, &this->pBackBuffer, NULL);

	return S_OK;
}

void DX11_BackgroundRenderer::SetViewPort()
{
	printf(" > Setting ViewPort...\n");

	// Set the viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)this->uWidth;
	viewport.Height = (FLOAT)this->uHeight;

	this->pImmediateContext->RSSetViewports(1, &viewport);
}

IDXGISurface* DX11_BackgroundRenderer::GpDXGISurface()
{
	if (!this->pIDXGISurface)
	{
		printf("\n > Creating IDXGISurface...\n");
		auto hResult = this->pBackBufferTexture->QueryInterface<IDXGISurface>(&this->pIDXGISurface);
		if (hResult != S_OK)
		{
			printf(" !      pBackBufferTexture->QueryInterface<IDXGISurface>(&pIDXGISurface)\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
		printf("    [+] pIDXGISurface = %#p\r\n", this->pIDXGISurface);
	}
	return this->pIDXGISurface;
}

ID3D11Device* DX11_BackgroundRenderer::GpDevice()
{
	return this->pDevice;
}

ID3D11DeviceContext* DX11_BackgroundRenderer::GpDeviceContext()
{
	return this->pImmediateContext;
}

ID3D11RenderTargetView* DX11_BackgroundRenderer::GpRenderTargetView()
{
	return this->pBackBuffer;
}

HRESULT DX11_BackgroundRenderer::CaptureTexture(D3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D*& pStaging) {
	auto pContext = this->pImmediateContext;
	auto pSource = this->pBackBufferTexture;

	if (!pContext || !pSource)
	{
		printf(" !      !pContext || !pSource\n");
		printf("    [-] hResult = %#x\n", E_INVALIDARG);
		return E_INVALIDARG;
	}

	D3D11_RESOURCE_DIMENSION resType = D3D11_RESOURCE_DIMENSION_UNKNOWN;
	pSource->GetType(&resType);

	if (resType != D3D11_RESOURCE_DIMENSION_TEXTURE2D)
		return HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED);

	ID3D11Texture2D* pTexture;
	HRESULT hr = pSource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pTexture));
	if (FAILED(hr))
	{
		printf(" !      pSource->QueryInterface\n");
		printf("    [-] hResult = %#x\n", hr);
		return hr;
	}

	pTexture->GetDesc(&desc);

	ID3D11Device* d3dDevice;
	pContext->GetDevice(&d3dDevice);

	if (desc.SampleDesc.Count > 1)
	{
		// MSAA content must be resolved before being copied to a staging texture
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		ID3D11Texture2D* pTemp;
		hr = d3dDevice->CreateTexture2D(&desc, 0, &pTemp);
		if (FAILED(hr))
		{
			printf(" !      d3dDevice->CreateTexture2D\n");
			printf("    [-] hResult = %#x\n", hr);
			return hr;
		}

		DXGI_FORMAT fmt = /*EnsureNotTypeless(*/desc.Format/*)*/;

		UINT support = 0;
		hr = d3dDevice->CheckFormatSupport(fmt, &support);
		if (FAILED(hr))
		{
			printf(" !      d3dDevice->CheckFormatSupport\n");
			printf("    [-] hr = %#x\n", hr);
			return hr;
		}

		if (!(support & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE))
			return E_FAIL;

		for (UINT item = 0; item < desc.ArraySize; ++item)
		{
			for (UINT level = 0; level < desc.MipLevels; ++level)
			{
				UINT index = D3D11CalcSubresource(level, item, desc.MipLevels);
				pContext->ResolveSubresource(pTemp, index, pSource, index, fmt);
			}
		}

		desc.BindFlags = 0;
		desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		hr = d3dDevice->CreateTexture2D(&desc, 0, &pStaging);
		if (FAILED(hr))
		{
			printf(" !      d3dDevice->CreateTexture2D\n");
			printf("    [-] hr = %#x\n", hr);
			return hr;
		}

		pContext->CopyResource(pStaging, pTemp);
	}
	else if ((desc.Usage == D3D11_USAGE_STAGING) && (desc.CPUAccessFlags & D3D11_CPU_ACCESS_READ))
	{
		// Handle case where the source is already a staging texture we can use directly
		pStaging = pTexture;
	}
	else
	{
		// Otherwise, create a staging texture from the non-MSAA source
		desc.BindFlags = 0;
		desc.MiscFlags &= D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;

		hr = d3dDevice->CreateTexture2D(&desc, 0, &pStaging);
		if (FAILED(hr))
		{
			printf(" !      d3dDevice->CreateTexture2D\n");
			printf("    [-] hr = %#x\n", hr);
			return hr;
		}

		pContext->CopyResource(pStaging, pSource);
	}

	return S_OK;
}

DX11_BackgroundRenderer::DX11_BackgroundRenderer(unsigned uWidth, unsigned uHeight, HWND hWnd) : uWidth(uWidth), uHeight(uHeight), hWnd(hWnd)
{
	printf("\n > Creating BackgroundRenderer...\n");

	this->uWidth = uWidth;
	this->uHeight = uHeight;
	this->hWnd = hWnd;
	//
	if (CreateDevice() != S_OK)
		return;
	if (CreateBackBuffer() != S_OK)
		return;
	if (this->GpDXGISurface() == nullptr)
		return;
	if (CreateRenderTarget() != S_OK)
		return;
	SetViewPort();
}

DX11_BackgroundRenderer::~DX11_BackgroundRenderer()
{
	printf("\n > Cleaning up BackgroundRenderer...\n");

	// close and release all existing COM objects
	if (this->swapchain)
	{
		printf(" > Cleaning up swapchain \n");
		this->swapchain->Release();
	}
	if (this->pBackBuffer)
	{
		printf(" > Cleaning up pBackBuffer \n");
		this->pBackBuffer->Release();
	}
	if (this->pDevice)
	{
		printf(" > Cleaning up pDevice \n");
		this->pDevice->Release();
	}
	if (this->pImmediateContext)
	{
		printf(" > Cleaning up pImmediateContext \n");
		this->pImmediateContext->Release();
	}
}

HRESULT DX11_BackgroundRenderer::Draw(HRESULT(*pCallback)(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView))
{
	{
		auto hResult = pCallback(this->pDevice, this->pImmediateContext, this->pBackBuffer);
		if (hResult != S_OK)
		{
			printf(" !      DX11_BackgroundRenderer::Draw->pCallback\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return hResult;
		}
	}
	return S_OK;
}

HRESULT DX11_BackgroundRenderer::Present()
{
	{
		auto hResult = this->swapchain->Present(0, 0);
		if (hResult != S_OK)
		{
			printf(" !      DX11_BackgroundRenderer::Present\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return hResult;
		}
	}
	return S_OK;
}