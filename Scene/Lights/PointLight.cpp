#include "PointLight.h"
#include "System/Painter.h"

#include "System/GlobalCBuffer.h"


PointLights::PointLight::PointLight(Rendering & renderer)
	:
	/*gsCBuffer{ renderer, 2u },
	shadowMap{ renderer, 1000, 1000 }*/
	lightBillboard{ renderer, {0.f,0.f,0.f}, "assets\\textures\\billboards\\pointLight.png", 5.f }
{

	/*vsShadowMap = VertexShader::Resolve(renderer, "cso\\Model_VS(OmniShadowMap)");
	gsShadowMap = GeometryShader::Resolve(renderer, "cso\\OmniShadowMap_GS");
	psShadowMap = PixelShader::Resolve(renderer, "cso\\Model_PS(DepthLengthOnly)");*/

	auto& data = GlobalCBuffer::GetData(renderer);
	id = data.numOfPointLights;
	data.numOfPointLights++;

}

void PointLights::PointLight::ShowControlWindow(Rendering& renderer)
{
	
	auto& data = GlobalCBuffer::GetData(renderer);

	auto& desc = data.pointLights.at(id);

	ImGui::Checkbox("AlignToCamPos", &bAlignedToCamPos);

	ImGui::ColorEdit3("Color", &desc.color.x);
	ImGui::SliderFloat("Const Attenuation", &desc.attConst, 0.f, 1.f);
	ImGui::SliderFloat("Att Linear", &desc.attLin, 0.f, 0.1f, "%.5f");
	ImGui::SliderFloat("Att Quadratic", &desc.attQuad, 0.f, 0.01f, "%.6f");
	if (ImGui::Button("Reset")) {
		Reset();
	}
		
}

void PointLights::PointLight::Reset() noexcept
{
	
}

void PointLights::PointLight::Draw(Rendering & renderer)
{
	
	lightBillboard.Draw(renderer);

}

void PointLights::PointLight::OnUpdate(Rendering & renderer)
{

	if (bAlignedToCamPos)
	{
		xmPosition = DirectX::XMVectorAdd(Painter::GetMainCamera()->GetPositionXM(), DirectX::XMVectorScale(Painter::GetMainCamera()->GetDirectionXM(), 100));
	}

	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, xmPosition);

	lightBillboard.SetPosition(renderer, pos);

	auto& data = GlobalCBuffer::GetData(renderer);

	auto& desc = data.pointLights.at(id);

	desc.positionWorldSpace = pos;
	DirectX::XMStoreFloat3(&desc.positionViewSpace, DirectX::XMVector3Transform(xmPosition, PointOfView::GetMainCamera().GetViewMatrix()));


}

void PointLights::ShowControlWindow(Rendering & renderer)
{
	if (ImGui::Begin("AllPointLights"))
	{

		if (ImGui::Button("AddNew"))
		{
			AddNew(renderer);
		}

		ImGui::Columns(2, nullptr, true);

		for (int i = 0; i < allPointLights.size(); i++)
		{
			if (ImGui::Selectable(std::to_string( allPointLights.at(i).id ).c_str(), selectedIndex == i))
				selectedIndex = i;
		}

		ImGui::NextColumn();

		if (selectedIndex < allPointLights.size())
		{
			allPointLights.at(selectedIndex).ShowControlWindow(renderer);
		}
	}

	ImGui::End();

}

void PointLights::AddNew(Rendering & renderer)
{
	auto& data = GlobalCBuffer::GetData(renderer);
	if (data.numOfPointLights < MaxNumberOfLights)
		allPointLights.emplace_back(renderer);
}

void PointLights::Draw(Rendering & renderer)
{
	for (auto& light : allPointLights)
	{
		light.Draw(renderer);
	}

}

void PointLights::OnUpdate(Rendering & renderer)
{
	for (auto& light : allPointLights)
	{
		light.OnUpdate(renderer);
	}

}

void PointLights::WriteToFile()
{
	FileIO::WriteToFile ifile("assets\\bin\\pointLights.bin");

	auto& data = GlobalCBuffer::GetData(Rendering::GetCurrent());


	ifile.Write(&data.numOfPointLights, sizeof(UINT));

	for (UINT i = 0; i < data.numOfPointLights; i++)
	{
		auto& light = data.pointLights[i];

		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, allPointLights[i].xmPosition);
		ifile.Write(&pos, sizeof(DirectX::XMFLOAT3));

		ifile.Write(&light.color, sizeof(DirectX::XMFLOAT3));
		ifile.Write(&light.colorIntensity, sizeof(float));

		ifile.Write(&light.attConst, sizeof(float));
		ifile.Write(&light.attLin, sizeof(float));
		ifile.Write(&light.attQuad, sizeof(float));
	}
}

void PointLights::ReadFromFile()
{
	FileIO::ReadFromFile ofile("assets\\bin\\pointLights.bin");

	auto& data = GlobalCBuffer::GetData(Rendering::GetCurrent());

	UINT numOfLights = 0;

	ofile.Read(&numOfLights, sizeof(UINT));

	for (UINT i = 0; i < numOfLights; i++)
	{
		auto& light = data.pointLights[i];

		allPointLights.emplace_back(Rendering::GetCurrent());

		DirectX::XMFLOAT3 pos;
		ofile.Read(&pos, sizeof(DirectX::XMFLOAT3));
		allPointLights[i].xmPosition = DirectX::XMLoadFloat3(&pos);

		ofile.Read(&light.color, sizeof(DirectX::XMFLOAT3));
		ofile.Read(&light.colorIntensity, sizeof(float));

		ofile.Read(&light.attConst, sizeof(float));
		ofile.Read(&light.attLin, sizeof(float));
		ofile.Read(&light.attQuad, sizeof(float));
	}

}
