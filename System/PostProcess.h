#pragma once
#include "Rendering.h"
#include "Bindable\ComputeShader.h"
#include "RenderTarget.h"
#include "System/Utilities/Random.h"
#include "src/Scene/Lights/GlobalLight.h"

namespace PostProcess
{
	class BlurFilter
	{
	public:

		void Process(Rendering& renderer, RenderTarget& target);

		BlurFilter(Rendering& renderer, const RenderTarget& targetToBlur)
			:
			rt2{ renderer, targetToBlur.width, targetToBlur.height, targetToBlur.format, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS },
			computeShaderH{ renderer, "cso\\CS_SSBlurHorizontal" },
			computeShaderV{ renderer, "cso\\CS_SSBlurVertical" }
		{
		}

		int numOfPasses = 1;

	private:
		RenderTarget rt2;

		ComputeShader computeShaderH;
		ComputeShader computeShaderV;

	};

	class VolumeLight
	{

		static constexpr int NumOfDownsampledShadowMaps = 4;
		static constexpr int ShadowHoleMIPIndex = 0;
		static_assert(ShadowHoleMIPIndex < NumOfDownsampledShadowMaps);

	public:
		VolumeLight(Rendering& renderer, GlobalLight& mGlobalLight);

		void Process(Rendering& renderer, GlobalLight& mGlobalLight, RenderTarget& result);

	private:

		//Textures
		RenderTarget shadowMapWorldSpace;
		RenderTarget shadowMapDownsampled[NumOfDownsampledShadowMaps];
		RenderTarget shadowMapDownsampledMinMaxDepth;
		//RenderTarget shadowMapHoles[2];

		PixelShader psVolumeLight;

		PixelShader psMinMaxDepth1;
		PixelShader psMinMaxDepth2;
		PixelShader psMinMaxDepth3;

		PixelShader psPropagateMinDepth1;
		PixelShader psPropagateMinDepth2;
		PixelShader psPropagateMaxDepth;

		PixelShader psConvertCamDepthToWorld;
		PixelShader psConvertLightDepthToWorld;

		struct VolumeLightCBuffer
		{
			float coarseStepDepthTexelSize;
			float samplingRate = 1.f;
			UINT bUseZOptimization = TRUE;
			UINT bUseAngleOptimization = TRUE;

			float coarseStepScale = 1.f;
			float intensity = 0.25f;
			float maxRayLength = 2500.f;
			int32_t maxSteps = 2000;

		} volumeLightCBufferData;

		PixelConstBuffer<VolumeLightCBuffer> volumeLightCBuffer;

		////NOISE
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noiseTexSRV;

		//Samplers
		Sampler samplerPointClamp;
		Sampler samplerLinearClamp;
		Sampler samplerAnisotropicClamp;
		Sampler samplerPointBorder;
		Sampler samplerPointBorderLess;
		Sampler samplerPointBorderGreater;


	};

	class AntiAliasing
	{

	public:
		AntiAliasing(Rendering& renderer)
			:
			psFXAA{ renderer, "cso\\RTFXAA_PS" },
			samplerAnisotropicClamp{ renderer, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_CLAMP }
		{

		}

		void Process(Rendering& renderer, RenderTarget& input, RenderTarget& output)
		{
			output.BindWithoutDepthStencil(renderer);
			input.SetAsPixelShaderResourceView(renderer, 0);
			samplerAnisotropicClamp.Bind(renderer);
			psFXAA.Bind(renderer);
			output.DrawPlane(renderer);
		}

	private:

		PixelShader psFXAA;

		Sampler samplerAnisotropicClamp;
	};

	class LightScattering
	{

	public:
		LightScattering(Rendering& renderer)
			:
			samplerLinearClamp{ renderer, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP },
			psLightScattering{ renderer, "cso\\RTLightScattering_PS" }
		{
			psLightScatterCBuffer.Create(renderer, 2u, sizeof(mDesc));
		}

		void Process(Rendering& renderer, GlobalLight& globalLight, Camera& mainCamera, RenderTarget& input, RenderTarget& output);

		void ShowControlWindow()
		{
			if (ImGui::Begin("LightScattering"))
			{
				ImGui::SliderFloat("Density", &mDesc.density, 0.1f, 1.f);
				ImGui::SliderFloat("Decay", &mDesc.decay, 0.1f, 1.f);
				ImGui::SliderInt("NumOfSamples", &mDesc.numOfSamples, 1, 1000);
				ImGui::SliderFloat("Exposure", &mDesc.exposure, 0.001f, 1.f);
				ImGui::SliderFloat("Abberation", &mDesc.abberation, 0., 500);
				ImGui::Checkbox("EnabledAbberation", (bool*)&mDesc.abberationEnabled);
			}
			ImGui::End();
		}

		struct Desc
		{
			void Update(const DirectX::XMFLOAT3& lightPos, const DirectX::XMMATRIX& camVP)
			{
				DirectX::XMFLOAT4 lightPos4 = { lightPos.x, lightPos.y, lightPos.z, 1.f };
				auto lightPosNDC = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&lightPos4), camVP);
				DirectX::XMStoreFloat4(&lightPos4, lightPosNDC);
				DirectX::XMStoreFloat4(&lightPos4, DirectX::XMVectorScale(lightPosNDC, 1.f / lightPos4.w));
				posUV.x = (lightPos4.x + 1.0f) * 0.5f;
				posUV.y = (1.0f - lightPos4.y) * 0.5f;
			}

			DirectX::XMFLOAT2 posUV;
			float density = 1.f;
			float decay = 0.93f;
			int numOfSamples = 100;
			float exposure = 1.f;
			float abberation = 25;
			UINT abberationEnabled = TRUE;
		} mDesc;

		PixelConstBuffer<Desc> psLightScatterCBuffer;

		PixelShader psLightScattering;

		Sampler samplerLinearClamp;
	};

	class LightAbberation
	{
	public:

		LightAbberation(Rendering& renderer)
			:
			psLightAbberation{ renderer, "cso\\RTLightAbberation_PS" },
			samplerLinearClamp{ renderer, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP }
		{

		}

		void Process(Rendering& renderer, RenderTarget& input, RenderTarget& output)
		{
			output.BindWithoutDepthStencil(renderer);
			samplerLinearClamp.Bind(renderer);
			input.SetAsPixelShaderResourceView(renderer);
			psLightAbberation.Bind(renderer);
			RenderTarget::DrawPlane(renderer);
		}

		Sampler samplerLinearClamp;

		PixelShader psLightAbberation;

	};

	class Blender
	{
	public:
		Blender(Rendering& renderer)
			:
			psFinalBlend{ renderer, "cso\\RTFinalBlendPass_PS" }
		{

		}

		void Process(Rendering& renderer, std::initializer_list<RenderTarget> rts, RenderTarget& output)
		{
			output.BindWithoutDepthStencil(renderer);
			//////Blend
			auto it = rts.begin();
			for (int i = 0; i < rts.size(); i++)
			{
				it->SetAsPixelShaderResourceView(renderer, i);
				it++;
			}
			psFinalBlend.Bind(renderer);
			output.DrawPlane(renderer);
		}

		PixelShader psFinalBlend;

	};




};//namespace