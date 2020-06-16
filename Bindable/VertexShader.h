#pragma once

#include <string>

#include "System/Rendering.h"
#include "InputLayout.h"

class VertexShader
{
public:

	VertexShader(Rendering& renderer, std::string_view Name, const VertexLayout& vertexLayout);
	VertexShader(Rendering& renderer, std::string_view Name, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout);

	void Bind(Rendering& renderer) const ;

	inline Microsoft::WRL::ComPtr<ID3DBlob>& GetByteCode() { return pByteCode; }

	static std::shared_ptr<VertexShader> Resolve(Rendering& renderer, std::string_view path, const VertexLayout& vertexLayout);
	static std::string GenerateUID( std::string_view path, const VertexLayout& vertexLayout);
	
public:
	InputLayout mVertexInputLayout;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> pByteCode;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader;

	static std::string_view pCurrentlyBoundID;
	std::string strID;
};