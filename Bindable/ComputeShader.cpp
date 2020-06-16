
#include "ComputeShader.h"
#include <d3dcompiler.h>
#include <string>


ComputeShader::ComputeShader(Rendering & renderer, const std::string& Name)
{

	renderer.ThrowIfFailed(D3DReadFileToBlob((std::wstring{ Name.begin(), Name.end() } +L".cso").c_str(), &pByteCode));


	renderer.ThrowIfFailed(renderer.GetDevice()->CreateComputeShader(pByteCode->GetBufferPointer(), pByteCode->GetBufferSize(), nullptr, &pComputeShader));
}

void ComputeShader::Bind(Rendering & renderer) const
{
	renderer.GetContext()->CSSetShader(pComputeShader.Get(), nullptr, 0u);
}
