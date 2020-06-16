#pragma once

#include "System/Rendering.h"

class ComputeShader
{
public:
	ComputeShader(Rendering& renderer, const std::string& Name);

	void Bind(Rendering& renderer) const ;

	Microsoft::WRL::ComPtr<ID3DBlob>& GetByteCode()  { return pByteCode; }

private:
	Microsoft::WRL::ComPtr<ID3DBlob> pByteCode;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> pComputeShader;
};