#include "RenderTarget.h"

//#include "Drawable/Plane.h"


void RenderTarget::Create(Rendering & renderer, UINT width, UINT height, DXGI_FORMAT format, UINT flags) 
{

	this->width = width;
	this->height = height;
	this->format = format;

	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Format = format;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = flags;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&texDesc, nullptr, &texture));

	if (flags & D3D11_BIND_RENDER_TARGET)
	{

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, &renderTargetView));

		////////////////////// Depth Stencil /////////////////////

		/*D3D11_TEXTURE2D_DESC descDepth2 = {};
		descDepth2.Width = width;
		descDepth2.Height = height;
		descDepth2.MipLevels = 1u;
		descDepth2.ArraySize = 1u;
		descDepth2.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth2.SampleDesc.Count = 1;
		descDepth2.SampleDesc.Quality = 0;
		descDepth2.Usage = D3D11_USAGE_DEFAULT;
		descDepth2.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&descDepth2, nullptr, &depthTexture));

		D3D11_DEPTH_STENCIL_VIEW_DESC descSV2 = {};
		descSV2.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descSV2.Texture2D.MipSlice = 0u;
		descSV2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateDepthStencilView(depthTexture.Get(), &descSV2, &depthStencilView));*/
	}

	////////////////////// SRV /////////////////////

	if (flags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &shaderResourceView));
	}

	////////////////////// UAV /////////////////////

	if (flags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = texDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.Texture2D.MipSlice = 0;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateUnorderedAccessView(texture.Get(), &uavDesc, &unorderedAccessView));
	}
}


RenderTarget::RenderTarget(Rendering & renderer, UINT width, UINT height, DXGI_FORMAT format, UINT flags) : width(width), height(height), format(format)
{

	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.Format = format;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = flags;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&texDesc, nullptr, &texture));

	if (flags & D3D11_BIND_RENDER_TARGET)
	{

		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, &renderTargetView));

		////////////////////// Depth Stencil /////////////////////

		/*D3D11_TEXTURE2D_DESC descDepth2 = {};
		descDepth2.Width = width;
		descDepth2.Height = height;
		descDepth2.MipLevels = 1u;
		descDepth2.ArraySize = 1u;
		descDepth2.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth2.SampleDesc.Count = 1;
		descDepth2.SampleDesc.Quality = 0;
		descDepth2.Usage = D3D11_USAGE_DEFAULT;
		descDepth2.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&descDepth2, nullptr, &depthTexture));

		D3D11_DEPTH_STENCIL_VIEW_DESC descSV2 = {};
		descSV2.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descSV2.Texture2D.MipSlice = 0u;
		descSV2.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		renderer.ThrowIfFailed(renderer.GetDevice()->CreateDepthStencilView(depthTexture.Get(), &descSV2, &depthStencilView));*/
	}

	////////////////////// SRV /////////////////////

	if (flags & D3D11_BIND_SHADER_RESOURCE)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &shaderResourceView));
	}

	////////////////////// UAV /////////////////////

	if (flags & D3D11_BIND_UNORDERED_ACCESS)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = texDesc.Format;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.Texture2D.MipSlice = 0;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateUnorderedAccessView(texture.Get(), &uavDesc, &unorderedAccessView));
	}
}

RenderTargetArray::RenderTargetArray(Rendering& renderer, UINT arraySize, UINT width, UINT height, DXGI_FORMAT format, UINT flags) : width(width), height(height), format(format)
{

	
		
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.Width = width;
		texDesc.Height = height;
		texDesc.Format = format;
		texDesc.MipLevels = 1;
		texDesc.ArraySize = arraySize;
		texDesc.CPUAccessFlags = 0;
		texDesc.MiscFlags = 0;
		texDesc.Usage = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags = flags;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;

		renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&texDesc, nullptr, &texture));

		if (flags & D3D11_BIND_RENDER_TARGET)
		{

			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
			rtvDesc.Format = texDesc.Format;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.MipSlice = 0;
			rtvDesc.Texture2DArray.ArraySize = arraySize;

			for (int i = 0; i < arraySize; i++)
			{
				rtvDesc.Texture2DArray.FirstArraySlice = D3D11CalcSubresource(0, i, 0);

				renderTargetViews.emplace_back();
				renderer.ThrowIfFailed(renderer.GetDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, &renderTargetViews.back()));

			}

		}

		////////////////////// SRV /////////////////////

		if (flags & D3D11_BIND_SHADER_RESOURCE)
		{

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = texDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			srvDesc.Texture2DArray.ArraySize = arraySize;
			srvDesc.Texture2DArray.FirstArraySlice = 0;
			srvDesc.Texture2DArray.MipLevels = 1;
			srvDesc.Texture2DArray.MostDetailedMip = 0;


			renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, &shaderResourceViews));
		}

		////////////////////// UAV /////////////////////

		if (flags & D3D11_BIND_UNORDERED_ACCESS)
		{
			
			D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.Format = texDesc.Format;
			uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			uavDesc.Buffer.FirstElement = 0;
			uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
			uavDesc.Texture2D.MipSlice = 0;

			renderer.ThrowIfFailed(renderer.GetDevice()->CreateUnorderedAccessView(texture.Get(), &uavDesc, &unorderedAccessViews));
		}

	

}

ShadowMap::ShadowMap(Rendering & renderer, float width, float height, UINT flags) 
{
	this->width = width;
	this->height = height;

	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;

	D3D11_TEXTURE2D_DESC descDepth2 = {};
	descDepth2.Width = width;
	descDepth2.Height = height;
	descDepth2.MipLevels = 1u;
	descDepth2.ArraySize = 1u;
	descDepth2.Format = DXGI_FORMAT_R32_TYPELESS;

	descDepth2.SampleDesc.Count = 1;
	descDepth2.SampleDesc.Quality = 0;
	
	descDepth2.Usage = D3D11_USAGE_DEFAULT;
	descDepth2.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&descDepth2, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthStencil = {};
	descDepthStencil.Format = DXGI_FORMAT_D32_FLOAT;
	descDepthStencil.Texture2D.MipSlice = 0u;
	descDepthStencil.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &descDepthStencil, &depthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, &shaderResourceView));
}

PointLightShadowMap::PointLightShadowMap(Rendering & renderer, float width, float height)
{
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> pDepthStencil;

	D3D11_TEXTURE2D_DESC descDepth2 = {};
	descDepth2.Width = width;
	descDepth2.Height = height;
	descDepth2.MipLevels = 1u;
	descDepth2.ArraySize = 6u;
	descDepth2.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	descDepth2.Format = DXGI_FORMAT_R16_TYPELESS;

	/*if (renderer.bMsaaEnabled)
	{
		descDepth2.SampleDesc.Count = 4;
		descDepth2.SampleDesc.Quality = renderer.MSAAQuality - 1;
	}
	else {*/
		descDepth2.SampleDesc.Count = 1;
		descDepth2.SampleDesc.Quality = 0;
	//}

	descDepth2.Usage = D3D11_USAGE_DEFAULT;
	descDepth2.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&descDepth2, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC descDepthStencil = {};
	descDepthStencil.Format = DXGI_FORMAT_D16_UNORM;
	descDepthStencil.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	descDepthStencil.Texture2DArray.FirstArraySlice = 0;
	descDepthStencil.Texture2DArray.ArraySize = 6;
	descDepthStencil.Texture2DArray.MipSlice = 0;
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateDepthStencilView(pDepthStencil.Get(), &descDepthStencil, &depthStencilView));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R16_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	/*srvDesc.Texture2D.MostDetailedMip = -1;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.TextureCube.MipLevels = 0;*/
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(pDepthStencil.Get(), &srvDesc, &depthTextureResourceView));


}
//
//RenderTargetSSAO::RenderTargetSSAO(DirectX::XMMATRIX projection, Rendering & renderer, UINT width, UINT height, DXGI_FORMAT format, UINT flags)
//	:RenderTarget(renderer, width, height, format, flags)
//{
//
//	status = GFSDK_SSAO_CreateContext_D3D11(renderer.GetDevice().Get(), &pAOContext);
//	assert(status == GFSDK_SSAO_OK);
//
//	ssaoInput.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
//	ssaoInput.DepthData.pFullResDepthTextureSRV = renderer.GetDepthSRV();
//	ssaoInput.DepthData.ProjectionMatrix.Data = *reinterpret_cast<GFSDK_SSAO_Float4x4*>(&projection);
//	ssaoInput.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
//	ssaoInput.DepthData.MetersToViewSpaceUnits = 1.f;
//
//	aoParams.Radius = 25.0f;
//	aoParams.Bias = 0.1f;
//	aoParams.PowerExponent = 6.0f;
//	aoParams.Blur.Enable = true;
//	aoParams.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;//GFSDK_SSAO_BLUR_RADIUS_2
//	aoParams.Blur.Sharpness = 16.f;
//	aoParams.StepCount = GFSDK_SSAO_STEP_COUNT_4;
//
//	ssaoOut.pRenderTargetView = renderTargetView.Get();
//	ssaoOut.Blend.Mode = GFSDK_SSAO_MULTIPLY_RGB;//GFSDK_SSAO_OVERWRITE_RGB;
//}
//
//void RenderTargetSSAO::ShowControlWindow()
//{
//	if (ImGui::Begin("HBAO"))
//	{
//		ImGui::SliderFloat("Radius", &aoParams.Radius, 0.0f, 100.f);
//		ImGui::SliderFloat("Distance", &ssaoInput.DepthData.MetersToViewSpaceUnits, 1.f, 5.f);
//		ImGui::SliderFloat("Bias", &aoParams.Bias, 0.01f, 5.f);
//		ImGui::SliderFloat("Power", &aoParams.PowerExponent, 1.f, 25.f);
//		ImGui::SliderFloat("BlurSharpness", &aoParams.Blur.Sharpness, 0.f, 25.f);
//
//	}
//	ImGui::End();
//}
