//#include "DX11RHI.h"
//
//
//
//HRESULT DX11RHI::Init()
//{
//	UINT createDeviceFlags = 0;
//#if defined(DEBUG) || defined(_DEBUG)  
//	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif
//
//	D3D_FEATURE_LEVEL featureLevel;
//	HRESULT hr = D3D11CreateDevice(
//		0,                 // default adapter
//		md3dDriverType,
//		0,                 // no software device
//		createDeviceFlags,
//		0, 0,              // default feature level array
//		D3D11_SDK_VERSION,
//		&md3dDevice,
//		&featureLevel,
//		&md3dImmediateContext);
//
//	if (FAILED(hr))
//	{
//		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
//		return false;
//	}
//
//	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
//	{
//		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
//		return false;
//	}
//
//	// Check 4X MSAA quality support for our back buffer format.
//	// All Direct3D 11 capable devices support 4X MSAA for all render 
//	// target formats, so we only need to check quality support.
//
//	HR(md3dDevice->CheckMultisampleQualityLevels(
//		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
//	assert(m4xMsaaQuality > 0);
//
//	// Fill out a DXGI_SWAP_CHAIN_DESC to describe our swap chain.
//
//	DXGI_SWAP_CHAIN_DESC sd;
//	sd.BufferDesc.Width = mClientWidth;
//	sd.BufferDesc.Height = mClientHeight;
//	sd.BufferDesc.RefreshRate.Numerator = 60;
//	sd.BufferDesc.RefreshRate.Denominator = 1;
//	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//
//	// Use 4X MSAA? 
//	if (mEnable4xMsaa)
//	{
//		sd.SampleDesc.Count = 4;
//		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
//	}
//	// No MSAA
//	else
//	{
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//	}
//
//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	sd.BufferCount = 1;
//	sd.OutputWindow = mhMainWnd;
//	sd.Windowed = true;
//	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//	sd.Flags = 0;
//
//	// To correctly create the swap chain, we must use the IDXGIFactory that was
//	// used to create the device.  If we tried to use a different IDXGIFactory instance
//	// (by calling CreateDXGIFactory), we get an error: "IDXGIFactory::CreateSwapChain: 
//	// This function is being called with a device from a different IDXGIFactory."
//
//	IDXGIDevice* dxgiDevice = 0;
//	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
//
//	IDXGIAdapter* dxgiAdapter = 0;
//	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));
//
//	IDXGIFactory* dxgiFactory = 0;
//	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
//
//	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));
//
//	ReleaseCOM(dxgiDevice);
//	ReleaseCOM(dxgiAdapter);
//	ReleaseCOM(dxgiFactory);
//}
//
//void DX11RHI::Render()
//{
//	//g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, Colors::MidnightBlue);
//}
