#include "Rendering.h"
#include "Window.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
#include <d3dcompiler.h>
#include <wincodec.h>

#include "src/thirdParty/ImGUI/imgui.h"
#include "src/thirdParty/ImGUI/imgui_impl_dx11.h"
#include "src/thirdParty/ImGUI/imgui_impl_win32.h"
#include "src/thirdParty/ScreenGrab/ScreenGrab.h"

#include "Utilities/Console.h"
#include "Scene/Camera.h"
#include "Utilities/DebugDraw.h"

namespace dx = DirectX;

constexpr DXGI_FORMAT sampleFormat = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

Rendering* Rendering::pCurrent = nullptr;

Rendering::Rendering(Window& window)
	:viewportWidth(window.GetSize().first),
	viewportHeight(window.GetSize().second)
{
	pCurrent = this;

	winHandle = window.GetHandle();

	CreateDeviceAndSwapChain(winHandle);
	
	CreateRenderTargetDepthStencil();
	
	
	viewport.Width = (float)viewportWidth;
	viewport.Height = (float)viewportHeight;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	pContext->RSSetViewports(1u, &viewport);

	ImGui_ImplDX11_Init(pDevice.Get(), pContext.Get());

}

void Rendering::OnWindowResize(int width, int height)
{
	//if (bWindowedMode)
	//{
	//	renderWidth = width; renderHeight = height;
	//}

	//pRenderTarget.Reset();
	//pDepthStencilView.Reset();

	//pContext->OMSetRenderTargets(0, 0, 0);

	////Preserves existing buffer's count and format, autos size from client's window 
	////ThrowIfFailed(pSwapChain->ResizeBuffers(2, renderWidth, renderHeight, DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	//ThrowIfFailed(pSwapChain->ResizeBuffers(0, renderWidth, renderHeight, sampleFormat, 0));
	//CreateRenderTargetDepthStencil();

	//D3D11_VIEWPORT vp;
	//vp.Width = (float)renderWidth;
	//vp.Height = (float)renderHeight;
	//vp.MinDepth = 0;
	//vp.MaxDepth = 1;
	//vp.TopLeftX = 0;
	//vp.TopLeftY = 0;
	//pContext->RSSetViewports(1u, &vp);

	//mainCamera->SetProjection(true, (float)renderWidth, (float)renderHeight, 0.1f, 10000.f);

}


Rendering::~Rendering()
{
	ImGui_ImplDX11_Shutdown();
	pSwapChain->GetFullscreenState(false, nullptr);
}

void Rendering::ResetSwapChainAndRenderTargets()
{
	
}

void Rendering::OnResolutionChange(unsigned int width, unsigned int height)
{
	if (bWindowedMode)
	{
		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferDesc.Width = width;
		sd.BufferDesc.Height = height;
		sd.BufferDesc.Format = sampleFormat;// _SRGB;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferDesc.Scaling = /*DXGI_MODE_SCALING_UNSPECIFIED;*/ DXGI_MODE_SCALING_CENTERED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = winHandle;
		sd.Windowed = bWindowedMode;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		pSwapChain->ResizeTarget(&sd.BufferDesc);
	}
	else
	{

		//TODO//DXGI_NO_WINDOW_CHANGES
		viewportWidth = width; viewportHeight = height;

		//pRenderTarget.Reset();
		pDepthStencilView.Reset();

		pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		CreateRenderTargetDepthStencil();

		D3D11_VIEWPORT vp;
		vp.Width = (float)viewportWidth;
		vp.Height = (float)viewportHeight;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pContext->RSSetViewports(1u, &vp);
	}


}

void Rendering::UpdateViewport()
{
	D3D11_VIEWPORT vp;
	vp.Width = (float)viewportWidth;
	vp.Height = (float)viewportHeight;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	pContext->RSSetViewports(1u, &vp);
}


void Rendering::GoFullScreen()
{
	bWindowedMode = false;
/*
	auto width = 2560;
	auto height = 1440;
*/
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = viewportWidth;
	sd.BufferDesc.Height = viewportHeight;

	sd.BufferDesc.Format = sampleFormat;// _SRGB;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//sd.BufferCount = 2;
	sd.BufferCount = 1;
	sd.OutputWindow = winHandle;
	sd.Windowed = bWindowedMode;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//pSwapChain->ResizeTarget(&sd.BufferDesc);

	ThrowIfFailed(pSwapChain->SetFullscreenState(true, nullptr));

	pSwapChain->ResizeTarget(&sd.BufferDesc);

	//OnWindowResize(width, height);
}


void Rendering::ShowTextureWindow()
{
	if (ImGui::Begin("Screen Capture"))
	{
		
		if (ImGui::Button("Save Image")) {
			ComPtr<ID3D11Texture2D> bufferTex;
			ThrowIfFailed( pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&bufferTex));
			
			DirectX::SaveWICTextureToFile(pContext.Get(), bufferTex.Get(), GUID_ContainerFormatPng, L"XXX222AAA.PNG");
			

		}
	}
	ImGui::End();
}

void Rendering::Begin(float r, float g, float b)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	ClearGBuffer(r, g, b);
	ClearDepth();

}

void Rendering::End()
{
	DebugDraw::DrawBatch();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (bMsaaEnabled)
	{
		auto subIndex = D3D11CalcSubresource(0, 0, 1);
		pContext->ResolveSubresource(pBackBufferTexture.Get(), subIndex, pMSAARenderTargetTexture.Get(), subIndex, sampleFormat);
	}
	
	SwapBuffers();
}

void Rendering::SwapBuffers()
{

	HRESULT hr;
	if (FAILED(hr = pSwapChain->Present((UINT)bVSyncEnabled, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
		{
			throw WND_EXCEPT(pDevice->GetDeviceRemovedReason());
		}
		else
		{
			throw WND_EXCEPT(hr);
		}
	}

	//pContext->OMSetRenderTargets(1u, pRenderTarget.GetAddressOf(), pDepthStencilView.Get());

}

void Rendering::ClearGBuffer(float r, float g, float b) noexcept
{
	const float c[4] = { r,g,b, 1.f };
	pContext->ClearRenderTargetView(pBackBufferRenderTarget.Get(), c);
}

void Rendering::DrawIndexed(UINT count, UINT indexIndex, UINT vertexIndex) noexcept
{
	pContext->DrawIndexed(count, indexIndex, vertexIndex);
}

void Rendering::ShowSettingsWindow()
{
	if (ImGui::Begin("Settings")) {
		const char* resolutions[] = { "720p", "1080p" };
		std::array<std::pair<int, int>, IM_ARRAYSIZE(resolutions)> reso_int = { std::make_pair(1280, 720), std::make_pair(1920,1080) };
		static const char* reso_current = resolutions[1];            // Here our selection is a single pointer stored outside the object.
		if (ImGui::BeginCombo("Screen Resolution", reso_current, 0)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < IM_ARRAYSIZE(resolutions); n++)
			{
				bool is_selected = (reso_current == resolutions[n]);
				if (ImGui::Selectable(resolutions[n], is_selected))
				{
					if (reso_current != resolutions[n])
					{
						reso_current = resolutions[n];
						auto[w, h] = reso_int.at(n);
						OnResolutionChange(w, h);
					}
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
			}
			ImGui::EndCombo();
		}

		if (ImGui::Checkbox("MSAA", &bMsaaEnabled))
		{
			CreateRenderTargetDepthStencil();
		}
		if (ImGui::Button("Fullscreen"))
		{
			GoFullScreen();
		}
		ImGui::Checkbox("VSync", &bVSyncEnabled);
	}
	ImGui::End();
}

void Rendering::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw WND_EXCEPT(hr);
	}
}

void Rendering::CreateRenderTargetDepthStencil()
{

	ThrowIfFailed(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBufferTexture));

	ThrowIfFailed(pDevice->CreateRenderTargetView(pBackBufferTexture.Get(), nullptr, &pBackBufferRenderTarget));

	//if (bMsaaEnabled)
	//{
	//	D3D11_TEXTURE2D_DESC texDesc = {};
	//	texDesc.Width = viewportWidth;
	//	texDesc.Height = viewportHeight;
	//	texDesc.Format = sampleFormat;// _SRGB;
	//	texDesc.MipLevels = 1;
	//	texDesc.ArraySize = 1;
	//	texDesc.CPUAccessFlags = 0;
	//	texDesc.MiscFlags = 0;
	//	texDesc.Usage = D3D11_USAGE_DEFAULT;
	//	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	//	texDesc.SampleDesc.Count = 4;
	//	texDesc.SampleDesc.Quality = MSAAQuality - 1;

	//	ThrowIfFailed(pDevice->CreateTexture2D(&texDesc, nullptr, &pMSAARenderTargetTexture));

	//	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	//	rtvDesc.Format = texDesc.Format;
	//	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	//	rtvDesc.Texture2D.MipSlice = 0;
	//	ThrowIfFailed(pDevice->CreateRenderTargetView(pMSAARenderTargetTexture.Get(), &rtvDesc, &pRenderTargetMSAA));

	//	pRenderTarget = pRenderTargetMSAA;
	//}
	
	//Depth Stencil
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

	ComPtr<ID3D11DepthStencilState> pDSState;
	ThrowIfFailed(pDevice->CreateDepthStencilState(&dsDesc, &pDSState));

	pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	//Depth Stencil Texture
	ComPtr<ID3D11Texture2D> pDepthStencil;

	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = viewportWidth;
	descDepth.Height = viewportHeight;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	
	if (bMsaaEnabled)
	{
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = MSAAQuality - 1;
	}
	else {
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
	}

	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	ThrowIfFailed(pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

	//Create view of texture
	D3D11_DEPTH_STENCIL_VIEW_DESC descSV = {};
	descSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descSV.Texture2D.MipSlice = 0u;

	if(!bMsaaEnabled)
	descSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	else descSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	ThrowIfFailed(pDevice->CreateDepthStencilView(pDepthStencil.Get(), &descSV, &pDepthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ThrowIfFailed(pDevice->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, &pDepthShaderResourceView));

	
	srvDesc.Format = DXGI_FORMAT_X24_TYPELESS_G8_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ThrowIfFailed(pDevice->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, &pStencilShaderResourceView));

	//Bind Render Target
	pContext->OMSetRenderTargets(1u, pBackBufferRenderTarget.GetAddressOf(), pDepthStencilView.Get());
}

void Rendering::CreateDeviceAndSwapChain(HWND windowHandle)
{

	DWORD createDeviceFlags = 0;
#ifndef NDEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;

	ThrowIfFailed(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		createDeviceFlags,
		nullptr, 0,
		D3D11_SDK_VERSION,
		&pDevice,
		&featureLevel,
		&pContext));

	ThrowIfFailed(pDevice->CheckMultisampleQualityLevels(
		sampleFormat,
		4,
		&MSAAQuality
	));
	assert(MSAAQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = viewportWidth;
	sd.BufferDesc.Height = viewportHeight;
	sd.BufferDesc.Format = sampleFormat;// _SRGB;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	
	sd.Windowed = bWindowedMode;
	sd.OutputWindow = windowHandle;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	//sd.BufferCount = 2;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	//sd.Flags = 0;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//DXGI_SWAP_CHAIN_DESC1 desc1; desc1.

	ComPtr<IDXGIDevice> dxgiDevice = nullptr;
	ThrowIfFailed(pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
	ThrowIfFailed(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	ComPtr<IDXGIFactory> dxgiFactory = nullptr;
	ThrowIfFailed(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	ThrowIfFailed(dxgiFactory->CreateSwapChain(
		pDevice.Get(),
		&sd,
		&pSwapChain)
	);

}
