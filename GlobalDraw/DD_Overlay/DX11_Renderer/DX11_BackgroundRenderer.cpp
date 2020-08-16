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
inline size_t BitsPerPixel
(
	_In_ DXGI_FORMAT fmt
);
inline void GetSurfaceInfo
(
	_In_ size_t width,
	_In_ size_t height,
	_In_ DXGI_FORMAT fmt,
	_Out_opt_ size_t* outNumBytes,
	_Out_opt_ size_t* outRowBytes,
	_Out_opt_ size_t* outNumRows
);

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

ID3D11Texture2D* DX11_BackgroundRenderer::GetBackBufferTexturePtr()
{
	return this->pBackBufferTexture;
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

HRESULT DX11_BackgroundRenderer::BltTextureToSurface(D3D11_TEXTURE2D_DESC& desc, ID3D11Texture2D* pStaging, LPDIRECTDRAWSURFACE7 pDest) {
	size_t rowPitch, slicePitch, rowCount;
	GetSurfaceInfo(desc.Width, desc.Height, desc.Format, &slicePitch, &rowPitch, &rowCount);

	D3D11_MAPPED_SUBRESOURCE mapped;
	auto hr = this->pImmediateContext->Map(pStaging, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(hr))
	{
		printf(" !      pImmediateContext->Map\n");
		printf("    [-] hr = %#x\n", hr);
		return hr;
	}

	auto sptr = reinterpret_cast<const uint8_t*>(mapped.pData);
	if (!sptr)
	{
		this->pImmediateContext->Unmap(pStaging, 0);
		return E_POINTER;
	}

	DDSURFACEDESC2 dd;
	{
		ZeroMemory(&dd, sizeof(dd));
		dd.dwSize = sizeof(dd);
		auto hResult = pDest->Lock(NULL, &dd, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
		if (hResult != DD_OK)
		{
			printf(" !      pDest->Lock\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			this->pImmediateContext->Unmap(pStaging, 0);
			return hResult;
		}
	}

	{
		auto pd = dd.lPitch - (dd.dwWidth * 4);
		auto setpixel = [dd](unsigned x, unsigned y, BYTE rgba[4])
		{
			DWORD Offset = y * dd.lPitch + x * (dd.ddpfPixelFormat.dwRGBBitCount >> 3);
			auto Ptr = ((BYTE*)((DWORD)dd.lpSurface + Offset));
			Ptr[0] = rgba[2];
			Ptr[1] = rgba[1];
			Ptr[2] = rgba[0];
			Ptr[3] = rgba[3];
		}; 
		for (auto h = 0u; h < dd.dwHeight; h++)
		{
			for (auto w = 0u; w < dd.dwWidth; w++)
			{
				BYTE rgba[4] = { sptr[0], sptr[1], sptr[2], sptr[3] };
				setpixel(w, h, rgba);

				sptr += sizeof(BYTE[4]);
			}
			// pitch
			//sptr += rowPitch;
			for (auto pdi = 0u; pdi < pd; pdi++)
				setpixel(dd.dwWidth + pdi, h, 0u);
		}
	}

	{
		auto hResult = pDest->Unlock(NULL);
		if (hResult != DD_OK)
		{
			printf(" !      pDest->Unlock\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			this->pImmediateContext->Unmap(pStaging, 0);
			return hResult;
		}
	}

	this->pImmediateContext->Unmap(pStaging, 0);

	return S_OK;
}

void DX11_BackgroundRenderer::RenderFrame(DX11_BackgroundRenderer::RenderFrameCallback pCallback)
{
	pCallback(this->pDevice, this->pImmediateContext, this->pBackBuffer);
}

void DX11_BackgroundRenderer::Flip()
{
	this->swapchain->Present(0, 0);
}

inline size_t BitsPerPixel(_In_ DXGI_FORMAT fmt)
{
	switch (fmt)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
	case DXGI_FORMAT_Y416:
	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_TYPELESS:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_TYPELESS:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_AYUV:
	case DXGI_FORMAT_Y410:
	case DXGI_FORMAT_YUY2:
		return 32;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		return 24;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_A8P8:
	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return 16;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
	case DXGI_FORMAT_NV11:
		return 12;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_AI44:
	case DXGI_FORMAT_IA44:
	case DXGI_FORMAT_P8:
		return 8;

	case DXGI_FORMAT_R1_UNORM:
		return 1;

	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 4;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return 8;

#if defined(_XBOX_ONE) && defined(_TITLE)

	case DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
	case DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
	case DXGI_FORMAT_R10G10B10_SNORM_A2_UNORM:
		return 32;

	case DXGI_FORMAT_D16_UNORM_S8_UINT:
	case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
		return 24;

	case DXGI_FORMAT_R4G4_UNORM:
		return 8;

#endif // _XBOX_ONE && _TITLE

	default:
		return 0;
	}
}
inline void GetSurfaceInfo(_In_ size_t width,
	_In_ size_t height,
	_In_ DXGI_FORMAT fmt,
	_Out_opt_ size_t* outNumBytes,
	_Out_opt_ size_t* outRowBytes,
	_Out_opt_ size_t* outNumRows)
{
	size_t numBytes = 0;
	size_t rowBytes = 0;
	size_t numRows = 0;

	bool bc = false;
	bool packed = false;
	bool planar = false;
	size_t bpe = 0;
	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		bc = true;
		bpe = 8;
		break;

	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_TYPELESS:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		bc = true;
		bpe = 16;
		break;

	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_YUY2:
		packed = true;
		bpe = 4;
		break;

	case DXGI_FORMAT_Y210:
	case DXGI_FORMAT_Y216:
		packed = true;
		bpe = 8;
		break;

	case DXGI_FORMAT_NV12:
	case DXGI_FORMAT_420_OPAQUE:
		planar = true;
		bpe = 2;
		break;

	case DXGI_FORMAT_P010:
	case DXGI_FORMAT_P016:
		planar = true;
		bpe = 4;
		break;

#if defined(_XBOX_ONE) && defined(_TITLE)

	case DXGI_FORMAT_D16_UNORM_S8_UINT:
	case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
		planar = true;
		bpe = 4;
		break;

#endif
	}

	if (bc)
	{
		size_t numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = max(1, (width + 3) / 4);
		}
		size_t numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = max(1, (height + 3) / 4);
		}
		rowBytes = numBlocksWide * bpe;
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if (packed)
	{
		rowBytes = ((width + 1) >> 1) * bpe;
		numRows = height;
		numBytes = rowBytes * height;
	}
	else if (fmt == DXGI_FORMAT_NV11)
	{
		rowBytes = ((width + 3) >> 2) * 4;
		numRows = height * 2; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
		numBytes = rowBytes * numRows;
	}
	else if (planar)
	{
		rowBytes = ((width + 1) >> 1) * bpe;
		numBytes = (rowBytes * height) + ((rowBytes * height + 1) >> 1);
		numRows = height + ((height + 1) >> 1);
	}
	else
	{
		size_t bpp = BitsPerPixel(fmt);
		rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
		numRows = height;
		numBytes = rowBytes * height;
	}

	if (outNumBytes)
	{
		*outNumBytes = numBytes;
	}
	if (outRowBytes)
	{
		*outRowBytes = rowBytes;
	}
	if (outNumRows)
	{
		*outNumRows = numRows;
	}
}