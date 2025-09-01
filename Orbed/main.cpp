////////////////////////////////////////////////////////////////////////////////
// main.cpp
////////////////////////////////////////////////////////////////////////////////

#define NOMINMAX
#include <d3d11.h>
#include <tchar.h>
#include "imgui\backends\imgui_impl_dx11.h"
#include "imgui\backends\imgui_impl_win32.h"
#include "imgui\imgui.h"
#include "orbed.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static ID3D11Device* sDevice;
static ID3D11DeviceContext* sDeviceContext;
static IDXGISwapChain* sSwapChain;
static ID3D11RenderTargetView* sRenderTargetView;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void CreateRenderTarget()
{
	ID3D11Texture2D* back;
	sSwapChain->GetBuffer(0, IID_PPV_ARGS(&back));
	sDevice->CreateRenderTargetView(back, nullptr, &sRenderTargetView);
	back->Release();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void ReleaseRenderTarget()
{
	if (sRenderTargetView != nullptr)
	{
		sRenderTargetView->Release();
		sRenderTargetView = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static bool CreateDeviceD3D(HWND hwnd)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {
		.BufferDesc = {
			.Width = 0,
			.Height = 0,
			.RefreshRate = {
				.Numerator = 60,
				.Denominator = 1,
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		},
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = hwnd,
		.Windowed = TRUE,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
	};

	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevelArray[] = {D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0};
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevelArray, countof(featureLevelArray), D3D11_SDK_VERSION, &swapChainDesc, &sSwapChain, &sDevice, &featureLevel, &sDeviceContext) != S_OK)
	{
		return false;
	}

	CreateRenderTarget();

	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static void ReleaseDeviceD3D()
{
	ReleaseRenderTarget();

	if (sSwapChain != nullptr)
	{
		sSwapChain->Release();
		sSwapChain = nullptr;
	}

	if (sDeviceContext != nullptr)
	{
		sDeviceContext->Release();
		sDeviceContext = nullptr;
	}

	if (sDevice != nullptr)
	{
		sDevice->Release();
		sDevice = nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
LRESULT WINAPI WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
	{
		return true;
	}

	switch (msg)
	{
		case WM_SIZE:
			if ((sDevice != nullptr) && (wparam != SIZE_MINIMIZED))
			{
				ReleaseRenderTarget();
				sSwapChain->ResizeBuffers(0, (UINT) LOWORD(lparam), (UINT) HIWORD(lparam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;

		case WM_SYSCOMMAND:
			if ((wparam & 0xfff0) == SC_KEYMENU)
			{
				// Disable ALT application menu.
				return 0;
			}
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
INT WINAPI WinMain(HINSTANCE hinst, HINSTANCE prevHinst, PSTR cmdLine, INT cmdShow)
{
	unused(hinst, prevHinst, cmdLine, cmdShow);

	WNDCLASSEX wndClass = {
		.cbSize = sizeof(WNDCLASSEX),
		.style = CS_CLASSDC,
		.lpfnWndProc = WndProc,
		.hInstance = GetModuleHandle(nullptr),
		.lpszClassName = _T("Orbed"),
	};
	RegisterClassEx(&wndClass);

	HWND hwnd = CreateWindow(wndClass.lpszClassName, _T("Orbed"), WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, nullptr, nullptr, wndClass.hInstance, nullptr);

	if (!CreateDeviceD3D(hwnd))
	{
		DestroyWindow(hwnd);
		UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

		return 1;
	}

	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(sDevice, sDeviceContext);

	if (!Orbed_Init())
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		ReleaseDeviceD3D();
		DestroyWindow(hwnd);
		UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

		return 2;
	}

	for (;;)
	{
		MSG msg;
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				break;
			}

			continue;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		RECT rect;
		GetClientRect(hwnd, &rect);
		ImGui::SetNextWindowPos({0.0f, 0.0f});
		ImGui::SetNextWindowSize({(float) (rect.right - rect.left), (float) (rect.bottom - rect.top)});

		ImGui::Begin("Orbed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);
		Orbed_Update();
		ImGui::End();

		ImGui::Render();

		sDeviceContext->OMSetRenderTargets(1, &sRenderTargetView, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		sSwapChain->Present(1, 0);
	}

	Orbed_Deinit();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	ReleaseDeviceD3D();
	DestroyWindow(hwnd);
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);

	return 0;
}
