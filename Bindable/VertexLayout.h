#pragma once

#include <vector>

#include <DirectXMath.h>
#include <type_traits>
#include <d3d11.h>

#include "src/System/Utilities/FileIO.h"


namespace VertexTypes
{
	struct BGRAColor
	{
		unsigned char r, g, b;
		unsigned char a = 255;
	};

	struct Weights
	{
		uint8_t weight[4];
	};

	struct BoneIndices
	{
		UINT8 index[4];
	};
}
	
class VertexLayout
{

public:
	enum AttribType
	{
		Position2D,
		Position3D,
		Position3DPlusIndex,
		Texture2D,
		Normal,
		Tangent,
		Bitangent,
		Float3Color,
		Float4Color,
		BGRAColor,
		BoneIndices,
		Weights
	};

	template<AttribType Type>
	struct Map;

	template<>
	struct Map<Position2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* semantic = "Position2D";
		static constexpr const char* code = "P2";
	};

	template<>
	struct Map<Position3DPlusIndex>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr const char* semantic = "Position";
		static constexpr const char* code = "P31";
	};

	template<>
	struct Map<Position3D>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Position";
		static constexpr const char* code = "P3";
	};

	template<>
	struct Map<Texture2D>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
		static constexpr const char* semantic = "TexCoord";
		static constexpr const char* code = "T2";
	};

	template<>
	struct Map<Normal>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Normal";
		static constexpr const char* code = "N";
	};

	template<>
	struct Map<Tangent>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Tangent";
		static constexpr const char* code = "T";
	};

	template<>
	struct Map<Bitangent>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Bitangent";
		static constexpr const char* code = "BT";
	};

	template<>
	struct Map<Float3Color>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
		static constexpr const char* semantic = "Color";
		static constexpr const char* code = "C3";
	};

	template<>
	struct Map<Float4Color>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr const char* semantic = "Color";
		static constexpr const char* code = "C4";
	};

	template<>
	struct Map<BGRAColor>
	{
		using SysType = VertexTypes::BGRAColor;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		static constexpr const char* semantic = "Color";
		static constexpr const char* code = "CU";
	};

	template<>
	struct Map<BoneIndices>
	{
		using SysType = VertexTypes::BoneIndices;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UINT;
		static constexpr const char* semantic = "BoneIndices";
		static constexpr const char* code = "BI";
	};

	template<>
	struct Map<Weights>
	{
		using SysType = VertexTypes::Weights;
		static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;//DXGI_FORMAT_R32G32B32A32_FLOAT;
		static constexpr const char* semantic = "Weights";
		static constexpr const char* code = "BW";
	};


	class VertexAttributeDesc
	{
	public:

		VertexAttributeDesc() = default;

		VertexAttributeDesc(AttribType type, size_t offset, UINT inputSlot)
			:type(type), offset(offset), inputSlot(inputSlot)
		{

		}

		size_t GetOffsetAfter() const noexcept
		{
			return offset + Size();
		}
		size_t GetOffset() const
		{
			return offset;
		}
		size_t Size() const noexcept
		{
			return SizeOf(type);
		}

		static constexpr size_t SizeOf(AttribType type) noexcept
		{
			using namespace DirectX;
			switch (type)
			{
			case Position2D:
				return sizeof(Map<Position2D>::SysType);
				break;
			case Position3D:
				return sizeof(Map<Position3D>::SysType);
				break;
			case Position3DPlusIndex:
				return sizeof(Map<Position3DPlusIndex>::SysType);
				break;
			case Texture2D:
				return sizeof(Map<Texture2D>::SysType);
				break;
			case Normal:
				return sizeof(Map<Normal>::SysType);
				break;
			case Tangent:
				return sizeof(Map<Tangent>::SysType);
				break;
			case Bitangent:
				return sizeof(Map<Bitangent>::SysType);
				break;
			case Float3Color:
				return sizeof(Map<Float3Color>::SysType);
				break;
			case Float4Color:
				return sizeof(Map<Float4Color>::SysType);
				break;
			case BGRAColor:
				return sizeof(Map<BGRAColor>::SysType);
				break;
			case Weights:
				return sizeof(Map<Weights>::SysType);
				break;
			case BoneIndices:
				return sizeof(Map<BoneIndices>::SysType);
				break;
			}
			assert("Invalid element type" && false);
			return 0u;
		}

		AttribType GetType() const noexcept
		{
			return type;
		}

		template<AttribType Type>
		static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset, UINT inputSlot)
		{
			return
			{ Map<Type>::semantic, 0, Map<Type>::dxgiFormat, inputSlot, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}

		D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept
		{
			switch (type)
			{
			case Position2D:
				return GenerateDesc<Position2D>(GetOffset(), inputSlot);
			case Position3D:
				return GenerateDesc<Position3D>(GetOffset(), inputSlot);
			case Position3DPlusIndex:
				return GenerateDesc<Position3DPlusIndex>(GetOffset(), inputSlot);
			case Texture2D:
				return GenerateDesc<Texture2D>(GetOffset(), inputSlot);
			case Normal:
				return GenerateDesc<Normal>(GetOffset(), inputSlot);
			case Tangent:
				return GenerateDesc<Tangent>(GetOffset(), inputSlot);
			case Bitangent:
				return GenerateDesc<Bitangent>(GetOffset(), inputSlot);
			case Float3Color:
				return GenerateDesc<Float3Color>(GetOffset(), inputSlot);
			case Float4Color:
				return GenerateDesc<Float4Color>(GetOffset(), inputSlot);
			case BGRAColor:
				return GenerateDesc<BGRAColor>(GetOffset(), inputSlot);
			case BoneIndices:
				return GenerateDesc<BoneIndices>(GetOffset(), inputSlot);
			case Weights:
				return GenerateDesc<Weights>(GetOffset(), inputSlot);
			}
			assert("Invalid attrib type" && false);
			return { "INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		}

		const char* GetCode() const noexcept
		{
			switch (type)
			{
			case Position2D:
				return Map<Position2D>::code;
			case Position3D:
				return Map<Position3D>::code;
			case Texture2D:
				return Map<Texture2D>::code;
			case Normal:
				return Map<Normal>::code;
			case Tangent:
				return Map<Tangent>::code;
			case Bitangent:
				return Map<Bitangent>::code;
			case Float3Color:
				return Map<Float3Color>::code;
			case Float4Color:
				return Map<Float4Color>::code;
			case BGRAColor:
				return Map<BGRAColor>::code;
			}
			assert("Invalid attrib type" && false);
			return  "INVALID";
		}

	private:
		AttribType type;
		size_t offset;
		UINT8 inputSlot;
	};

	//---------------------------------------//-----------------------------------------------//

public:
	VertexLayout() = default;

	VertexLayout(std::initializer_list<AttribType> attributes, UINT inputSlot = 0)
	{
		for (auto& type : attributes)
		{
			attributesDescs.emplace_back(type, SizeBytes(), inputSlot);
		}
	}

	template <AttribType Type>
	const VertexAttributeDesc& GetDescByType() const noexcept
	{
		for (auto& desc : attributesDescs)
		{
			if (desc.GetType() == Type)
			{
				return desc;
			}
		}
		assert("could not find matching atrrib type" && false);
		return attributesDescs.front();
	}

	const VertexAttributeDesc& GetDescByIndex(size_t i) const noexcept
	{
		return attributesDescs.at(i);
	}

	std::string GetCode() const noexcept
	{
		std::string nullstr;

		for (auto& desc : attributesDescs)
		{
			nullstr += desc.GetCode();
		}
		return nullstr;

	}

	const std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;

		desc.reserve(GetAttribCount());
		for (const auto& a : attributesDescs)
		{
			desc.push_back(a.GetDesc());
		}
		return desc;

	}

	VertexLayout& Append(AttribType type, UINT streamSlot) noexcept
	{
		attributesDescs.emplace_back(type, SizeBytes(), streamSlot);
		return *this;
	}
	VertexLayout& Append(std::initializer_list<AttribType> types, UINT streamSlot) noexcept
	{
		for (auto& type : types)
		{
			attributesDescs.emplace_back(type, SizeBytes(), streamSlot);
		}
		return *this;
	}

	size_t SizeBytes() const noexcept
	{
		return attributesDescs.empty() ? 0u : attributesDescs.back().GetOffsetAfter();
	}

	size_t GetAttribCount() const noexcept
	{
		return attributesDescs.size();
	}

	const std::vector<VertexAttributeDesc>& GetAttributes() const noexcept
	{
		return attributesDescs;
	}

	void WriteToFile(FileIO::WriteToFile& file);

	void ReadFromFile(FileIO::ReadFromFile& file);

private:
	std::vector<VertexAttributeDesc> attributesDescs;
};

