
#include "ImageLoader.h"
#include <algorithm>
#include <filesystem>
#include <cassert>

#include <sstream>

#include "src/System/Window.h"
#include "System/Utilities/Time.h"

#include "DDSTextureLoader/DDSTextureLoader.h"

TextureLoader::TextureLoader(unsigned int width, unsigned int height) 
{
	HRESULT hr = scratch.Initialize2D(format, width, height, 1u, 1u);
	if (FAILED(hr))
	{
		throw TextureLoader::TexException(__LINE__, __FILE__, "Failed to Initialize Scratch Image", hr);
	}
}

TextureLoader::TextureLoader(DirectX::ScratchImage scratch) noexcept 
	:scratch(std::move(scratch))
{
}

void TextureLoader::Clear(Color fillValue) noexcept
{
	const auto width = GetWidth();
	const auto height = GetHeight();
	auto& imgData = *scratch.GetImage(0, 0, 0);
	for (size_t y = 0u; y < height; y++)
	{
		auto rowStart = reinterpret_cast<Color*>(imgData.pixels + imgData.rowPitch * y);
		std::fill(rowStart, rowStart + imgData.width, fillValue);
	}
}

void TextureLoader::PutPixel(unsigned int x, unsigned int y, Color c) noexcept
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetWidth());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x] = c;
}

TextureLoader::Color TextureLoader::GetPixel(unsigned int x, unsigned int y) const noexcept
{
	assert(x >= 0);
	assert(y >= 0);
	assert(x < GetWidth());
	assert(y < GetWidth());
	auto& imgData = *scratch.GetImage(0, 0, 0);
	return reinterpret_cast<Color*>(&imgData.pixels[y * imgData.rowPitch])[x];
}

unsigned int TextureLoader::GetWidth() const noexcept
{
	return (unsigned int)scratch.GetMetadata().width;
}

unsigned int TextureLoader::GetHeight() const noexcept
{
	return (unsigned int)scratch.GetMetadata().height;
}

TextureLoader::Color * TextureLoader::GetBufferPtr() noexcept
{
	return reinterpret_cast<Color*>(scratch.GetPixels());
}

const TextureLoader::Color * TextureLoader::GetBufferPtr() const noexcept
{
	return const_cast<TextureLoader*>(this)->GetBufferPtr();
}

const TextureLoader::Color* TextureLoader::GetBufferPtrConst() const noexcept
{
	return const_cast<TextureLoader*>(this)->GetBufferPtr();
}

TextureLoader TextureLoader::FromWICFile(std::string_view name)
{
	
	wchar_t wideName[512];
	// convert filename to wide string 
	mbstowcs_s(nullptr, wideName, name.data(), _TRUNCATE);

	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromWICFile(wideName, DirectX::WIC_FLAGS_NONE, nullptr, scratch);
	
	if (FAILED(hr))
	{
		throw TextureLoader::TexException(__LINE__, __FILE__, "Failed to load Image from a File", hr);
	}

	if (scratch.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (FAILED(hr))
		{
			throw TextureLoader::TexException(__LINE__, __FILE__, "Failed to convert Image to default format", hr);
		}

		return TextureLoader(std::move(converted));
	}

	return TextureLoader(std::move(scratch));
}

TextureLoader TextureLoader::FromDDSFile(std::string_view name)
{
	wchar_t wideName[512];
	// convert filename to wide string 
	mbstowcs_s(nullptr, wideName, name.data(), _TRUNCATE);

	DirectX::ScratchImage scratch;
	HRESULT hr = DirectX::LoadFromDDSFile(wideName, DirectX::WIC_FLAGS_NONE, nullptr, scratch);

	if (FAILED(hr))
	{
		throw TextureLoader::TexException(__LINE__, __FILE__, "Failed to load Image from a File", hr);
	}

	if (scratch.GetImage(0, 0, 0)->format != format)
	{
		DirectX::ScratchImage converted;
		hr = DirectX::Convert(
			*scratch.GetImage(0, 0, 0),
			format,
			DirectX::TEX_FILTER_DEFAULT,
			DirectX::TEX_THRESHOLD_DEFAULT,
			converted
		);

		if (FAILED(hr))
		{
			throw TextureLoader::TexException(__LINE__, __FILE__, "Failed to convert Image to default format", hr);
		}

		return TextureLoader(std::move(converted));
	}

	return TextureLoader(std::move(scratch));
}

void TextureLoader::Save(const std::string & filename) const
{
	const auto GetCodecID = [](const std::string filename)
	{
		const std::filesystem::path path = filename;
		const auto ext = path.extension().string();
		if (ext == ".png")
		{
			return DirectX::WIC_CODEC_PNG;
		}

		else if (ext == ".png")
		{
			return DirectX::WIC_CODEC_JPEG;
		}

		else if (ext == ".bmp")
		{
			return DirectX::WIC_CODEC_BMP;
		}

		else
		{
			throw TextureLoader::TexException( __LINE__, __FILE__, "Unable to save Image of this format");
		}
	};

	wchar_t wideName[512];
	// convert filename to wide string 
	mbstowcs_s(nullptr, wideName, filename.c_str(), _TRUNCATE);

	HRESULT hr = DirectX::SaveToWICFile(
		*scratch.GetImage(0, 0, 0),
		DirectX::WIC_FLAGS_NONE,
		GetWICCodec(GetCodecID(filename)),
		wideName
		);
		
	if (FAILED(hr))
	{
		throw TextureLoader::TexException(__LINE__, __FILE__, "Failed to save Image ", hr);
	}


}


bool TextureLoader::AlphaLoaded() const noexcept
{
	return !scratch.IsAlphaAllOpaque();
}

TextureLoader::TexException::TexException(int line, const char * file, std::string note) noexcept
	:Exception(line, file),
	note(std::move(note))
{

}

TextureLoader::TexException::TexException(int line, const char * file, std::string note, HRESULT hr) noexcept
	:Exception(line, file), hr(hr), note(std::move(note))
{

}



const char * TextureLoader::TexException::what() const noexcept
{
	std::ostringstream ss;
	ss << GetType() << std::endl
	 << GetNote() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = ss.str();
	return whatBuffer.c_str();
}

const char * TextureLoader::TexException::GetType() const noexcept
{
	return "Texture Loader Exception";
}

const std::string & TextureLoader::TexException::GetNote() const noexcept
{
	return note;
}

HRESULT TextureLoader::TexException::GetErrorCode() const noexcept
{
	return hr;
}

std::string TextureLoader::TexException::GetErrorString() const noexcept
{
	return Window::WinException::TranslateErrorCode(hr);
}