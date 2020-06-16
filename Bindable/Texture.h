#pragma once
#include <vector>

#include "System/Rendering.h"

class Texture
{
public:

	static constexpr auto DiffuseSlot = 0;
	static constexpr auto NormalSlot = 1;
	static constexpr auto SpecularSlot = 2;
	static constexpr auto EmissionSlot = 4;
	
	Texture(Rendering& renderer, std::string_view path, unsigned int slot = 0);
	void Bind(Rendering& renderer) const;
	void Bind(Rendering& renderer, UINT slot) const;

	void BindVS(Rendering& renderer) const;
	
	void LoadFromWIC(Rendering& renderer, std::string_view path);
	void LoadFromDDS(Rendering& renderer, std::string_view path);

	static UINT CalculateNumberOfMIP(UINT width, UINT height);

	static std::shared_ptr<Texture> Resolve(Rendering& renderer, std::string_view path, unsigned int slot = 0u);
	static std::string GenerateUID(std::string_view path, unsigned int slot);
	
	bool HasActiveAlpha() const noexcept
	{
		return alphaLoaded;
	}
	
private:
	
	unsigned int slot;
	bool alphaLoaded = false;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
};

class TextureArray
{
public:

	TextureArray(Rendering& renderer, const std::vector<std::string_view>& paths, unsigned int slot = 0);
	void Bind(Rendering& renderer) const ;

	
	static std::shared_ptr<TextureArray> Resolve(Rendering& renderer, const std::vector<std::string_view>& paths, unsigned int slot = 0);
	static std::string GenerateUID(const std::vector<std::string_view>& paths, unsigned int slot);
	
	bool HasActiveAlpha() const noexcept
	{
		return alphaLoaded;
	}
	
private:
	
	unsigned int slot;
	
	bool alphaLoaded = false;
protected:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureArrayView;
};