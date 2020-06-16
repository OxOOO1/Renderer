#pragma once



#include "Bindable/RawVertexBuffer.h"
#include "Bindable/BindablesH.h"

#include "Components/BoundingVolumes.h"

class DebugDraw
{
public:

	enum class Color
	{
		WHITE,
		RED,
		BLUE,
		PURPLE,
		ORANGE,
		GRADIENT_ORANGE_RED,
		GRADIENT_RED_PURPLE,
		GRADIENT_ORANGE_BLUE,
		RANDOM
	};

	static void DrawLine(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, DebugDraw::Color color = Color::GRADIENT_ORANGE_RED)
	{
		Get().line.Submit(pos1, pos2, color);
	}

	static void DrawCube(const DirectX::BoundingBox& box, DebugDraw::Color color = Color::ORANGE)
	{
		Cube::BoxInfo info;
		box.GetCorners(info.vertices.data());
		info.color = color;
		Get().cube.Submit(std::move(info));
	}
	static void DrawCube(const DirectX::BoundingOrientedBox& box, DebugDraw::Color color = Color::ORANGE)
	{
		Cube::BoxInfo info;
		box.GetCorners(info.vertices.data());
		info.color = color;
		Get().cube.Submit(std::move(info));
	}
	static void DrawFrustrum(const DirectX::BoundingFrustum& box, DebugDraw::Color color = Color::ORANGE)
	{
		Cube::BoxInfo info;
		box.GetCorners(info.vertices.data());
		info.color = color;
		Get().cube.Submit(std::move(info));
	}
	static void DrawSphere(const DirectX::BoundingSphere& box, DebugDraw::Color color = Color::ORANGE)
	{
		Get().sphere.Draw(box, color);
	}
	static void DrawSphere(const DirectX::XMFLOAT3& center, float radius, DebugDraw::Color color = Color::RED)
	{
		Get().sphere.Draw(center, radius, color);
	}

	static void DrawBatch()
	{
		Get().line.Draw();
		Get().cube.Draw();
	}

private:

	DebugDraw() = default;

	static DebugDraw& Get()
	{
		static DebugDraw renderer;
		return renderer;
	}

	class Primitive
	{
	public:

		Primitive(Topology& rtopology, Rasterizer& rrasterizer)
			:topology(rtopology), rasterizer(rrasterizer)
		{}

		void Bind(Rendering& renderer)
		{
			pVertexShader->Bind(renderer);
			pPixelShader->Bind(renderer);
			topology.Bind(renderer);
			rasterizer.Bind(renderer);
		}

		std::shared_ptr<VertexShader> pVertexShader;
		std::shared_ptr<PixelShader> pPixelShader;
		Topology& topology;
		Rasterizer& rasterizer;
	};

	class Line : private Primitive
	{
	public:

		Line(Rendering& renderer);

		void Submit(const DirectX::XMFLOAT3& pos1, const DirectX::XMFLOAT3& pos2, DebugDraw::Color color);

		void Draw();

	private:

		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			VertexTypes::BGRAColor color;
		};

		std::vector<Vertex> batch;

		void Bind(Rendering& renderer);

	};

	class Cube : private Primitive
	{
	public:

		Cube(Rendering& renderer);

		void Draw();
		struct BoxInfo
		{
			std::array<DirectX::XMFLOAT3, 8> vertices;
			DebugDraw::Color color;
		};

		std::vector<DebugDraw::Color> colorBatch;
		std::vector<std::array<DirectX::XMFLOAT3, 8>> vertBatch;

		void Submit(BoxInfo&& info)
		{
			colorBatch.emplace_back(std::move(info.color));
			vertBatch.emplace_back(std::move(info.vertices));
		}

	private:

		PixelConstBuffer<DirectX::XMFLOAT4> psCBufColor;

		//////////////Bindables////////////

		void Bind(Rendering& renderer);

		IndexBuffer indexBuffer;

		const std::vector<unsigned short> indices =
		{ 0, 1, 2,
		  0, 2, 3,

		  3, 2, 6,
		  3, 6, 7,

		  7, 6, 5,
		  7, 5, 4,

		  4, 5, 1,
		  4, 1, 0,

		  4, 0, 3,
		  4, 3, 7,

		  4, 6, 2,
		  5, 2, 1
		};

	};

	class Sphere : private Primitive
	{

	public:

		Sphere(Rendering& renderer);

		void Draw(const DirectX::BoundingSphere& sphere, DebugDraw::Color color);
		void Draw(const DirectX::XMFLOAT3& center, float radius, DebugDraw::Color color);
	private:

		void DrawSphere(const DirectX::XMFLOAT3& center, float radius);

	private:

		static constexpr size_t c_ringSegments = 64;
		std::array<DirectX::XMFLOAT3, c_ringSegments + 1> vertices;

		//////////////Bindables////////////

		void Bind(Rendering& renderer)
		{
			Primitive::Bind(renderer);
			vertexBuffer.Bind(renderer);
			indexBuffer.Bind(renderer);
		}

		VertexBuffer vertexBuffer;
		IndexBuffer indexBuffer;

		DynVertexBuffer dynVB;
		PixelConstBuffer<DirectX::XMFLOAT4> psCBufColor;
	};

	Line line{ Rendering::GetCurrent() };
	Cube cube{ Rendering::GetCurrent() };
	Sphere sphere{ Rendering::GetCurrent() };

};