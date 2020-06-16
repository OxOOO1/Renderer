#include "PixelShader.h"
#include <d3dcompiler.h>
#include <string>
#include "TexturesShadersPool.h"

using namespace std::string_literals;

std::string_view PixelShader::pCurrentlyBoundID = std::string_view{};

PixelShader::PixelShader(Rendering & renderer, const std::string & Name) : id(Name)
{

	renderer.ThrowIfFailed(D3DReadFileToBlob((std::wstring{ Name.begin(), Name.end() } +L".cso").c_str(), &pByteCode));

	renderer.ThrowIfFailed(renderer.GetDevice()->CreatePixelShader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), nullptr, &pPixelShader));

}

 void PixelShader::Bind(Rendering & renderer) const
{
	 if (id != pCurrentlyBoundID)
	 {
		 pCurrentlyBoundID = id;
		 renderer.GetContext()->PSSetShader(pPixelShader.Get(), nullptr, 0u);
	 }

}

 std::shared_ptr<PixelShader> PixelShader::Resolve(Rendering & renderer, const std::string& path)
 {
	 return TexturesShadersPool::Resolve<PixelShader>(renderer, path);
 }

 std::string PixelShader::GenerateUID(const std::string & path)
 {
	 return path;
 }
