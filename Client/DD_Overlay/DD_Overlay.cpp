#include "DD_Overlay.hpp"

LPDDPIXELFORMAT DD_Overlay::DefinePixelFormat()
{
	printf(" > Defining PixelFormat...\n");
	if (!this->pPixelFormat)
	{
		this->pPixelFormat = reinterpret_cast<LPDDPIXELFORMAT>(malloc(sizeof(DDPIXELFORMAT)));
		{
			this->pPixelFormat->dwSize = sizeof(DDPIXELFORMAT);
			this->pPixelFormat->dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
			this->pPixelFormat->dwFourCC = 0;
			this->pPixelFormat->dwRGBBitCount = 32;
			this->pPixelFormat->dwRBitMask = 0x00ff0000;
			this->pPixelFormat->dwGBitMask = 0x0000ff00;
			this->pPixelFormat->dwBBitMask = 0x000000ff;
			this->pPixelFormat->dwRGBAlphaBitMask = 0xff000000;
		}
	}
	return this->pPixelFormat;
}

IDirectDraw7* DD_Overlay::CreateDevice()
{
	printf(" > Creating DirectDraw Device...\n");
	{
		LPVOID lpDD = nullptr;
		auto hResult = DirectDrawCreateEx(
			NULL,               //This can be NULL to indicate the active display driver
			//------------------------------
			&lpDD,              //A pointer to a variable to be set to a valid IDirectDraw7 interface pointer if the call succeeds.
			//------------------------------
			IID_IDirectDraw7,   //This parameter must be set to IID_IDirectDraw7. This function fails and returns DDERR_INVALIDPARAMS if any other interface is specified.
			NULL                //Allows for future compatibility with COM aggregation features. Currently, this function returns an error if this parameter is not NULL.
		);
		if (hResult != DD_OK)
		{
			printf(" !      DirectDrawCreateEx\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
		this->pIDirectDraw7 = static_cast<IDirectDraw7*>(lpDD);
	}
	printf("    [+] pIDirectDraw7 = %#p\r\n", this->pIDirectDraw7);
	printf(" > Setting Cooperative Level...\n");
	{
		auto hResult = this->pIDirectDraw7->SetCooperativeLevel(
			this->hWnd,								//This parameter can be NULL when the DDSCL_NORMAL flag is specified in the dwFlags parameter.
			DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN      //DDSCL_EXCLUSIVE allows us to change video modes, and requires the DDSCL_FULLSCREEN flag, which will cause the window to take over the fullscreen.
		);
		if (hResult != DD_OK)
		{
			printf(" !      SetCooperativeLevel\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
	}
	return this->pIDirectDraw7;
}

LPDIRECTDRAWSURFACE7 DD_Overlay::CreatePrimary() {
	printf(" > Creating Primary Surface...\n");
	{
		DDSURFACEDESC2 ddsd = { 0 };                    //A structure to describe the surfaces we want
		ZeroMemory(&ddsd, sizeof(ddsd));                //Clear all members of the structure to 0
		ddsd.dwSize = sizeof(ddsd);                     //The first parameter of the structure must contain the size of the structure
		ddsd.dwFlags = DDSD_CAPS;                       //The ddsCaps member is valid.
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;   //This surface is the primary surface. It represents what is visible to the user at the moment.
		auto hResult = this->pIDirectDraw7->CreateSurface(
			&ddsd,              //Address of a DDSURFACEDESC2 structure that describes the requested surface.
			//------------------------------
			&this->pPrimary,    //Address of a variable to be set to a valid IDirectDrawSurface7 interface pointer if the call succeeds.
			NULL                //Allows for future compatibility with COM aggregation features.Currently, this method returns an error if this parameter is not NULL.
		);
		if (hResult != DD_OK)
		{
			printf(" !      CreateSurface\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
	}
	printf("    [+] pPrimary = %#p\r\n", this->pPrimary);
	return this->pPrimary;
}

LPDIRECTDRAWSURFACE7 DD_Overlay::CreateBuffer() {
	printf(" > Creating Buffer Surface...\n");
	{
		DDSURFACEDESC2 ddsd = { 0 };                            //A structure to describe the surfaces we want
		ZeroMemory(&ddsd, sizeof(ddsd));                        //Clear all members of the structure to 0
		ddsd.dwSize = sizeof(ddsd);                             //The first parameter of the structure must contain the size of the structure
		ddsd.dwFlags =
			DDSD_CAPS |                                         //The ddsCaps member is valid.
			DDSD_WIDTH |                                        //The dwWidth member is valid.
			DDSD_HEIGHT |                                       //The dwHeight member is valid.
			DDSD_BACKBUFFERCOUNT |                              //The dwBackBufferCount member is valid.
			DDSD_PIXELFORMAT;                                   //The ddpfPixelFormat member is valid.
		ddsd.ddsCaps.dwCaps =
			DDSCAPS_FLIP |                                      //This surface is a part of a surface flipping structure. /* The dwBackBufferCount member of the DDSURFACEDESC structure must be set to at least 1 in order for the method call to succeed. The DDSCAPS_COMPLEX capability must always be set when creating multiple surfaces by using the CreateSurface method. */
			DDSCAPS_OVERLAY |                                   //This surface is an overlay. The visibility of this overlay depends on whether it is currently being overlaid onto the primary surface.
			DDSCAPS_COMPLEX |                                   //A complex surface is being described. A complex surface results in the creation of more than one surface.
			DDSCAPS_VIDEOMEMORY;                                //This surface exists in display memory.
		ddsd.dwWidth = this->uWidth;                            //Specifies the width of the surface in pixels.
		ddsd.dwHeight = this->uHeight;                          //Specifies the height of surface in pixels.
		ddsd.dwBackBufferCount = 1u;                            //Specifies the number of back buffers associated with the surface.
		memcpy(&ddsd.ddpfPixelFormat, this->pPixelFormat, sizeof(DDPIXELFORMAT));
		auto hResult = this->pIDirectDraw7->CreateSurface(
			&ddsd,              //Address of a DDSURFACEDESC2 structure that describes the requested surface.
			//------------------------------
			&this->pBuffer,     //Address of a variable to be set to a valid IDirectDrawSurface7 interface pointer if the call succeeds.
			NULL                //Allows for future compatibility with COM aggregation features.Currently, this method returns an error if this parameter is not NULL.
		);
		if (hResult != DD_OK)
		{
			printf(" !      CreateSurface\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
	}
	printf("    [+] pBuffer = %#p\r\n", this->pBuffer);
	return this->pBuffer;
}

LPDIRECTDRAWSURFACE7 DD_Overlay::CreateBackBuffer() {
	printf(" > Creating BackBuffer Surface...\n");
	{
		DDSCAPS2 oCaps = { 0 };             //A structure to describe the capabilities of a Microsoft DirectDraw surface object.
		ZeroMemory(&oCaps, sizeof(oCaps));  //Clear all members of the structure to 0
		oCaps.dwCaps = DDSCAPS_BACKBUFFER;  //This surface is the back buffer of a surface flipping structure.
		auto hResult = this->pBuffer->GetAttachedSurface(
			&oCaps,         //A pointer to a DDSCAPS2 structure that indicates the hardware capabilities of the attached surface.
			//------------------------------
			&this->pBackBuffer    //A pointer to a variable to receive a pointer to the retrieved surface's IDirectDrawSurface7 interface. The retrieved surface is the one that matches the description, according to the lpDDSCaps parameter.
		);
		if (hResult != DD_OK)
		{
			printf(" !      GetAttachedSurface\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
	}
	printf("    [+] pBackBuffer = %#p\r\n", this->pBackBuffer);
	return this->pBackBuffer;
}

IDXGISurface* DD_Overlay::QueryIDXGISurface()
{
	if ( this->pDXGISurface == nullptr )
	{
		auto hResult = this->pBackBuffer->QueryInterface(__uuidof(IDXGISurface), (void**)&this->pDXGISurface);
		if (hResult != DD_OK)
		{
			printf(" !      pBackBuffer->QueryInterface<IDXGISurface>(&pDXGISurface)\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return nullptr;
		}
	}
	return this->pDXGISurface;
}

DD_Overlay::DD_Overlay(unsigned width, unsigned height, HWND hWnd) : hWnd(hWnd), uWidth(width), uHeight(height) {
	printf("\n > Creating DD_Overlay...\n");
	if (!DefinePixelFormat())
		return;
	if (!CreateDevice())
		return;
	if (!CreatePrimary())
		return;
	if (!CreateBuffer())
		return;
	if (!CreateBackBuffer())
		return;
}

DD_Overlay::~DD_Overlay()
{
	printf("\n > Cleaning up DD_Overlay...\n");
	if (this->pBackBuffer)
	{
		printf(" > Cleaning up pBackBuffer...\n");
		this->pBackBuffer->Release();
	}
	if (this->pBuffer)
	{
		printf(" > Cleaning up pBuffer...\n");
		this->pBuffer->Release();
	}
	if (this->pPrimary)
	{
		printf(" > Cleaning up pPrimary...\n");
		this->pPrimary->Release();
	}
	if (this->pIDirectDraw7)
	{
		printf(" > Cleaning up pIDirectDraw7...\n");
		this->pIDirectDraw7->Release();
	}
	if (this->pPixelFormat)
	{
		printf(" > Cleaning up pPixelFormat...\n");
		delete this->pPixelFormat;
	}
	if (this->pDXGISurface)
	{
		printf(" > Cleaning up pDXGISurface...\n");
		this->pDXGISurface->Release();
	}
}

HRESULT DD_Overlay::Draw(HRESULT(*pCallback)(IDirectDraw7* pDevice, LPDIRECTDRAWSURFACE7 pSurface))
{
	{
		auto hResult = pCallback(this->pIDirectDraw7, this->pBackBuffer);
		if (hResult != DD_OK)
		{
			printf(" !      DD_Overlay::Draw->pCallback\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return hResult;
		}
	}
	return DD_OK;
}

HRESULT DD_Overlay::Flip()
{
	{
		auto hResult = this->pBuffer->Flip(NULL, DDFLIP_WAIT);
		if (hResult != DD_OK)
		{
			printf(" !      pBuffer->Flip\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			return hResult;
		}
	}
	return S_OK;
}

HRESULT DD_Overlay::UpdateOverlay()
{
	RECT boundings = { 0 };
	boundings.left = 0;
	boundings.right = uWidth;
	boundings.top = 0;
	boundings.bottom = uHeight;
	{
		DDOVERLAYFX effects = { 0 };
		{
			ZeroMemory(&effects, sizeof(effects));
			effects.dwSize = sizeof(effects);
			effects.dckDestColorkey.dwColorSpaceHighValue = effects.dckDestColorkey.dwColorSpaceLowValue = 0; // RGB(0,0,0) == Transparent
			effects.dwAlphaDestConstBitDepth = 24;
			effects.dwAlphaSrcConstBitDepth = 24;
		}
		auto hResult = this->pBuffer->UpdateOverlay(NULL, pPrimary, &boundings, DDOVER_SHOW | DDOVER_ALPHASRC | DDOVER_KEYSRCOVERRIDE | DDOVER_DDFX, &effects);
		if (hResult != DD_OK)
		{
			printf(" !      pBuffer->UpdateOverlay\n");
			printf("    [-] hResult = %#x%s\n", hResult, hResult == DDERR_NOOVERLAYHW ? " [DDERR_NOOVERLAYHW]\n" : "");
			return hResult == DDERR_NOOVERLAYHW ? DD_OK : hResult;
		}
	}
	return DD_OK;
}

HRESULT DD_Overlay::Restore()
{
	printf(" > Restoring...\n");
	for (auto pSurface : { this->pPrimary , this->pBuffer , this->pBackBuffer })
		if (pSurface->IsLost() == DDERR_SURFACELOST)
		{
			auto hResult = pSurface->Restore();
			if (hResult == DDERR_WRONGMODE)
			{
				printf(" !      pSurface->Restore\n");
				printf("    [-] hResult = %#x [DDERR_WRONGMODE]\r\n", hResult);
				return hResult;
			}
		}
	return DD_OK;
}