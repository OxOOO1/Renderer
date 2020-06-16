#include "VertexShader.h"
#include <d3dcompiler.h>
#include <string>
#include "TexturesShadersPool.h"
using namespace std::string_literals;

std::string_view VertexShader::pCurrentlyBoundID = std::string_view{};

VertexShader::VertexShader(Rendering & renderer, std::string_view Name, const VertexLayout& vertexLayout) : strID(Name)
{
	renderer.ThrowIfFailed(D3DReadFileToBlob((std::wstring{ Name.begin(), Name.end() } +L".cso").c_str(), &pByteCode));

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateVertexShader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), nullptr, &pVertexShader));

	mVertexInputLayout.Create(renderer, vertexLayout, pByteCode.Get());
}

VertexShader::VertexShader(Rendering & renderer, std::string_view Name, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout)
{
	renderer.ThrowIfFailed(D3DReadFileToBlob((std::wstring{ Name.begin(), Name.end() } +L".cso").c_str(), &pByteCode));

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateVertexShader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), nullptr, &pVertexShader));

	mVertexInputLayout.Create(renderer, layout, pByteCode.Get());
}


 void VertexShader::Bind(Rendering & renderer) const
{
	if (strID != pCurrentlyBoundID)
	{
		pCurrentlyBoundID = strID;
		mVertexInputLayout.Bind(renderer);
		renderer.GetContext()->VSSetShader(pVertexShader.Get(), nullptr, 0u);
	}
}

 std::shared_ptr<VertexShader> VertexShader::Resolve(Rendering & renderer, std::string_view path, const VertexLayout& vertexLayout)
 {
	 return TexturesShadersPool::Resolve<VertexShader>(renderer, path, vertexLayout);
 }

 std::string VertexShader::GenerateUID(std::string_view path, const VertexLayout& vertexLayout)
 {
	 return std::string{ path };
 }
