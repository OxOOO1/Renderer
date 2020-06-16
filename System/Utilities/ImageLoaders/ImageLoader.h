#pragma once
#include <windows.h>
#include "src/thirdParty/DirectXTex/DirectXTex.h"
#include <string>
#include <optional>
#include "src/System/Exception.h"


class TextureLoader
{

public:

	class Color
	{
	
	public:
		constexpr Color() noexcept : dword()
		{}

		constexpr Color(const Color& col) noexcept
			:dword(col.dword)
		{}

		constexpr Color(unsigned int dw) noexcept
			: dword(dw)
		{}

		constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b)
			: dword((x << 24u) | (r << 16u) | (g << 8u) | b)
		{}

		constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
			:
			dword( (255u << 24u ) | (r << 16u) | (g << 8u) | b)
		{}
		constexpr Color(Color col, unsigned char x) noexcept
			:
			Color((x << 24u) | col.dword)
		{}
		Color& operator =(Color color) noexcept
		{
			dword = color.dword;
			return *this;
		}
		constexpr unsigned char GetX() const noexcept
		{
			return dword >> 24u;
		}
		constexpr unsigned char GetA() const noexcept
		{
			return GetX();
		}
		constexpr unsigned char GetR() const noexcept
		{
			return (dword >> 16u) & 0xFFu;
		}
		constexpr unsigned char GetG() const noexcept
		{
			return (dword >> 8u) & 0xFFu;
		}
		constexpr unsigned char GetB() const noexcept
		{
			return dword & 0xFFu;
		}
		void SetX(unsigned char x) noexcept
		{
			dword = (dword & 0xFFFFFFu) | (x << 24u);
		}
		void SetA(unsigned char a) noexcept
		{
			SetX(a);
		}
		void SetR(unsigned char r) noexcept
		{
			dword = (dword & 0xFF00FFFFu) | (r << 16u);
		}
		void SetG(unsigned char g) noexcept
		{
			dword = (dword & 0xFFFF00FFu) | (g << 8u);
		}
		void SetB(unsigned char b) noexcept
		{
			dword = (dword & 0xFFFFFF00u) | b;
		}
	public:
		unsigned int dword;
	};

public:
	class TexException : public Exception
	{
	public:
		TexException(int line, const char* file, std::string note) noexcept;
		TexException(int line, const char* file, std::string note, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		const std::string& GetNote() const noexcept;
		
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		std::string note = {};
		HRESULT hr;
	};
public:
	TextureLoader(unsigned int width, unsigned int height) ;
	TextureLoader(TextureLoader&& source) noexcept = default;
	TextureLoader(TextureLoader&) = delete;
	TextureLoader& operator = (TextureLoader&& donor) noexcept = default;
	TextureLoader& operator = (const TextureLoader&) = delete;
	~TextureLoader() = default;

	void Clear(Color fillValue) noexcept;
	void PutPixel(unsigned int x, unsigned int y, Color c) noexcept;
	Color GetPixel(unsigned int x, unsigned int y) const noexcept;

	unsigned int GetWidth() const noexcept;
	unsigned int GetHeight() const noexcept;

	Color* GetBufferPtr() noexcept;
	const Color* GetBufferPtr() const noexcept;
	const Color* GetBufferPtrConst() const noexcept;

	static TextureLoader FromWICFile(std::string_view name);
	static TextureLoader FromDDSFile(std::string_view name);

	void Save(const std::string& filename) const;
	
	
	bool AlphaLoaded() const noexcept;
private:
	TextureLoader(DirectX::ScratchImage scratch) noexcept;
private:
	static constexpr DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
	DirectX::ScratchImage scratch;
};