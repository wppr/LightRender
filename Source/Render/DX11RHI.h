#pragma once
// Render Hardware Interface 
#include "RenderDef.h"

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <DirectXMath.h>


#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }
#define SafeDelete(x) { delete x; x = 0; }

class DX11RHI{
public:
	HRESULT Init();


	HRESULT InitDevice(HWND wnd);
	void Render();
	void SwapBuffer();
	//--------------------------------------------------------------------------------------
	// Clean up the objects we've created
	//--------------------------------------------------------------------------------------
	void CleanupDevice();

private:
	HINSTANCE               g_hInst = nullptr;
	HWND                    g_hWnd = nullptr;
	D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11Device*           g_pd3dDevice = nullptr;
	ID3D11Device1*          g_pd3dDevice1 = nullptr;
	ID3D11DeviceContext*    g_pImmediateContext = nullptr;
	ID3D11DeviceContext1*   g_pImmediateContext1 = nullptr;
	IDXGISwapChain*         g_pSwapChain = nullptr;
	IDXGISwapChain1*        g_pSwapChain1 = nullptr;
	ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
};