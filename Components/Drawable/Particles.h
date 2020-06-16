#pragma once

#include "ImGUI/imgui.h"

#include "System/Rendering.h"
#include "Bindable/BindablesH.h"

#include "System/Utilities/Random.h"

#include "System/Utilities/Time.h"


class Particles
{
public:

	enum class Type : UINT
	{
		Emitter = 0u,
		Flare = 1u,
	};

	Particles(Rendering& renderer, UINT numOfParticles);
	
	void ShowControlWindow()
	{
		if(ImGui::Begin("Particles"))
		{
			ImGui::SliderInt("NumOfCopies", (int*)&particleDescBufferData.numOfCopies, 0, 25);
			ImGui::SliderFloat("EmitPeriod", &particleDescBufferData.emitPeriod, 0.00001f, 500.f);
			ImGui::SliderFloat("Lifetime", &particleDescBufferData.lifeTime, 100.f, 5000.f);
			ImGui::Checkbox("RandomPosition", (bool*)&particleDescBufferData.isPositionRandom);
			ImGui::DragFloat3("Position", &particleDescBufferData.position.x);
			ImGui::Checkbox("RandomVelocity", (bool*)&particleDescBufferData.isVelocityRandom);
			ImGui::DragFloat3("Acceleration", &particleDescBufferData.acceleration.x);
			ImGui::SliderFloat("TimeStep", &particleDescBufferData.timeStepModifier, 10.f, 10000.f);
			ImGui::DragFloat3("Spread", &particleDescBufferData.spreadRadius.x);
			ImGui::DragFloat("Size", &particleDescBufferData.size);
			ImGui::ColorEdit3("ColorBegin", &particleDescBufferData.color1.x);
			ImGui::ColorEdit3("ColorEnd", &particleDescBufferData.color2.x);

		}
		ImGui::End();
	}

	static void OnUpdate(Rendering& renderer)
	{
		particleDescBufferData.dTime = Time::Get().GetDeltaTimeMs();
		particleDescBufferData.globalTime = Time::Get().GetTimeAfterLaunchMs();
		Particles::m_pGSCBuffer->Update(renderer, particleDescBufferData);
	}

	void SetEmitPosition(Rendering& renderer, const DirectX::XMFLOAT3& pos)
	{
		
	}

	void SetEmitDirection(Rendering& renderer, const DirectX::XMFLOAT3& dir)
	{
		
	}

	void Reset(Rendering& renderer)
	{
		m_VertexBufferDefault.CreateConst(renderer, sizeof(defaultParticle), sizeof(defaultParticle), &defaultParticle);
		defaultPass = true;
	}

	void Draw(Rendering& renderer)
	{
		DepthStencilState::BindDefaultNoDepthWrites(renderer);
		Begin(renderer);
		renderer.GetContext()->DrawAuto();
		End(renderer);
		DepthStencilState::BindDefault(renderer);
	}

	struct ParticlePoint
	{
		DirectX::XMFLOAT3 position{ 0.f,0.f,0.f };
		DirectX::XMFLOAT3 velocity{ 0.f,0.f,0.f };
		float age{ 0.f };
		UINT type{ 0u };

	} defaultParticle;

private:

	void Begin(Rendering& renderer)
	{
		m_pGSCBuffer->Bind(renderer);

		renderer.GetContext()->GSSetShaderResources(0u, 1u, randomValuesTextureSRV.GetAddressOf());
		Sampler::Get(renderer).BindGS(renderer, 0u);

		m_InputLayout.Bind(renderer);
		Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST).Bind(renderer);

		{
			m_VertexShaderSO->Bind(renderer);
			m_GeometryShaderSO.Bind(renderer);

			if (!defaultPass)
			{
				m_VertexBufferSOWrite.Bind(renderer);
				m_VertexBufferSODraw.BindSO(renderer);
				renderer.GetContext()->DrawAuto(); 
			}
			else
			{
				m_VertexBufferDefault.Bind(renderer);
				m_VertexBufferSODraw.BindSO(renderer);
				renderer.GetContext()->Draw(1u, 0u);
				defaultPass = false;
			}

			m_VertexBufferSODraw.UnbindSO(renderer);
		}
		
		m_VertexBufferSODraw.Bind(renderer);

		m_VertexShader->Bind(renderer);
		renderer.GetContext()->VSSetShaderResources(0u, 1u, randomValuesTextureSRV.GetAddressOf());
		Sampler::Get(renderer).BindVS(renderer, 0u);

		m_PixelShader->Bind(renderer);
		if (m_Texture) m_Texture->Bind(renderer);
		else m_TextureArray->Bind(renderer);
		m_BlendState->Bind(renderer);
		m_RasterState->Bind(renderer);
		Sampler::Get(renderer).Bind(renderer);

		m_GeometryShader->Bind(renderer);

	}

	void End(Rendering& renderer)
	{
		m_GeometryShaderSO.UnBind(renderer);
		m_GeometryShader->SetNull(renderer);
		std::swap(m_VertexBufferSODraw, m_VertexBufferSOWrite);
	}

	/////////////// Stream Out Pass //////////////

	std::shared_ptr<VertexShader> m_VertexShaderSO;

	VertexBuffer m_VertexBufferDefault;
	VertexBuffer m_VertexBufferSODraw;
	VertexBuffer m_VertexBufferSOWrite;

	bool activeVBIndex = 0;

	GeometryShaderWithSO m_GeometryShaderSO;

	////////////// Render Pass ///////////////

	std::shared_ptr<VertexShader> m_VertexShader;
	
	InputLayout m_InputLayout;
	
	std::shared_ptr<PixelShader> m_PixelShader;
	std::shared_ptr<Texture> m_Texture;
	std::shared_ptr<TextureArray> m_TextureArray;
	Blender* m_BlendState;
	Rasterizer* m_RasterState;

	std::shared_ptr<GeometryShader> m_GeometryShader;
	
	struct ParticleDescBuffer
	{
		DirectX::XMFLOAT3 color1{ 1.f,1.f,1.f };
		float dTime;

		DirectX::XMFLOAT3 color2{ 1.f,1.f,1.f };
		float globalTime;

		DirectX::XMFLOAT3 spreadRadius{ 10.f,10.f,10.f };
		float lifeTime = 2000.f;

		DirectX::XMFLOAT3 acceleration{ 0.5f,1.3f, 0.f };
		float emitPeriod = 50.f;

		DirectX::XMFLOAT3 position{ 0.f,0.f,0.f };
		float size = 20.f;

		UINT numOfCopies = 0;
		float timeStepModifier = 100.f;
		UINT isPositionRandom = 0;
		UINT isVelocityRandom = 1;
	};

	static ParticleDescBuffer particleDescBufferData;
	static std::unique_ptr<VSGSConstBuffer<ParticleDescBuffer>> m_pGSCBuffer;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> randomValuesTextureSRV;

	bool defaultPass = true;
};

