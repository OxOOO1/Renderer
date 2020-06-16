#pragma once

#include "System/Rendering.h"

class GeometryShader
{
public:
	GeometryShader(Rendering& renderer, const std::string& Name);

	void Bind(Rendering& renderer) const;

	Microsoft::WRL::ComPtr<ID3DBlob>& GetByteCode() { return pByteCode; }

	static void SetNull(Rendering& renderer)
	{
		renderer.GetContext()->GSSetShader(nullptr, nullptr, 0u);
	}

	static std::shared_ptr<GeometryShader> Resolve(Rendering& renderer, const std::string path);
	static std::string GenerateUID(const std::string& path);
	
private:
	
	Microsoft::WRL::ComPtr<ID3DBlob> pByteCode;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
};

class GeometryShaderWithSO
{
public:

	void Create(Rendering& renderer, const std::string& Name, const std::vector<D3D11_SO_DECLARATION_ENTRY>& decl, bool streamOnly = false);

	void Create(Rendering& renderer, Microsoft::WRL::ComPtr<ID3DBlob> vertexShader, const std::vector<D3D11_SO_DECLARATION_ENTRY>& decl, bool streamOnly = true);

	void Bind(Rendering& renderer) const ;

	Microsoft::WRL::ComPtr<ID3DBlob>& GetByteCode() { return pByteCode; }

	void UnBind(Rendering& renderer)
	{
		renderer.GetContext()->GSSetShader(nullptr, nullptr, 0u);
	}

private:
	
	Microsoft::WRL::ComPtr<ID3DBlob> pByteCode;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> pGeometryShader;
};