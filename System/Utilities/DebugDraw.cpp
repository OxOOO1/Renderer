#include "DebugDraw.h"

#include <random>

DebugDraw::Line::Line(Rendering& renderer)
	: Primitive(Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_LINELIST), Rasterizer::Get(renderer, true))
{
	pVertexShader = VertexShader::Resolve(renderer, "cso\\Line_World_VS", { VertexLayout::Position3D, VertexLayout::BGRAColor });
	pPixelShader = PixelShader::Resolve(renderer, "cso\\IMRender_Line_PS");
}

void DebugDraw::Line::Submit(const DirectX::XMFLOAT3 & pos1, const DirectX::XMFLOAT3 & pos2, DebugDraw::Color color)
{
	VertexTypes::BGRAColor color1;
	VertexTypes::BGRAColor color2;
	switch (color)
	{
	case DebugDraw::Color::WHITE:
		color1 = VertexTypes::BGRAColor{ 255,255,255,255 };
		color2 = VertexTypes::BGRAColor{ 255,255,255,255 };
		break;
	case DebugDraw::Color::RED:
		color1 = VertexTypes::BGRAColor{ 255,50,50,255 };
		color2 = VertexTypes::BGRAColor{ 255,50,50,255 };
		break;
	case DebugDraw::Color::BLUE:
		color1 = VertexTypes::BGRAColor{ 50,50,255,255 };
		color2 = VertexTypes::BGRAColor{ 50,50,255,255 };
		break;
	case DebugDraw::Color::PURPLE:
		color1 = VertexTypes::BGRAColor{ 200,50,200,255 };
		color2 = VertexTypes::BGRAColor{ 200,50,200,255 };
		break;
	case DebugDraw::Color::ORANGE:
		color1 = VertexTypes::BGRAColor{ 255,200,50,255 };
		color2 = VertexTypes::BGRAColor{ 255,200,50,255 };
		break;
	case DebugDraw::Color::GRADIENT_ORANGE_RED:
		color1 = VertexTypes::BGRAColor{ 255,200,50,255 };
		color2 = VertexTypes::BGRAColor{ 255,50,50,255 };
		break;
	case DebugDraw::Color::GRADIENT_RED_PURPLE:
		color1 = VertexTypes::BGRAColor{ 255,50,50,255 };
		color2 = VertexTypes::BGRAColor{ 255,50,200,255 };
		break;
	case DebugDraw::Color::GRADIENT_ORANGE_BLUE:
		color1 = VertexTypes::BGRAColor{ 255,200,50,255 };
		color2 = VertexTypes::BGRAColor{ 50,50,255,255 };
		break;
	default:
		break;
	}
	Vertex v{ pos1, color1 };
	Vertex v2{ pos2, color2 };
	batch.emplace_back(std::move(v));
	batch.emplace_back(std::move(v2));
}

void DebugDraw::Line::Draw()
{
	if (!batch.size()) return;

	auto& Renderer = Rendering::GetCurrent();

	Bind(Renderer);

	auto numofinstances = batch.size();

	Renderer.GetContext()->Draw((UINT)numofinstances, 0u);

	batch.clear();
	batch.reserve(numofinstances);
}

void DebugDraw::Line::Bind(Rendering & renderer)
{
	Primitive::Bind(renderer);
	TVertexBuffer<Vertex>{ renderer, batch }.Bind(renderer);
}

DebugDraw::Cube::Cube(Rendering& renderer)
	:Primitive(Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP), Rasterizer::Get(renderer, false)),
	psCBufColor{ renderer, 1u }
{
	pVertexShader = VertexShader::Resolve(renderer, "cso\\Box_World_VS", { VertexLayout::Position3D});
	pPixelShader = PixelShader::Resolve(renderer, "cso\\IMRender_Box_PS");
	indexBuffer.Create(Rendering::GetCurrent(), indices);
}


void DebugDraw::Cube::Draw()
{
	if (!vertBatch.size()) return;

	auto& Renderer = Rendering::GetCurrent();

	Bind(Renderer);

	VertexBuffer vb;
	vb.CreateConst(Renderer, vertBatch.size() * 8 * sizeof(DirectX::XMFLOAT3), sizeof(DirectX::XMFLOAT3), vertBatch.data());
	vb.Bind(Renderer);
	
	for (int i = 0; i < vertBatch.size(); i++)
	{
		switch (colorBatch.at(i))
		{
		case DebugDraw::Color::WHITE:
			psCBufColor.Update(Renderer, { 1.0f,1.0f,1.0f,1.0f });
			psCBufColor.Bind(Renderer);
			break;
		case DebugDraw::Color::RED:
			psCBufColor.Update(Renderer, { 1.0f,0.2f,0.2f,1.0f });
			psCBufColor.Bind(Renderer);
			break;
		case DebugDraw::Color::BLUE:
			psCBufColor.Update(Renderer, { 0.2f,0.2f,1.0f,1.0f });
			psCBufColor.Bind(Renderer);
			break;
		case DebugDraw::Color::PURPLE:
			psCBufColor.Update(Renderer, { 1.0f,0.2f,1.0f,1.0f });
			psCBufColor.Bind(Renderer);
			break;
		case DebugDraw::Color::ORANGE:
			psCBufColor.Update(Renderer, { 1.0f,0.5f,0.3f,1.0f });
			psCBufColor.Bind(Renderer);
			break;
		default:
			psCBufColor.Update(Renderer, { 1.0f,0.5f,0.5f,1.0f });
			psCBufColor.Bind(Renderer);
			break;
		}
		/*dynVB.Bind(Renderer);
		dynVB.Update(Renderer, v.vertices.data());*/
		
		Renderer.GetContext()->DrawIndexed((UINT)indices.size(), 0, i * 8);
	}

	auto numOfSubmittedBoxes = vertBatch.size();
	vertBatch.clear();
	colorBatch.clear();
	vertBatch.reserve(numOfSubmittedBoxes);
	colorBatch.reserve(numOfSubmittedBoxes);
	numOfSubmittedBoxes = 0;

}

void DebugDraw::Cube::Bind(Rendering & renderer)
{
	Primitive::Bind(renderer);
	indexBuffer.Bind(renderer);
}

///////////////////////////////////////////

DebugDraw::Sphere::Sphere(Rendering& renderer)
	:Primitive(Topology::Get(renderer, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP), Rasterizer::Get(renderer, false)),
	dynVB{ renderer, sizeof(DirectX::XMFLOAT3), vertices.size() * sizeof(DirectX::XMFLOAT3) },
	psCBufColor{ renderer }
{
	pVertexShader = VertexShader::Resolve(renderer, "cso\\Box_World_VS", { VertexLayout::Position3D });
	pPixelShader = PixelShader::Resolve(renderer, "cso\\IMRender_Box_PS");
}

void DebugDraw::Sphere::Draw(const DirectX::BoundingSphere & sphere, DebugDraw::Color color)
{
	Rendering& Renderer = Rendering::GetCurrent();

	Bind(Renderer);

	switch (color)
	{
	case DebugDraw::Color::WHITE:
		psCBufColor.Update(Renderer, { 1.0f,1.0f,1.0f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::RED:
		psCBufColor.Update(Renderer, { 1.0f,0.2f,0.2f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::BLUE:
		psCBufColor.Update(Renderer, { 0.2f,0.2f,1.0f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::PURPLE:
		psCBufColor.Update(Renderer, { 1.0f,0.2f,1.0f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::ORANGE:
		psCBufColor.Update(Renderer, { 1.0f,0.5f,0.3f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	default:
		psCBufColor.Update(Renderer, { 1.0f,0.5f,0.5f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	}

	dynVB.Bind(Rendering::GetCurrent());

	DrawSphere(sphere.Center, sphere.Radius);


}

void DebugDraw::Sphere::Draw(const DirectX::XMFLOAT3 & center, float radius, DebugDraw::Color color)
{
	Rendering& Renderer = Rendering::GetCurrent();

	Bind(Renderer);

	switch (color)
	{
	case DebugDraw::Color::WHITE:
		psCBufColor.Update(Renderer, { 1.0f,1.0f,1.0f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::RED:
		psCBufColor.Update(Renderer, { 1.0f,0.2f,0.2f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::BLUE:
		psCBufColor.Update(Renderer, { 0.2f,0.2f,1.0f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::PURPLE:
		psCBufColor.Update(Renderer, { 1.0f,0.2f,1.0f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	case DebugDraw::Color::ORANGE:
		psCBufColor.Update(Renderer, { 1.0f,0.5f,0.3f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	default:
		psCBufColor.Update(Renderer, { 1.0f,0.5f,0.5f,1.0f });
		psCBufColor.Bind(Renderer);
		break;
	}

	dynVB.Bind(Rendering::GetCurrent());

	DrawSphere(center, radius);
}

void DebugDraw::Sphere::DrawSphere(const DirectX::XMFLOAT3 & center, float radius)
{
	DirectX::XMVECTOR origin = DirectX::XMLoadFloat3(&center);

	DirectX::XMVECTOR xaxis = DirectX::XMVectorScale(DirectX::g_XMIdentityR0, radius);
	DirectX::XMVECTOR yaxis = DirectX::XMVectorScale(DirectX::g_XMIdentityR1, radius);
	DirectX::XMVECTOR zaxis = DirectX::XMVectorScale(DirectX::g_XMIdentityR2, radius);

	const FLOAT dAngle = DirectX::XM_2PI / float(c_ringSegments);

	float angle = 0.f;

	for (int i = 0; i < c_ringSegments; i++)
	{
		DirectX::XMFLOAT3 pos;
		pos.x = DirectX::XMScalarCos(angle) * radius;
		pos.z = DirectX::XMScalarSin(angle) * radius;
		pos.y = 0.0f;

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(origin, DirectX::XMLoadFloat3(&pos)));

		vertices.at(i) = pos;

		angle += dAngle;

	}
	vertices.at(c_ringSegments) = vertices.at(0);

	angle = 0.f;

	dynVB.Update(Rendering::GetCurrent(), vertices.data());

	Rendering::GetCurrent().GetContext()->Draw((UINT)vertices.size(), 0);

	for (int i = 0; i < c_ringSegments; i++)
	{
		DirectX::XMFLOAT3 pos;
		pos.z = DirectX::XMScalarCos(angle) * radius;
		pos.y = DirectX::XMScalarSin(angle) * radius;
		pos.x = 0.0f;

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(origin, DirectX::XMLoadFloat3(&pos)));

		vertices.at(i) = pos;

		angle += dAngle;

	}
	vertices.at(c_ringSegments) = vertices.at(0);
	angle = 0.f;
	dynVB.Update(Rendering::GetCurrent(), vertices.data());

	Rendering::GetCurrent().GetContext()->Draw((UINT)vertices.size(), 0);

	//DrawRingXY

	for (int i = 0; i < c_ringSegments; i++)
	{
		DirectX::XMFLOAT3 pos;
		pos.x = DirectX::XMScalarCos(angle) * radius;
		pos.y = DirectX::XMScalarSin(angle) * radius;
		pos.z = 0.0f;

		DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(origin, DirectX::XMLoadFloat3(&pos)));

		vertices.at(i) = pos;

		angle += dAngle;

	}
	vertices.at(c_ringSegments) = vertices.at(0);

	dynVB.Update(Rendering::GetCurrent(), vertices.data());

	Rendering::GetCurrent().GetContext()->Draw((UINT)vertices.size(), 0);


}




