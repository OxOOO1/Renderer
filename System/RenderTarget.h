#pragma once
#include "thirdParty/HBAO+/GFSDK_SSAO.h"
#include "System/Rendering.h"
#include "Bindable/BindablesH.h"

class RenderTargetPlane
{
public:

	RenderTargetPlane(Rendering& renderer)
		: vertexShader{ renderer, "cso\\FullscreenTriangle_VS", VertexLayout{} },
		pixelShader{ renderer, "cso\\FullscreenTriangle_PS" },
		pixelShaderLightFilter{ renderer, "cso\\RTLightFilterPass" }
	{

	}

	void Draw(Rendering& renderer, bool filterHDRLight = false) noexcept
	{
		Blender::Get(renderer, false).Bind(renderer);
		Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).Bind(renderer);
		Sampler::Get(renderer).Bind(renderer);

		vertexShader.Bind(renderer);

		if (!filterHDRLight)
			pixelShader.Bind(renderer);
		else
			pixelShaderLightFilter.Bind(renderer);

		renderer.GetContext()->Draw(3, 0);
	}

	void DrawGeometry(Rendering& renderer)
	{
		Blender::Get(renderer, false).Bind(renderer);
		Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).Bind(renderer);
		//Sampler::Get(renderer).Bind(renderer);

		vertexShader.Bind(renderer);

		renderer.GetContext()->Draw(3, 0);
	}

	void DrawQuad(Rendering& renderer)
	{
		Blender::Get(renderer, false).Bind(renderer);
		Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST).Bind(renderer);
		Sampler::Get(renderer).Bind(renderer);

		//vertexShader.Bind(renderer);

		renderer.GetContext()->Draw(6, 0);
	}


private:
	VertexShader vertexShader;
	PixelShader pixelShader;
	PixelShader pixelShaderLightFilter;

};

class RenderTarget
{
public:

	RenderTarget() = default;

	RenderTarget(Rendering& renderer, UINT width, UINT height, DXGI_FORMAT format, UINT flags);

	void Create(Rendering& renderer, UINT width, UINT height, DXGI_FORMAT format, UINT flags);

	void Bind(Rendering& renderer)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), renderer.GetDepthStencilView());
	}

	void BindClear(Rendering& renderer)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		const float c[4] = { 0.0f, 0.0f, 0.f, 1.f };
		renderer.GetContext()->ClearRenderTargetView(renderTargetView.Get(), c);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), renderer.GetDepthStencilView());
	}

	void BindClearNoDepth(Rendering& renderer)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		const float c[4] = { 0.0f, 0.0f, 0.f, 1.f };
		renderer.GetContext()->ClearRenderTargetView(renderTargetView.Get(), c);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), nullptr);
	}

	void BindWithoutDepthStencil(Rendering& renderer)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetView.GetAddressOf(), nullptr);
	}

	void Draw(Rendering& renderer, bool HDRLightOnly = false)
	{
		SetAsPixelShaderResourceView(renderer);
		GetDrawable(renderer).Draw(renderer, HDRLightOnly);
		SetDefaultShaderResourceForPS();
	}

	static void DrawPlane(Rendering& renderer, bool fullQuad = false)
	{
		if(!fullQuad)
		GetDrawable(renderer).DrawGeometry(renderer);
		else
			GetDrawable(renderer).DrawQuad(renderer);
	}

	static void BindDefaultRenderTarget()
	{
		Rendering::GetCurrent().BindDefaultRenderTarget();
	}

	static void SetDefaultShaderResourceForPS()
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV[5] = { nullptr };
		Rendering::GetCurrent().GetContext()->PSSetShaderResources(0, 5, pSRV->GetAddressOf());
	}

	static void SetDefaultShaderResourceForCS()
	{
		Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> pUAV[1] = { nullptr };
		Rendering::GetCurrent().GetContext()->CSSetUnorderedAccessViews(0u, 1, pUAV->GetAddressOf(), nullptr);
	}

	static void SetDefaultUAVForCS()
	{
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pSRV[1] = { nullptr };
		Rendering::GetCurrent().GetContext()->CSSetShaderResources(0, 1, pSRV->GetAddressOf());
	}

	void SetAsPixelShaderResourceView(Rendering& renderer, UINT slot = 0u) const
	{
		renderer.GetContext()->PSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
	}

	void SetAsCompShaderResource(Rendering& renderer)
	{
		renderer.GetContext()->CSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
	}

	void SetAsCompShaderUAV(Rendering& renderer)
	{
		renderer.GetContext()->CSSetUnorderedAccessViews(0, 1, unorderedAccessView.GetAddressOf(), 0);
	}

	ID3D11RenderTargetView* GetView() const
	{
		return renderTargetView.Get();
	}

	static RenderTargetPlane& GetDrawable(Rendering& renderer)
	{
		static RenderTargetPlane p(renderer);
		return p;
	}

	 UINT width;
	 UINT height;
	 DXGI_FORMAT format;

protected:

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

	D3D11_VIEWPORT viewport;

};

class RenderTargetArray
{

public:

	RenderTargetArray(Rendering& renderer, UINT arraySize, UINT width, UINT height, DXGI_FORMAT format, UINT flags);

	void Bind(Rendering& renderer, UINT index)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetViews.at(index).GetAddressOf(), renderer.GetDepthStencilView());
	}

	void BindClearNoDepth(Rendering& renderer, UINT index)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		const float c[4] = { 0.f, 0.f, 0.f, 1.f };
		renderer.GetContext()->ClearRenderTargetView(renderTargetViews.at(index).Get(), c);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetViews.at(index).GetAddressOf(), nullptr);
	}

	void BindWithoutDepthStencil(Rendering& renderer, UINT index)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		renderer.GetContext()->OMSetRenderTargets(1u, renderTargetViews.at(index).GetAddressOf(), nullptr);
	}

	void SetAsPixelShaderResourceView(Rendering& renderer, UINT slot)
	{
		renderer.GetContext()->PSSetShaderResources(slot, 1, shaderResourceViews.GetAddressOf());
	}

	void SetAsCompShaderResource(Rendering& renderer)
	{
		renderer.GetContext()->CSSetShaderResources(0, 1, shaderResourceViews.GetAddressOf());
	}

	void SetAsCompShaderUAV(Rendering& renderer, UINT index)
	{
		renderer.GetContext()->CSSetUnorderedAccessViews(0, 1, unorderedAccessViews.GetAddressOf(), 0);
	}

	ID3D11RenderTargetView* GetView(UINT index) const
	{
		return renderTargetViews.at(index).Get();
	}

	UINT width;
	UINT height;
	DXGI_FORMAT format;

protected:

	std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> renderTargetViews;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unorderedAccessViews;

	D3D11_VIEWPORT viewport;


};
//
//class RenderTargetSSAO : public RenderTarget
//{
//public:
//
//	RenderTargetSSAO(DirectX::XMMATRIX projection, Rendering& renderer, UINT width, UINT height, DXGI_FORMAT format, UINT flags);
//
//	void Draw(Rendering& renderer)
//	{
//		status = pAOContext->RenderAO(renderer.GetContext().Get(), ssaoInput, aoParams, ssaoOut);
//		assert(status == GFSDK_SSAO_OK);
//	}
//	void SetInput(ID3D11RenderTargetView* view)
//	{
//		ssaoOut.pRenderTargetView = view;
//	}
//	void ShowControlWindow();
//
//private:
//
//	GFSDK_SSAO_Status status;
//	GFSDK_SSAO_Context_D3D11* pAOContext;
//	GFSDK_SSAO_InputData_D3D11 ssaoInput;
//	GFSDK_SSAO_Parameters aoParams;
//	GFSDK_SSAO_Output_D3D11 ssaoOut;
//};


class ShadowMap : public RenderTarget 
{
public:

	ShadowMap(Rendering& renderer, float width, float height, UINT flags = 0);

	void Bind(Rendering& renderer)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		RenderTarget::SetDefaultShaderResourceForPS();

		ID3D11RenderTargetView* nullRenderTarget[1] = { nullptr };

		renderer.GetContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
		renderer.GetContext()->OMSetRenderTargets(1, nullRenderTarget, depthStencilView.Get());

	}

private:
	
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	
};

class PointLightShadowMap
{
public:

	PointLightShadowMap(Rendering& renderer, float width, float height);

	void Bind(Rendering& renderer)
	{
		renderer.GetContext()->RSSetViewports(1u, &viewport);
		RenderTarget::SetDefaultShaderResourceForPS();
		ID3D11RenderTargetView* renderTargets[1] = { nullptr };
		renderer.GetContext()->OMSetRenderTargets(1, renderTargets, depthStencilView.Get());

		renderer.GetContext()->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	static void SetDefaultViewport()
	{
		Rendering::GetCurrent().UpdateViewport();
	}

	void Present(Rendering& renderer)
	{
		SetAsPixelShaderResourceView(renderer, 0u);
		RenderTarget::GetDrawable(renderer).Draw(renderer);
		RenderTarget::SetDefaultShaderResourceForPS();
	}

	void SetAsPixelShaderResourceView(Rendering& renderer, UINT slot)
	{
		renderer.GetContext()->PSSetShaderResources(slot, 1, depthTextureResourceView.GetAddressOf());
	}

private:

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthTextureResourceView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;

	D3D11_VIEWPORT viewport;


};
