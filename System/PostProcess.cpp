#include "PostProcess.h"

void PostProcess::BlurFilter::Process(Rendering & renderer, RenderTarget & target)
{
	for (int i = 0; i < numOfPasses; ++i)
	{
		computeShaderH.Bind(renderer);
		
		target.SetAsCompShaderResource(renderer);
		rt2.SetAsCompShaderUAV(renderer);

		renderer.GetContext()->Dispatch(target.width / 256 + 1, target.height, 1);

		RenderTarget::SetDefaultShaderResourceForCS();
		RenderTarget::SetDefaultUAVForCS();

		computeShaderV.Bind(renderer);

		rt2.SetAsCompShaderResource(renderer);
		target.SetAsCompShaderUAV(renderer);

		renderer.GetContext()->Dispatch(target.width, target.height / 256 + 1, 1);

		RenderTarget::SetDefaultShaderResourceForCS();
		RenderTarget::SetDefaultUAVForCS();
	}

	renderer.GetContext()->CSSetShader(nullptr, nullptr, 0u);

}


PostProcess::VolumeLight::VolumeLight(Rendering& renderer, GlobalLight& mGlobalLight)
	:
	psVolumeLight{ renderer, "cso\\VLVolumeLight_PS" },
	psMinMaxDepth1{ renderer, "cso\\VLMinMaxDepth_PS" },
	psMinMaxDepth2{ renderer, "cso\\VLMinMaxDepth_2_PS" },
	psMinMaxDepth3{ renderer, "cso\\VLMinMaxDepth_3_PS" },
	psPropagateMinDepth1{ renderer, "cso\\VLPropagateMinDepth_PS" },
	psPropagateMinDepth2{ renderer, "cso\\VLPropagateMinDepth_2_PS" },
	psPropagateMaxDepth{ renderer, "cso\\VLPropagateMaxDepth_PS" },
	psConvertCamDepthToWorld{ renderer, "cso\\VLConvertCamDepthToWorld_PS" },
	psConvertLightDepthToWorld{ renderer, "cso\\VLConvertLightDepthToWorld_PS" },
	samplerPointClamp{ renderer, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP },
	samplerLinearClamp{ renderer, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP },
	samplerAnisotropicClamp{ renderer, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP },
	samplerPointBorder{ renderer, D3D11_FILTER_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS, {10000.f,10000.f,10000.f,10000.f} },
	samplerPointBorderLess{ renderer, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_LESS, {0,0,0,0} },
	samplerPointBorderGreater{ renderer, D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT, D3D11_TEXTURE_ADDRESS_BORDER, D3D11_COMPARISON_GREATER, {10000.f,10000.f,10000.f,10000.f} },
	shadowMapWorldSpace{ renderer, mGlobalLight.shadowNear->shadowMap.width, mGlobalLight.shadowNear->shadowMap.height, DXGI_FORMAT_R32_FLOAT /*TODOformat_d24*/, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE }
{

	RandomGenerator::CreateNoiseTexture(renderer, renderer.GetViewportSize().first / 8, renderer.GetViewportSize().second / 8, noiseTexSRV);

	auto width = mGlobalLight.shadowNear->shadowMap.width;
	auto height = mGlobalLight.shadowNear->shadowMap.height;

	/*for (int j = 0; j < 2; j++)
	{
		shadowMapHoles[j].Create(renderer, width, height, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	}*/

	for (int i = 0; i < NumOfDownsampledShadowMaps; i++)
	{
		width /= 2;
		height /= 2;
		shadowMapDownsampled[i].Create(renderer, width, height, DXGI_FORMAT_R32G32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	}

	shadowMapDownsampledMinMaxDepth.Create(renderer, width, height, DXGI_FORMAT_R32G32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	volumeLightCBuffer.Create(renderer, 2u, sizeof(volumeLightCBufferData));
	volumeLightCBufferData.coarseStepDepthTexelSize = mGlobalLight.shadowNear->GetSize() / shadowMapDownsampledMinMaxDepth.width;
	volumeLightCBuffer.Update(renderer, volumeLightCBufferData);

}

void PostProcess::VolumeLight::Process(Rendering & renderer, GlobalLight & mGlobalLight, RenderTarget & result)
{

	//Volume Light
	if (ImGui::Begin("VolumeLight"))
	{
		ImGui::Checkbox("AngleOptimization", (bool*)&volumeLightCBufferData.bUseAngleOptimization);
		ImGui::Checkbox("ZBufferOptimisation", (bool*)&volumeLightCBufferData.bUseZOptimization);
		ImGui::SliderFloat("LongStepScale", &volumeLightCBufferData.coarseStepScale, 0.25f, 10.f);
		ImGui::SliderFloat("SamplingRate", &volumeLightCBufferData.samplingRate, 0.5, 10.f);
		ImGui::SliderInt("MaxSteps", &volumeLightCBufferData.maxSteps, 1000, 5000);
		ImGui::SliderFloat("MaxRayLength", &volumeLightCBufferData.maxRayLength, 500.f, 10000.f);
		ImGui::DragFloat("LightIntensity", &volumeLightCBufferData.intensity, 0.001f);
	}
	ImGui::End();

	volumeLightCBufferData.coarseStepDepthTexelSize = mGlobalLight.GetSize() / shadowMapDownsampledMinMaxDepth.width;
	volumeLightCBuffer.Update(renderer, volumeLightCBufferData);
	volumeLightCBuffer.Bind(renderer);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Convert light depth to world space
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	shadowMapWorldSpace.BindWithoutDepthStencil(renderer);
	//globalLight.shadowMap.SetAsPixelShaderResourceView(renderer, 0);
	mGlobalLight.SetShadowMapAsPSResourceView(renderer, 0);
	Sampler::Get(renderer, D3D11_FILTER_MIN_MAG_MIP_POINT).Bind(renderer);
	psConvertLightDepthToWorld.Bind(renderer);
	RenderTarget::DrawPlane(renderer);
	RenderTarget::SetDefaultShaderResourceForPS();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Downscale Light Depth, produce textures where R stores Min depth and G stores Max depth values (for coarse step optimization)
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	samplerPointClamp.Bind(renderer);
	shadowMapDownsampled[0].BindWithoutDepthStencil(renderer);
	shadowMapWorldSpace.SetAsPixelShaderResourceView(renderer);
	psMinMaxDepth1.Bind(renderer);
	RenderTarget::DrawPlane(renderer);
	RenderTarget::SetDefaultShaderResourceForPS();
	for (int i = 0; i < NumOfDownsampledShadowMaps - 1; i++)
	{
		shadowMapDownsampled[i + 1].BindWithoutDepthStencil(renderer);
		shadowMapDownsampled[i].SetAsPixelShaderResourceView(renderer);
		psMinMaxDepth2.Bind(renderer);
		RenderTarget::DrawPlane(renderer);
		RenderTarget::SetDefaultShaderResourceForPS();
	}
	shadowMapDownsampledMinMaxDepth.BindWithoutDepthStencil(renderer);
	shadowMapDownsampled[NumOfDownsampledShadowMaps - 1].SetAsPixelShaderResourceView(renderer);
	psMinMaxDepth3.Bind(renderer);
	RenderTarget::DrawPlane(renderer);
	RenderTarget::SetDefaultShaderResourceForPS();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw Volume Light
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	result.BindWithoutDepthStencil(renderer);

	Sampler::Get(renderer, D3D11_FILTER_MIN_MAG_MIP_POINT).Bind(renderer, 0);
	Sampler::Get(renderer).Bind(renderer, 1);
	samplerPointBorder.Bind(renderer, 2);
	samplerPointBorderLess.Bind(renderer, 3);
	samplerPointBorderGreater.Bind(renderer, 4);
	renderer.SetDepthAsPixelShaderResourceView(0u);
	shadowMapWorldSpace.SetAsPixelShaderResourceView(renderer, 1u);
	renderer.GetContext()->PSSetShaderResources(2u, 1, noiseTexSRV.GetAddressOf());
	shadowMapDownsampledMinMaxDepth.SetAsPixelShaderResourceView(renderer, 3u);
	psVolumeLight.Bind(renderer);
	RenderTarget::DrawPlane(renderer);
	RenderTarget::SetDefaultShaderResourceForPS();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

}

void PostProcess::LightScattering::Process(Rendering & renderer, GlobalLight & globalLight, Camera & mainCamera, RenderTarget & input, RenderTarget & output)
{
	mDesc.Update(globalLight.GetPosition(), mainCamera.GetViewProjectionMatrix());
	psLightScatterCBuffer.Bind(renderer);
	psLightScatterCBuffer.Update(renderer, mDesc);
	output.BindWithoutDepthStencil(renderer);
	samplerLinearClamp.Bind(renderer);
	input.SetAsPixelShaderResourceView(renderer, 0);
	psLightScattering.Bind(renderer);
	RenderTarget::DrawPlane(renderer);
}
