#pragma once
#include "VertexLayout.h"

#include "src/System/Utilities/FileIO.h"

class Vertex
{
	friend class RawVertexBuffer;
public:
	
	template<VertexLayout::AttribType Type>
	auto& GetAttribute() noexcept
	{
		auto pAttribute = pData + layout.GetDescByType<Type>().GetOffset();
		return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
	}

	template<VertexLayout::AttribType DestLayoutType, typename SrcType>
	void SetAttribute(char* pAttribute, SrcType&& val) noexcept
	{
		using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
		if constexpr (std::is_assignable<Dest, SrcType>::value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
		{
			assert(false);
		}
	}

	template<typename T>
	void SetAttributeByIndex(size_t i, T&& data) noexcept
	{
		using namespace DirectX;
		const auto& attribDesc = layout.GetDescByIndex(i);
		auto pAttribute = pData + attribDesc.GetOffset();
		switch (attribDesc.GetType())
		{
		case VertexLayout::Position2D:
			SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Position3D:
			SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Texture2D:
			SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Normal:
			SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Tangent:
			SetAttribute<VertexLayout::Tangent>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Bitangent:
			SetAttribute<VertexLayout::Bitangent>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Float3Color:
			SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Float4Color:
			SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::Weights:
			SetAttribute<VertexLayout::Weights>(pAttribute, std::forward<T>(data));
			break;
		case VertexLayout::BoneIndices:
			SetAttribute<VertexLayout::BoneIndices>(pAttribute, std::forward<T>(data));
			break;
		default:
			assert("Bad element type" && false);
		}
	}

	template<typename First, typename ...Rest>
	void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept
	{
		SetAttributeByIndex(i, std::forward<First>(first));
		SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
	}

protected:

	Vertex(char* pData, VertexLayout& layout)
		:pData(pData), layout(layout)
	{
		assert(pData != nullptr && "Corrupted data provided");
	}
	
private:
	char* pData = nullptr;
	const VertexLayout& layout;
};

class ConstVertex
{
public:
	ConstVertex(const Vertex& vertex) noexcept
		: vertex(vertex)
	{
	}

	template<VertexLayout::AttribType Type>
	const auto& GetAttribute() const noexcept
	{
		return const_cast<Vertex&>(vertex).GetAttribute<Type>();
	}
private:
	Vertex vertex;
};


class RawVertexBuffer
{
	
public:

	RawVertexBuffer(const VertexLayout& layout) noexcept
		:mVertexLayout(layout)
	{}
	const VertexLayout& GetVertexLayout() const noexcept
	{
		return mVertexLayout;
	}
	void SetVertexLayout(VertexLayout layout) noexcept
	{
		mVertexLayout = layout;
	}
	size_t NumOfVertices() const noexcept
	{
		return buffer.size() / mVertexLayout.SizeBytes();
	}

	size_t SizeBytes() const noexcept
	{
		return buffer.size();
	}

	template<typename ...Params>
	void EmplaceBackVertex(Params&&... params) noexcept
	{
		assert(sizeof...(params) == mVertexLayout.GetAttribCount() && "Param count doesn't match num of vertex elements");

		buffer.resize(buffer.size() + mVertexLayout.SizeBytes());
		BackVertex().SetAttributeByIndex(0u, std::forward<Params>(params)...);
	}

	template<VertexLayout::AttribType DestType, typename Src>
	void EmplaceBackAttribute(Src&& src)
	{
		using Dest = typename VertexLayout::Map<DestType>::SysType;

		constexpr auto attribSize = sizeof(Dest);
		
		buffer.resize(buffer.size() + attribSize);
		char* back = BackPtr(attribSize);

		assert(attribSize == sizeof(Dest));
		if constexpr (std::is_assignable<Dest, Src>::value)
		{
			*reinterpret_cast<Dest*>(back) = src;
		}
		else{
			assert(false && "Attrib types mismatch");
		}

	}

	Vertex BackVertex() noexcept
	{
		assert(buffer.size() != 0u);

		return Vertex{ buffer.data() + buffer.size() - mVertexLayout.SizeBytes(), mVertexLayout };
	}
	char* BackPtr(size_t reverseOffset) noexcept
	{
		assert(buffer.size() != 0u);
		return (buffer.data() + buffer.size() - reverseOffset);
	}
	Vertex Front() noexcept
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(), mVertexLayout };
	}

	Vertex operator[](size_t i) noexcept
	{
		assert(i < NumOfVertices());
		return Vertex{ buffer.data() + mVertexLayout.SizeBytes() * i, mVertexLayout };
	}

	const char* GetDataConst() const noexcept
	{
		return buffer.data();
	}

	char* GetData() noexcept
	{
		return buffer.data();
	}

	std::vector<char>& GetBuffer()
	{
		return buffer;
	}

	void WriteToFile(FileIO::WriteToFile& file);

	void ReadFromFile(FileIO::ReadFromFile& file);

private:
	std::vector<char> buffer;
	VertexLayout mVertexLayout;
};

	
