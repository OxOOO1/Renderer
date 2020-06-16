#include "Particles.h"

Particles::ParticleDescBuffer Particles::particleDescBufferData;
std::unique_ptr<VSGSConstBuffer<Particles::ParticleDescBuffer>> Particles::m_pGSCBuffer;

Particles::Particles(Rendering & renderer, UINT numOfParticles)
{
	m_pGSCBuffer = std::make_unique<VSGSConstBuffer<ParticleDescBuffer>>(renderer);

	const std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
	{
		{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Velocity", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Age", 0,		DXGI_FORMAT_R32_FLOAT,		 0,		24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "Type", 0,		DXGI_FORMAT_R32_UINT,		 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	/////////////////////////// STREAM OUT PASS /////////////////////////

	m_VertexShaderSO = std::make_shared<VertexShader>(renderer, "cso\\ParticlesSO_VS", layout); 

	m_VertexBufferDefault.CreateConst(renderer, sizeof(defaultParticle), sizeof(defaultParticle), &defaultParticle);

	m_VertexBufferSOWrite.CreateStreamOutput(renderer, numOfParticles * sizeof(ParticlePoint), sizeof(ParticlePoint));
	m_VertexBufferSODraw.CreateStreamOutput(renderer, numOfParticles * sizeof(ParticlePoint), sizeof(ParticlePoint));

	const std::vector<D3D11_SO_DECLARATION_ENTRY> declEntry =
	{
		{ 0, "Position",	0, 0, 3, 0 },
		{ 0, "Velocity",	0, 0, 3, 0 },
		{ 0, "Age",			0, 0, 1, 0 },
		{ 0, "Type",		0, 0, 1, 0 },
	};

	m_GeometryShaderSO.Create(renderer, std::string{ "cso\\ParticlesSO_GS" }, declEntry, true);

	RandomGenerator::CreateRandomNumbersTexture1D(renderer, randomValuesTextureSRV);

	/////////////////////////////// RENDER PASS //////////////////////////////////////////////

	m_VertexShader = std::make_shared<VertexShader>(renderer, "cso\\Particles_VS", layout);
	auto bytecode = m_VertexShader->GetByteCode();

	m_InputLayout.Create(renderer, layout, bytecode.Get());

	m_PixelShader = PixelShader::Resolve(renderer, "cso\\Particles_PS");

	m_GeometryShader = GeometryShader::Resolve(renderer, "cso\\Particles_GS");

	//m_TextureArray = TextureArray::Resolve(renderer, texturePaths, 0u);
	m_Texture = Texture::Resolve(renderer, "assets\\textures\\billboards\\smokeCloud.png", 1u);

	m_BlendState = &Blender::GetParticlesBlender(renderer, true, false);
	//m_BlendState = &Blender::Get(renderer, true, false);
	m_RasterState = &Rasterizer::Get(renderer, true);

}
