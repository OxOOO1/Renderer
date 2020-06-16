#pragma once

#include "System/Rendering.h"

class Bindable;

class PixelShader
{
public:

	PixelShader(Rendering& renderer, const std::string& Name);

	void Bind(Rendering& renderer) const;

	static void SetNull(Rendering& renderer)
	{
		pCurrentlyBoundID = std::string_view{};
		renderer.GetContext()->PSSetShader(nullptr, nullptr, 0u);
	}

	static std::shared_ptr<PixelShader> Resolve(Rendering& renderer, const std::string& path);
	static std::string GenerateUID(const std::string& path);
	
private:
	
	std::string id;

	static std::string_view pCurrentlyBoundID;

	Microsoft::WRL::ComPtr<ID3DBlob> pByteCode;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};