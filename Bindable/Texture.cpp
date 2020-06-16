#include "Texture.h"
#include "TexturesShadersPool.h"

#include "DDSTextureLoader/DDSTextureLoader.h"
#include "System/Utilities/ImageLoaders/ImageLoader.h"

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

Texture::Texture(Rendering & renderer, std::string_view path, unsigned int slot) : slot(slot)
{
	const std::string_view substr = path;
	auto fFormat = substr.substr(substr.size() - 3, substr.size()).data();

	if (strcmp(fFormat, "dds") == 0) {
		LoadFromDDS(renderer, path);
	}
	else
	{
		LoadFromWIC(renderer, path);
	}
}

void Texture::Bind(Rendering & renderer) const
{
	renderer.GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
}

void Texture::BindVS(Rendering & renderer) const
{
	renderer.GetContext()->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());
}


void Texture::Bind(Rendering & renderer, UINT _slot) const
{
	renderer.GetContext()->PSSetShaderResources(_slot, 1u, pTextureView.GetAddressOf());
}

void Texture::LoadFromWIC(Rendering & renderer, std::string_view path)
{
	auto i = TextureLoader::FromWICFile(path);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = i.GetWidth();
	textureDesc.Height = i.GetHeight();
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS ;

	alphaLoaded = i.AlphaLoaded();
/*
	D3D11_SUBRESOURCE_DATA subdata;
	subdata.pSysMem = i.GetBufferPtrConst();
	subdata.SysMemPitch = i.GetWidth() * sizeof(TextureLoader::Color);
*/
	ComPtr<ID3D11Texture2D> pTexture;
	renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&textureDesc, nullptr, &pTexture));

	renderer.GetContext()->UpdateSubresource(
		pTexture.Get(),
		0u,
		nullptr,
		i.GetBufferPtrConst(),
		i.GetWidth() * sizeof(TextureLoader::Color),
		0u);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(pTexture.Get(), &srvDesc, &pTextureView));

	renderer.GetContext()->GenerateMips(pTextureView.Get());
}

void Texture::LoadFromDDS(Rendering & renderer, std::string_view path)
{
	//ComPtr<ID3D11Texture2D> pTexture;
	DirectX::DDS_ALPHA_MODE alphaMode;
	renderer.ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(renderer.GetDevice().Get(),
		nullptr,
		std::wstring{ path.begin(), path.end() }.c_str(),
		0U,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE,
		0,
		0,
		false,
		/*(ID3D11Resource**)pTexture.GetAddressOf()*/nullptr, &pTextureView, &alphaMode));
	
	if (alphaMode == DirectX::DDS_ALPHA_MODE_STRAIGHT || alphaMode == DirectX::DDS_ALPHA_MODE_PREMULTIPLIED) alphaLoaded = true;
}

UINT Texture::CalculateNumberOfMIP(UINT width, UINT height)
{
	const float xSteps = std::ceil(log2((float)width));
	const float ySteps = std::ceil(log2((float)height));
	return (UINT)((((xSteps > (ySteps)) ? (xSteps) : (ySteps))));
}

std::shared_ptr<Texture> Texture::Resolve(Rendering & renderer, std::string_view path, unsigned int slot)
{
	return TexturesShadersPool::Resolve<Texture>(renderer, path, slot);
}

std::string Texture::GenerateUID(std::string_view path, unsigned int slot)
{
	using namespace std::string_literals;
	return typeid(Texture).name() + "#"s + std::string{ path } +"#" + std::to_string(slot);
}


TextureArray::TextureArray(Rendering & renderer, const std::vector<std::string_view>& filenames, unsigned int slot) : slot(slot)
{

	UINT textureCount = (UINT)filenames.size();
	assert(textureCount);

	std::vector<Microsoft::WRL::ComPtr<ID3D11Texture2D>> textures(textureCount);

	DirectX::DDS_ALPHA_MODE alphaMode;
	for (UINT i = 0; i < textureCount; ++i)
	{
		renderer.ThrowIfFailed(DirectX::CreateDDSTextureFromFileEx(renderer.GetDevice().Get(),
			nullptr,
			std::wstring{ filenames.at(i).begin(), filenames.at(i).end() }.c_str(),
			0U,
			D3D11_USAGE_DEFAULT,
			D3D11_BIND_SHADER_RESOURCE,
			0,
			0,
			false,
			(ID3D11Resource**)textures.at(i).GetAddressOf(),
			nullptr,
			&alphaMode));
	}

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textures.front()->GetDesc(&textureDesc);
	
	D3D11_TEXTURE2D_DESC arrayDesc = {};
	arrayDesc.Width = textureDesc.Width;
	arrayDesc.Height = textureDesc.Height;
	arrayDesc.MipLevels = textureDesc.MipLevels;
	arrayDesc.ArraySize = textureCount;
	arrayDesc.Format = textureDesc.Format;
	arrayDesc.SampleDesc.Count = 1;
	arrayDesc.SampleDesc.Quality = 0;
	arrayDesc.Usage = D3D11_USAGE_DEFAULT;
	arrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	arrayDesc.CPUAccessFlags = 0;
	arrayDesc.MiscFlags = 0;

	if (alphaMode == DirectX::DDS_ALPHA_MODE_STRAIGHT || alphaMode == DirectX::DDS_ALPHA_MODE_PREMULTIPLIED) alphaLoaded = true;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> textureArray;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateTexture2D(&arrayDesc, nullptr, &textureArray));

	for (UINT arraySlice = 0; arraySlice < textureCount; ++arraySlice)
	{
		for (UINT mipSlice = 0; mipSlice < textureDesc.MipLevels; ++mipSlice)
		{
			const UINT sourceSubresourceIndex = D3D11CalcSubresource(mipSlice, 0, textureDesc.MipLevels);
			const UINT destSubresourceIndex = D3D11CalcSubresource(mipSlice, arraySlice, textureDesc.MipLevels);
			renderer.GetContext()->CopySubresourceRegion(
				textureArray.Get(),
				destSubresourceIndex,
				0,
				0,
				0,
				textures.at(arraySlice).Get(),
				sourceSubresourceIndex,
				nullptr);
		}
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = arrayDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	srvDesc.Texture2DArray.MipLevels = arrayDesc.MipLevels;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.ArraySize = textureCount;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateShaderResourceView(textureArray.Get(), &srvDesc, &pTextureArrayView));

}

void TextureArray::Bind(Rendering & renderer) const
{
	renderer.GetContext()->PSSetShaderResources(slot, 1u, pTextureArrayView.GetAddressOf());
}

std::shared_ptr<TextureArray> TextureArray::Resolve(Rendering & renderer, const std::vector<std::string_view>& paths, unsigned int slot)
{
	return TexturesShadersPool::Resolve<TextureArray>(renderer, paths, slot);
}

std::string TextureArray::GenerateUID(const std::vector<std::string_view>& paths, unsigned int slot)
{
	std::string s{ typeid(TextureArray).name() };

	for (auto&p : paths)
	{
		auto index = p.find_last_of("\\");
		s.append(std::string{ p.begin() + index, p.end() });
	}

	return s;
}

