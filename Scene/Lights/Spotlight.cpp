#include "Spotlight.h"

#include "src/thirdParty/ImGUI/imgui.h"
#include "System/Painter.h"
#include "Scene/Camera.h"
#include "System/GlobalCBuffer.h"

//////////////////////// SpotLight ///////////////////////


Spotlights::Spotlight::Spotlight(Rendering & renderer)
	:
	/*pixelcbuf(renderer, 3),
	vsCBuffer(renderer, 1u),
	shadowMap{ renderer, width, height },*/
	lightBillboard{ renderer, {0,0,0}, "assets\\textures\\billboards\\spotLight.png", 7.f }
{
	//cbdata.width = width;
	//cbdata.height = height;
	//matProjection = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(75.f), cbdata.width / cbdata.height, 9.1f, 3000.f);
	////matProjection = DirectX::XMMatrixOrthographicLH(cbdata.width, cbdata.height, 0.f, 100000.f);
	//vertexShaderShadowMap = VertexShader::Resolve(renderer, "cso\\Model_VS(ShadowMap)");
	//pixelShaderShadowMap = PixelShader::Resolve(renderer, "cso\\Model_PS(DepthOnly)");
	//cookieTexture = Texture::Resolve(renderer, cookieTexturePath);

	auto& data = GlobalCBuffer::GetData(renderer);
	id = data.numOfSpotLights;
	data.numOfSpotLights++;

}

void Spotlights::Spotlight::ShowControlWindow(Rendering& renderer)
{
	
	auto& data = GlobalCBuffer::GetData(renderer);

	auto& desc = data.spotLights.at(id);

	ImGui::Checkbox("To Cam", &bAlignedToCamPos);

	ImGui::SliderFloat("Cutoff", &desc.cutoff, 0.f, 1.5f);
	ImGui::SliderFloat("InnerCutoff", &desc.InnerCutoff, 0.f, 1.5f);
	ImGui::SliderFloat("Power", &desc.spotlightPower, -2.f, 2.f);
	ImGui::SliderFloat("Const Attenuation", &desc.attConst, 0.f, 1.f);
	ImGui::SliderFloat("Att Linear", &desc.attLin, 0.f, 0.1f, "%.6f");
	ImGui::SliderFloat("Att Quadratic", &desc.attQuad, 0.f, 0.01f, "%.6f");
	
	
}

void Spotlights::Spotlight::Draw(Rendering& renderer)
{
	lightBillboard.Draw(renderer);

	if(cookieTexture)
	cookieTexture->Bind(renderer, 5u);
}

void Spotlights::Spotlight::OnUpdate(Rendering& renderer)
{
	if (bAlignedToCamPos)
	{
		xmPosition = Painter::GetMainCamera()->GetPositionXM();
		xmDirection = Painter::GetMainCamera()->GetDirectionXM();
	}
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, xmPosition);

	lightBillboard.SetPosition(renderer, pos);

	auto& data = GlobalCBuffer::GetData(renderer);

	auto& desc = data.spotLights.at(id);
	//TODO dir W
	DirectX::XMStoreFloat4(&desc.directionViewSpace, DirectX::XMVector4Transform(xmDirection, Painter::GetMainCamera()->GetViewMatrix()));

	DirectX::XMStoreFloat3(&desc.positionViewSpace, DirectX::XMVector3Transform(xmPosition, Painter::GetMainCamera()->GetViewMatrix()));

	/*if (cookieTexture)
		cookieTexture->Bind(renderer, 5u);*/
}


void Spotlights::ShowControlWindow(Rendering & renderer)
{
	if (ImGui::Begin("AllSpotlights"))
	{

		if (ImGui::Button("AddNew"))
		{
			AddNew(renderer);
		}

		ImGui::Columns(2, nullptr, true);

		for (int i = 0; i < allSpotlights.size(); i++)
		{
			if (ImGui::Selectable(std::to_string(allSpotlights.at(i).id).c_str(), selectedIndex == i))
				selectedIndex = i;
		}

		ImGui::NextColumn();

		if (selectedIndex < allSpotlights.size())
		{
			allSpotlights.at(selectedIndex).ShowControlWindow(renderer);
		}
	}

	ImGui::End();
}

void Spotlights::AddNew(Rendering & renderer)
{
	auto& data = GlobalCBuffer::GetData(renderer);
	if (data.numOfSpotLights < MaxNumberOfLights)
		allSpotlights.emplace_back(renderer);
}

void Spotlights::Draw(Rendering & renderer)
{
	for (auto& light : allSpotlights)
	{
		light.Draw(renderer);
	}
}

void Spotlights::OnUpdate(Rendering & renderer)
{
	for (auto& light : allSpotlights)
	{
		light.OnUpdate(renderer);
	}
}

void Spotlights::WriteToFile()
{
	FileIO::WriteToFile ifile("assets\\bin\\spotLights.bin");

	auto& data = GlobalCBuffer::GetData(Rendering::GetCurrent());


	ifile.Write(&data.numOfSpotLights, sizeof(UINT));

	for (UINT i = 0; i < data.numOfSpotLights; i++)
	{
		auto& light = data.spotLights[i];

		DirectX::XMFLOAT3 pos;
		DirectX::XMStoreFloat3(&pos, allSpotlights[i].xmPosition);
		ifile.Write(&pos, sizeof(DirectX::XMFLOAT3));

		DirectX::XMFLOAT4 dir;
		DirectX::XMStoreFloat4(&dir, allSpotlights[i].xmDirection);
		ifile.Write(&dir, sizeof(DirectX::XMFLOAT4));

		ifile.Write(&light.cutoff, sizeof(float));
		ifile.Write(&light.InnerCutoff, sizeof(float));

		ifile.Write(&light.spotlightPower, sizeof(float));

		ifile.Write(&light.attConst, sizeof(float));
		ifile.Write(&light.attLin, sizeof(float));
		ifile.Write(&light.attQuad, sizeof(float));
	}
}

void Spotlights::ReadFromFile()
{
	FileIO::ReadFromFile ofile("assets\\bin\\spotLights.bin");

	auto& data = GlobalCBuffer::GetData(Rendering::GetCurrent());

	UINT numOfLights = 0;

	ofile.Read(&numOfLights, sizeof(UINT));

	for (UINT i = 0; i < numOfLights; i++)
	{
		auto& light = data.spotLights[i];

		allSpotlights.emplace_back(Rendering::GetCurrent());

		DirectX::XMFLOAT3 pos;
		ofile.Read(&pos, sizeof(DirectX::XMFLOAT3));
		allSpotlights[i].xmPosition = DirectX::XMLoadFloat3(&pos);

		DirectX::XMFLOAT4 dir;
		ofile.Read(&dir, sizeof(DirectX::XMFLOAT4));
		allSpotlights[i].xmDirection = DirectX::XMLoadFloat4(&dir);
			  
		ofile.Read(&light.cutoff, sizeof(float));
		ofile.Read(&light.InnerCutoff, sizeof(float));
			  
		ofile.Read(&light.spotlightPower, sizeof(float));
			  
		ofile.Read(&light.attConst, sizeof(float));
		ofile.Read(&light.attLin, sizeof(float));
		ofile.Read(&light.attQuad, sizeof(float));
	}
}




















//
//void Spotlight::BindForShadowMap(Rendering & renderer)
//{
//	shadowMap.Bind(renderer);
//
//	if (alignToCamPos)
//	{
//		auto posT = Painter::GetMainCamera()->GetPosition();
//		auto dir = Painter::GetMainCamera()->GetDirection();
//		//transform.position = { posT.x + dir.x - 2.0f, posT.y + dir.y - 200.f, posT.z - 10.1f };
//		cbdata.direction = { dir.x, dir.y, dir.z, 0.f };
//		cbdata.position = { posT.x + dir.x - 2.0f, posT.y + dir.y - 4.f, posT.z };
//		auto front = DirectX::XMLoadFloat4(&cbdata.direction);
//		auto pos = DirectX::XMLoadFloat3(&cbdata.position);
//		auto right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(front, DirectX::g_XMIdentityR1));
//		auto up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, front));
//
//		matView = DirectX::XMMatrixLookAtLH(
//			pos,
//			DirectX::XMVectorAdd(pos, front),
//			up
//		);
//		//matView = renderer.GetCamera()->GetViewMatrix();
//	}
//	else
//	{
//		auto front = DirectX::XMLoadFloat4(&cbdata.direction);
//		auto pos = DirectX::XMLoadFloat3(&cbdata.position);
//		auto right = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(front, DirectX::g_XMIdentityR1));
//		auto up = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(right, front));
//
//		matView = DirectX::XMMatrixLookAtLH(
//			pos,
//			DirectX::XMVectorAdd(pos, front),
//			up
//		);
//	}
//
//	vsCBufferData.spotLightVP = DirectX::XMMatrixTranspose(
//		DirectX::XMMatrixMultiply(matView, matProjection)
//	);
//
//	vsCBuffer.Update(renderer, vsCBufferData);
//	vsCBuffer.Bind(renderer);
//
//	vertexShaderShadowMap->Bind(renderer);
//	pixelShaderShadowMap->Bind(renderer);
//
//}
