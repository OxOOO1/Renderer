#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <wrl.h>
#include <array>
#include <DirectXMath.h>
#include <mutex>
#include "Exception.h"

class Camera;
class Window;

class Rendering
{
public:

	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Rendering(Window& window);
	Rendering() = delete;
	Rendering(const Rendering&) = delete;
	Rendering& operator=(const Rendering&) = delete;
	Rendering(const Rendering&&) = delete;
	Rendering& operator=(const Rendering&&) = delete;

	~Rendering();

	inline ComPtr<ID3D11Device>& GetDevice() { return pDevice; }
	inline ComPtr<ID3D11DeviceContext>& GetContext() { return pContext; }
	void ThrowIfFailed(HRESULT hr);
	
	inline std::pair<int, int> GetViewportSize() { return { viewportWidth, viewportHeight }; }

	void OnWindowResize(int width, int height);
	void OnResolutionChange(unsigned int width, unsigned int height);
	void UpdateViewport();
	void ShowTextureWindow();
	void Begin(float r = 0.1f, float g = 0.1f, float b = 0.1f);
	void End();
	void SwapBuffers();
	void ClearGBuffer(float r, float g, float b) noexcept;
	void DrawIndexed(UINT count, UINT indexIndex, UINT vertexIndex) noexcept;
	
	void ShowSettingsWindow();

	ID3D11DepthStencilView* GetDepthStencilView()
	{
		return pDepthStencilView.Get();
	}
	void BindDefaultRenderTarget()
	{
		pContext->OMSetRenderTargets(1u, pRenderTarget.GetAddressOf(), pDepthStencilView.Get());
	}
	void BindDefaultRenderTargetAndViewport()
	{
		pContext->OMSetRenderTargets(1u, pRenderTarget.GetAddressOf(), pDepthStencilView.Get());
		pContext->RSSetViewports(1u, &viewport);
	}
	void BindDefaultRenderTargetAndViewportOnly()
	{
		pContext->OMSetRenderTargets(1u, pRenderTarget.GetAddressOf(), nullptr);
		pContext->RSSetViewports(1u, &viewport);
	}
	void BindDefaultViewport()
	{
		pContext->RSSetViewports(1u, &viewport);
	}
	void BindNullRenderTarget()
	{
		ID3D11RenderTargetView* nullRenderTarget[1] = { nullptr };
		pContext->OMSetRenderTargets(1, nullRenderTarget, pDepthStencilView.Get());
	}
	void ClearDepth()
	{
		pContext->ClearDepthStencilView(pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
	}
	void BindDepthOnly()
	{
		ID3D11RenderTargetView* nullRenderTarget[1] = { nullptr };
		pContext->OMSetRenderTargets(1, nullRenderTarget, pDepthStencilView.Get());
	}
	void SetDepthAsPixelShaderResourceView(UINT slot)
	{
		pContext->PSSetShaderResources(slot, 1, pDepthShaderResourceView.GetAddressOf());
	}
	void SetStencilAsPixelShaderResourceView(UINT slot)
	{
		pContext->PSSetShaderResources(slot, 1, pStencilShaderResourceView.GetAddressOf());
	}
	ID3D11ShaderResourceView* GetDepthSRV()
	{
		return pDepthShaderResourceView.Get();
	}

	void GoFullScreen();

private:
	void CreateRenderTargetDepthStencil();
	void CreateDeviceAndSwapChain(HWND hwnd);
	void ResetSwapChainAndRenderTargets();
private:
	ComPtr<ID3D11Device> pDevice;
	ComPtr<IDXGISwapChain> pSwapChain;
	ComPtr<ID3D11Texture2D> pBackBufferTexture;
	ComPtr<ID3D11RenderTargetView> pBackBufferRenderTarget;
	ComPtr<ID3D11DeviceContext> pContext;
	ComPtr<ID3D11Texture2D> pMSAARenderTargetTexture;
	ComPtr<ID3D11RenderTargetView> pRenderTargetMSAA;
	ComPtr<ID3D11DepthStencilView> pDepthStencilView;
	ComPtr<ID3D11ShaderResourceView> pDepthShaderResourceView;
	ComPtr<ID3D11ShaderResourceView> pStencilShaderResourceView;

	ComPtr<ID3D11RenderTargetView>& pRenderTarget{ pBackBufferRenderTarget };

	D3D11_VIEWPORT viewport;

public:
	int viewportWidth, viewportHeight;
	UINT MSAAQuality = 0;
	bool bMsaaEnabled = false;
	bool bVSyncEnabled = false;
	bool bWindowedMode = true;

	HWND winHandle;

	static Rendering& GetCurrent()
	{
		return *pCurrent;
	}
	static Rendering* pCurrent;

};


