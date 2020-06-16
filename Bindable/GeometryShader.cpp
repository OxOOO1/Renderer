
#include "GeometryShader.h"
#include <d3dcompiler.h>
#include <string>
#include "TexturesShadersPool.h"



GeometryShader::GeometryShader(Rendering & renderer, const std::string& Name)
{

	renderer.ThrowIfFailed(D3DReadFileToBlob((std::wstring{ Name.begin(), Name.end() } +L".cso").c_str(), &pByteCode));


	renderer.ThrowIfFailed(renderer.GetDevice()->CreateGeometryShader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), nullptr, &pGeometryShader));
}

void GeometryShader::Bind(Rendering & renderer) const
{
	renderer.GetContext()->GSSetShader(pGeometryShader.Get(), nullptr, 0u);
}

std::shared_ptr<GeometryShader> GeometryShader::Resolve(Rendering & renderer, const std::string path)
{
	return TexturesShadersPool::Resolve<GeometryShader>(renderer, path);
}

std::string GeometryShader::GenerateUID(const std::string & path)
{
	using namespace std::string_literals;
	return typeid(GeometryShader).name() + "#"s + path;
}


void GeometryShaderWithSO::Create(Rendering & renderer, const std::string& Name, const std::vector<D3D11_SO_DECLARATION_ENTRY>& decl, bool streamOnly) 
{

	UINT raster = 0u;
	if (streamOnly)
	{
		raster = D3D11_SO_NO_RASTERIZED_STREAM;
	}

	renderer.ThrowIfFailed(D3DReadFileToBlob((std::wstring{ Name.begin(), Name.end() } +L".cso").c_str(), &pByteCode));

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateGeometryShaderWithStreamOutput(
		pByteCode->GetBufferPointer(),
		pByteCode->GetBufferSize(),
		decl.data(),
		decl.size(),
		nullptr,
		0,
		raster,
		nullptr,
		&pGeometryShader
	));
}

void GeometryShaderWithSO::Create(Rendering & renderer, Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, const std::vector<D3D11_SO_DECLARATION_ENTRY>& decl, bool streamOnly)
{
	UINT raster = 0u;
	if (streamOnly)
	{
		raster = D3D11_SO_NO_RASTERIZED_STREAM;
	}

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateGeometryShaderWithStreamOutput(
		vertexShader->GetBufferPointer(),
		vertexShader->GetBufferSize(),
		decl.data(),
		decl.size(),
		nullptr,
		0,
		raster,
		nullptr,
		&pGeometryShader
	));

}

void GeometryShaderWithSO::Bind(Rendering & renderer) const 
{
	renderer.GetContext()->GSSetShader(pGeometryShader.Get(), nullptr, 0u);
}

