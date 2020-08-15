// DirectDrawFun.cpp

#include <inc.hpp>

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
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

auto duration = 0ull;

int main()
{
	unsigned uZoom = 60;
	unsigned uWidth = 16 * uZoom;
	unsigned uHeight = 9 * uZoom;

	printf(" > Init...\n");

	//HWND hWnd = GetConsoleWindow();
	HWND hWnd = CreateRenderWindow(uWidth, uHeight);
	auto pDX11_Renderer = new DX11_Renderer(uWidth, uHeight, hWnd);
	auto pDD_Overlay = new DD_Overlay(uWidth, uHeight);

	printf("\n > Hide Render Preview Window\n");
	//ShowWindow(hWnd, SW_HIDE);
	ShowWindow(hWnd, SW_MINIMIZE);

	printf("\n > Render Loop...\n");

	// enter the main loop:

	auto t1 = std::chrono::high_resolution_clock::now();

	MSG msg;
	while (TRUE)
	{
		auto t2 = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
		
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				printf("\n > wmquit\n");
				break;
			}
		}
		// Render 3D
		{
			pDX11_Renderer->GetDX11_BackgroundRenderer()->RenderFrame([](ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D11RenderTargetView* pRenderTargetView) -> void
				{

					// Get's overwritten by 2D renderer in current implementation

					// clear the back buffer to red
					//color[0] = 255.f;
					//pContext->ClearRenderTargetView(pRenderTargetView, color);

				}
			);
		}
		// Render 2D
		{
			auto hResult = pDX11_Renderer->GetID2D1_Renderer()->Update([](ID2D1_Renderer* pInst, ID2D1RenderTarget* pRenderTarget) -> HRESULT
				{
					pRenderTarget->BeginDraw();
					pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

					pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black)); //Transparent

					auto rect = D2D1::RectF(50,50,100,100);
					pRenderTarget->FillRectangle(rect, pInst->GetBrush(RGB(0, 255, 0)));

					// 1000000 microseconds = 1 second
					auto fps = duration ? 1000000ull / duration : 9999999ull;
					printf("FPS: %llu | %llu\n", fps, duration);

					return pRenderTarget->EndDraw();
				}
			);
			if (hResult != DD_OK)
			{
				printf(" !      pDX11_Renderer->Update2D\n");
				printf("    [-] hResult = %#x \n", hResult);
				return hResult;
			}
		}
		// Blit to DirectDraw (Render Overlay)
		{
			auto hResult = pDD_Overlay->Update(pDX11_Renderer->GetDX11_BackgroundRenderer());
			if (hResult != DD_OK)
			{
				printf(" !      pOverlay->Update\n");
				printf("    [-] hResult = %#x \n", hResult);
				printf("\n");
			}
			if (hResult == DDERR_SURFACELOST)
			{
				hResult = pDD_Overlay->Restore();
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
		// Render Preview 
		{
			pDX11_Renderer->GetDX11_BackgroundRenderer()->Flip();
		}

		t1 = t2;
	}

	delete pDD_Overlay;
	delete pDX11_Renderer;

	printf("\n");
	printf(" > Quitting...\n");

	system("pause");

	return 0;
}