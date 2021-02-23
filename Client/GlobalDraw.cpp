// DirectDrawFun.cpp

#include <inc.hpp>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
	{
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

HWND CreateRenderWindow(unsigned uWidth, unsigned uHeight)
{
	printf("\n > Creating Render Preview Window...\n");

	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = L"rendererWC";

	RegisterClassEx(&wc);

	RECT wr = { 0, 0, (LONG)uWidth, (LONG)uHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	hWnd = CreateWindowEx(NULL,
		L"rendererWC",
		L"DirectX11 Renderer Preview",
		WS_OVERLAPPEDWINDOW,
		300,
		300,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	ShowWindow(hWnd, SW_SHOW);

	return hWnd;
}

HRESULT Blt_DXGISurface_To_DIRECTDRAWSURFACE7(IDirectDraw7* pDevice, IDXGISurface* pSrc, LPDIRECTDRAWSURFACE7 pDest)
{
	// Check Source Format
	bool SrcFormatIsRGBA = false;
	DXGI_SURFACE_DESC ds = { 0 };
	{
		ZeroMemory(&ds, sizeof(DXGI_SURFACE_DESC));
		{
			auto hResult = pSrc->GetDesc(&ds);
			if (hResult != S_OK)
			{
				printf(" !      pSrc->GetDesc\n");
				printf("    [-] hResult = %#x\r\n", hResult);
				return hResult;
			}
		}
		for (auto o :
			{
				DXGI_FORMAT_R8G8B8A8_TYPELESS,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
				DXGI_FORMAT_R8G8B8A8_UINT,
				DXGI_FORMAT_R8G8B8A8_SNORM,
				DXGI_FORMAT_R8G8B8A8_SINT
			})
			if (ds.Format == o)
			{
				SrcFormatIsRGBA = true;
				continue;
			}
		if (!SrcFormatIsRGBA)
		{
			printf(" !      SrcFormatIs<NOT>RGBA\n");
			return DD_FALSE;
		}
	}
	// Lock Destination
	DDSURFACEDESC2 dd;
	{
		ZeroMemory(&dd, sizeof(dd));
		dd.dwSize = sizeof(dd);
		{
			auto hResult = pDest->Lock(NULL, &dd, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_WRITEONLY, NULL);
			if (hResult != DD_OK)
			{
				printf(" !      pDest->Lock\n");
				printf("    [-] hResult = %#x\r\n", hResult);
				return hResult;
			}
		}
	}
	// Check Destination Format
	bool DestFormatIsBGRA = false;
	{
		DestFormatIsBGRA = (
			(dd.ddpfPixelFormat.dwRBitMask & 0x00FF0000) &&
			(dd.ddpfPixelFormat.dwGBitMask & 0x0000FF00) &&
			(dd.ddpfPixelFormat.dwBBitMask & 0x000000FF) &&
			(dd.ddpfPixelFormat.dwRGBAlphaBitMask & 0xFF000000));
		if (!DestFormatIsBGRA)
		{
			printf(" !      DestFormatIs<NOT>BGRA\n");
			printf("    > B: %#x\n", dd.ddpfPixelFormat.dwBBitMask);
			printf("    > G: %#x\n", dd.ddpfPixelFormat.dwGBitMask);
			printf("    > R: %#x\n", dd.ddpfPixelFormat.dwRBitMask);
			printf("    > A: %#x\n", dd.ddpfPixelFormat.dwRGBAlphaBitMask);
			pDest->Unlock(NULL);
			return DD_FALSE;
		}
	}
	// Map Source
	DXGI_MAPPED_RECT ms = { 0 };
	{
		ZeroMemory(&ms, sizeof(DXGI_MAPPED_RECT));
		{
			auto hResult = pSrc->Map(&ms, D3D11_MAP_READ);
			if (hResult != DD_OK)
			{
				printf(" !      pSrc->Map\n");
				printf("    [-] hResult = %#x\r\n", hResult);
				pDest->Unlock(NULL);
				return hResult;
			}
		}
	}
	// Blit
	{
		const auto cpixelbytes = 4u;
		auto src_ptr = ms.pBits;
		auto dest_ptr = (BYTE*)dd.lpSurface;
		for (auto h = 0u; h < min(dd.dwHeight, ds.Height); h++)
		{
			auto w = 0u;
			for (; w < min(dd.dwWidth, ds.Width); w++)
			{
				dest_ptr[0] = src_ptr[2];
				dest_ptr[1] = src_ptr[1];
				dest_ptr[2] = src_ptr[0];
				dest_ptr[3] = src_ptr[3];
				//
				src_ptr += cpixelbytes;
				dest_ptr += cpixelbytes;
			}
			dest_ptr = dest_ptr - (w * cpixelbytes) + dd.lPitch;
			src_ptr = src_ptr - (w * cpixelbytes) + ms.Pitch;
		}
	}
	// Unmap Source
	{
		auto hResult = pSrc->Unmap();
		if (hResult != DD_OK)
		{
			printf(" !      pSrc->Unmap\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			pDest->Unlock(NULL);
			return hResult;
		}
	}
	// Unlock Destination
	{
		auto hResult = pDest->Unlock(NULL);
		if (hResult != DD_OK)
		{
			printf(" !      pDest->Unlock\n");
			printf("    [-] hResult = %#x\r\n", hResult);
			
			return hResult;
		}
	}
	return S_OK;
}

auto duration = 0ull;

namespace GLOBALS
{
	DD_Overlay*					pDD_Overlay = nullptr;
	DX11_BackgroundRenderer*	pDX11_BackgroundRenderer = nullptr;
	ID2D1_Renderer*				pID2D1_Renderer = nullptr;
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

int main()
{
	unsigned uZoom = 120;

	/*while (true)
	{
		printf("------------------------------------------------------" "\n");
		{
			auto pParam = queueCmd(eCmdID::ECHO, (unsigned char*)"test", 5);
			auto oParam = getFuture(pParam).get();
			std::cout << " > " << "RPLpBuf: " << (char*)oParam.ui64RPLpBuf << std::endl;
			std::cout << " > " << "RPLsize:  " << oParam.ui64RPLsize << std::endl;
			freeCmd(pParam);
		}
		printf("------------------------------------------------------" "\n");
		{
			auto pParam = queueCmd(eCmdID::PING, (unsigned char*)"ping", 5);
			auto oParam = getFuture(pParam).get();
			std::cout << " > " << "RPLpBuf: " << (char*)oParam.ui64RPLpBuf << std::endl;
			std::cout << " > " << "RPLsize:  " << oParam.ui64RPLsize << std::endl;
			freeCmd(pParam);
		}
		system("pause");
	}*/

	unsigned uWidth = 16 * uZoom;
	unsigned uHeight = 9 * uZoom;

	printf(" > Init...\n");

	system("pause");

	//HWND hWnd = GetConsoleWindow();
	HWND hWnd = CreateRenderWindow(uWidth, uHeight);

	GLOBALS::pDD_Overlay = new DD_Overlay(uWidth, uHeight);
	GLOBALS::pDX11_BackgroundRenderer = new DX11_BackgroundRenderer(uWidth, uHeight, hWnd);
	GLOBALS::pID2D1_Renderer = new ID2D1_Renderer(uWidth, uHeight, GLOBALS::pDX11_BackgroundRenderer->GpDXGISurface());
	render_utils::render = new render_utils::c_render();
	input::input = new input::c_input();
	ui::ui = new ui::c_ui();

	printf("\n > Hide Render Preview Window\n");
	ShowWindow(hWnd, SW_HIDE);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO&io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
	//io.ConfigViewportsNoDefaultParent = true;
	//io.ConfigDockingAlwaysTabBar = true;
	//io.ConfigDockingTransparentPayload = true;
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!
	//io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; // FIXME-DPI: Experimental.
	io.DisplaySize.x = uWidth;
	io.DisplaySize.y = uHeight;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Renderer backend
	ImGui_ImplDX11_Init(GLOBALS::pDX11_BackgroundRenderer->GpDevice(), GLOBALS::pDX11_BackgroundRenderer->GpDeviceContext());

	printf("\n > Render Loop...\n");

	// enter the main loop:

	auto t1 = std::chrono::high_resolution_clock::now();

	MSG msg;
	while (TRUE)
	{
		auto t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

		// check for pending window close
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			printf("msg: 0x%x\n", msg.message);

			if (msg.message == WM_QUIT)
			{
				printf("\n > wmquit\n");
				break;
			}
		}
		// Render DirectX11 (Direct3D)
		{
			auto hResult = GLOBALS::pDX11_BackgroundRenderer->Draw([](ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView) -> HRESULT
				{

					// Start the Dear ImGui frame
					ImGui_ImplDX11_NewFrame();
					ImGui::NewFrame();

					{
						static float f = 0.0f;
						static int counter = 0;

						ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

						ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

						ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
						ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

						if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
							counter++;
						ImGui::SameLine();
						ImGui::Text("counter = %d", counter);

						ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
						ImGui::End();
					}

					// Rendering
					ImGui::Render();
					
					pContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
					//const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
					//pContext->ClearRenderTargetView(pRenderTargetView, clear_color_with_alpha);
					ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

					return S_OK;
				}
			);
			if (hResult != S_OK)
			{
				printf(" !      pDX11_BackgroundRenderer->Draw\n");
				printf("    [-] hResult = %#x \n", hResult);
				//return hResult;
			}
		}
		// Render Direct2D
		{
			auto hResult = GLOBALS::pID2D1_Renderer->Draw([](ID2D1_Renderer* pInst, ID2D1RenderTarget* pRenderTarget) -> HRESULT
				{
					/*
					pRenderTarget->BeginDraw();
					pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

					pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black, 0.F)); //Transparent
					auto fps = duration ? 1000000ull / duration : 9999999ull;

					if (render_utils::render->was_setup == false) render_utils::render->setup(pInst->m_pDirect2dFactory, pRenderTarget, pInst->dwrite_factory);
					else
					{
						render_utils::render->text("global-draw-overlay~", 0.6f, 0.6f, RGBA({ 0, 255, 0, 255 }), "Consolas0.8");
						render_utils::render->text(std::string("overlay fps:").append(std::to_string((int)fps)), 0.6f, 2.4f, RGBA({ 255, 0, 0, 255 }), "Consolas0.8");
						//
						//render_utils::render->line(0.6f, 4.f, 8.f, 4.f, 0.15f, RGBA({0, 255, 0, 255}));
						////
						//render_utils::render->orect(0.6f, 5.f, 5.f, 5.f, 0.15f, RGBA({ 255,0,0,255 }));
						////
						//render_utils::render->frect(6.6f, 5.f, 5.f, 5.f, RGBA({ 0,255,0,255 }));
						////
						//render_utils::render->orrect(0.6f, 11.f, 5.f, 5.f, 5.f, 0.15f, RGBA({ 255,0,0,255 }));
						////
						//render_utils::render->frrect(6.6f, 11.f, 5.f, 5.f, 5.f, RGBA({ 0,255,0,255 }));
						ui::ui->work();
					}
					return pRenderTarget->EndDraw();
					*/
					return S_OK;
				}
			);
			if (hResult != DD_OK)
			{
				printf(" !      pID2D1_Renderer->Draw\n");
				printf("    [-] hResult = %#x \n", hResult);
				return hResult;
			}
		}
		// Render DirectDraw
		{
			auto hResult = GLOBALS::pDD_Overlay->Draw([](IDirectDraw7* pDevice, LPDIRECTDRAWSURFACE7 pSurface) -> HRESULT
				{
					// Render 2D Texture of 3D Space
					D3D11_TEXTURE2D_DESC desc;
					ID3D11Texture2D* pTexture = nullptr;
					{
						auto hResult = GLOBALS::pDX11_BackgroundRenderer->CaptureTexture(desc, pTexture);
						if (hResult != S_OK)
						{
							printf(" !      pDX11_BackgroundRenderer->CaptureTexture\n");
							printf("    [-] hResult = %#x \n", hResult);
							return hResult;
						}
					}
					// Query Surface
					IDXGISurface* pTextureSurface = nullptr;
					{
						auto hResult = pTexture->QueryInterface<IDXGISurface>(&pTextureSurface);
						if (hResult != S_OK)
						{
							printf(" !      pTexture->QueryInterface<IDXGISurface>(&pTextureSurface)\n");
							printf("    [-] hResult = %#x\r\n", hResult);
							return hResult;
						}
					}
					// BitBlit
					{
						auto hResult = Blt_DXGISurface_To_DIRECTDRAWSURFACE7(pDevice, pTextureSurface, pSurface);
						if (hResult != S_OK)
						{
							printf(" !      Blt_DXGISurface_To_DIRECTDRAWSURFACE7\n");
							printf("    [-] hResult = %#x\r\n", hResult);
							return hResult;
						}
					}
					// Release Surface
					{
						pTextureSurface->Release();
					}
					// Release Texture
					{
						pTexture->Release();
					}
					return S_OK;
				}
			);
			if (hResult != S_OK)
			{
				printf(" !      pDD_Overlay->Draw\n");
				printf("    [-] hResult = %#x \n", hResult);
				//return hResult;
			}
		}
		// Present DirectX11 (Direct3D) [+Direct2D rendered on top of it]
		{
			//auto hResult = GLOBALS::pDX11_BackgroundRenderer->Present();
			//if (hResult != DD_OK)
			//{
			//	printf(" !      pDX11_BackgroundRenderer->Present\n");
			//	printf("    [-] hResult = %#x \n", hResult);
			//	//return hResult;
			//}
		}
		// Flip DirectDraw
		{
			auto hResult = GLOBALS::pDD_Overlay->Flip();
			if (hResult != DD_OK)
			{
				printf(" !      pDD_Overlay->Flip\n");
				printf("    [-] hResult = %#x \n", hResult);
				printf("\n");
			}
			if (hResult == DDERR_SURFACELOST)
			{
				hResult = GLOBALS::pDD_Overlay->Restore();
				if (hResult != DD_OK)
				{
					printf(" !      pOverlay->Restore\n");
					printf("    [-] hResult = %#x \n", hResult);
					printf("\n");
				}
				if (hResult == DDERR_WRONGMODE)
					break;
			}
		}
		// Update DirectDraw Overlay
		{
			auto hResult = GLOBALS::pDD_Overlay->UpdateOverlay();
			if (hResult != DD_OK)
			{
				printf(" !      pDD_Overlay->UpdateOverlay\n");
				printf("    [-] hResult = %#x \n", hResult);
				printf("\n");
			}
			if (hResult == DDERR_SURFACELOST)
			{
				hResult = GLOBALS::pDD_Overlay->Restore();
				if (hResult != DD_OK)
				{
					printf(" !      pOverlay->Restore\n");
					printf("    [-] hResult = %#x \n", hResult);
					printf("\n");
				}
				if (hResult == DDERR_WRONGMODE)
					break;
			}
		}

		t1 = t2;
	}

	delete GLOBALS::pID2D1_Renderer;
	delete GLOBALS::pDX11_BackgroundRenderer;
	delete GLOBALS::pDD_Overlay;

	printf("\n");
	printf(" > Quitting...\n");

	system("pause");

	return 0;
}