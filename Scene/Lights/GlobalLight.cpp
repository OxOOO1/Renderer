#include "GlobalLight.h"
#include "src/thirdParty/ImGUI/imgui.h"
#include "System/Painter.h"
#include "Scene/Camera.h"
//#include "System/GlobalCBuffer.h"



//////////////////////////////Global Light//////////////////////////////////////////

GlobalLightCascade::GlobalLightCascade(Rendering & renderer, UINT shadowMapSize, float size)
	:
	shadowMap{ renderer, shadowMapSize, shadowMapSize },
	PointOfView(false, renderer, size, size, 0.f, size)
{

	radius = size / 2.f;

	auto& data = GlobalCBuffer::GetData(renderer);
	data.shadowMapWidth = shadowMapSize;
	data.shadowMapHeight = shadowMapSize;

	DirectX::XMStoreFloat4(&fOrientation, DirectX::XMQuaternionIdentity());

	//OnUpdate(renderer);

}

void GlobalLightCascade::OnUpdate(Rendering& renderer, const DirectX::XMVECTOR& direction, const Camera& mainCamera, DirectX::XMVECTOR& frustPos)
{

	xmDirection = direction;
	auto& camDir = mainCamera.GetDirectionXM();
	

	frustPos = DirectX::XMVectorAdd(frustPos, DirectX::XMVectorScale(camDir, radius));

	DirectX::XMFLOAT4 cascadePos;
	DirectX::XMStoreFloat4(&cascadePos, DirectX::XMVectorSubtract(frustPos, DirectX::XMVectorScale(xmDirection, radius)));
	if (cascadePos.y < radius) cascadePos.y = radius;

	//Texel snapping
	float texelsPerUnit = shadowMap.width / (radius * 2.f);
	DirectX::XMMATRIX scalar = DirectX::XMMatrixScaling(texelsPerUnit, texelsPerUnit, texelsPerUnit);

	auto lookAt = DirectX::XMMatrixLookAtLH(DirectX::XMVECTOR{}, DirectX::XMVectorNegate(xmDirection), DirectX::g_XMIdentityR1);

	lookAt = DirectX::XMMatrixMultiply(scalar, lookAt);

	cascadePos.w = 1.f;

	DirectX::XMStoreFloat4(&cascadePos, DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&cascadePos), lookAt));

	cascadePos.x = (float)floorf(cascadePos.x);
	cascadePos.y = (float)floorf(cascadePos.y);
	cascadePos.z = (float)floorf(cascadePos.z);

	xmPosition = DirectX::XMVector4Transform(DirectX::XMLoadFloat4(&cascadePos), DirectX::XMMatrixInverse(nullptr, lookAt));


	UpdateViewMatrix(renderer);



	frustPos = DirectX::XMVectorAdd(frustPos, DirectX::XMVectorScale(camDir, radius));

}

void GlobalLightCascade::BindShadowMap(Rendering & renderer)
{
	shadowMap.Bind(renderer);
}

void GlobalLightCascade::ShowControlWindow(Rendering& renderer)
{


}

GlobalLight::GlobalLight(Rendering& renderer, Camera& mainCamera)
	:mesh{ renderer, fPosition, "assets\\textures\\billboards\\moon.png", 500.f }
{
	DirectX::XMStoreFloat4(&fOrientation, DirectX::XMQuaternionIdentity());
	xmDirection = DirectX::XMVectorSet(0.0f, -0.8f, 0.5f, 0.0f);
	xmPosition = DirectX::XMVectorSet(500.f, 17000.f, -15000.f, 0.f);
	DirectX::XMStoreFloat(&lightPositionOffset, DirectX::XMVector3Length(xmPosition));

	std::array<DirectX::XMFLOAT3, 8> corners;

	//Frust 1
	corners = mainCamera.frustrumNear.GetCorners();
	box.Init(8, corners.data(), sizeof(DirectX::XMFLOAT3));
	float radius;
	DirectX::XMStoreFloat(&radius, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&box.dAABB.Extents)));
	shadowNear = std::make_unique<GlobalLightCascade>(renderer, 2048, radius * 2.f);

	//Frust 2
	corners = mainCamera.frustrumMid.GetCorners();
	box2.Init(8, corners.data(), sizeof(DirectX::XMFLOAT3));
	float radius2;
	DirectX::XMStoreFloat(&radius2, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&box2.dAABB.Extents)));
	shadowMid = std::make_unique<GlobalLightCascade>(renderer, 1024, radius2 * 2.f);

	//Frust 3
	corners = mainCamera.frustrumFar.GetCorners();
	box3.Init(8, corners.data(), sizeof(DirectX::XMFLOAT3));
	float radius3;
	DirectX::XMStoreFloat(&radius3, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&box3.dAABB.Extents)));
	shadowFar = std::make_unique<GlobalLightCascade>(renderer, 512, radius3 * 2.f);

	OnUpdate(renderer, mainCamera);

}

void GlobalLight::ShowControlWindow(Rendering & renderer)
{
	if (ImGui::Begin("GlobalLight"))
	{

		auto& data = GlobalCBuffer::GetData(renderer);

		ImGui::ColorEdit3("LightColor", &data.globalLightColor.x);
		ImGui::DragFloat("LightIntensity", &data.globalLightIntensity, 0.001f);
		ImGui::SliderFloat("AmbientLightIntensity", &data.ambientLightIntensity, 0.f, 0.5f);

		ImGui::SliderFloat("PositionOffset", &lightPositionOffset, 1.f, 50000.f);
		ImGui::SliderAngle("Pitch", &pitchYawRoll.z, 0, 360);
		ImGui::SliderAngle("Yaw", &pitchYawRoll.y, 0, 360);
		ImGui::SliderAngle("Roll", &pitchYawRoll.x, 0, 360);

		lightRotation = DirectX::XMQuaternionRotationRollPitchYaw(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);


	}
	ImGui::End();
}

void GlobalLight::OnUpdate(Rendering & renderer, Camera & mainCamera)
{

	auto unitVector = DirectX::XMVector3Rotate(DirectX::g_XMIdentityR0, lightRotation);
	xmPosition = DirectX::XMVectorScale(unitVector, lightPositionOffset);
	xmDirection = DirectX::XMVectorNegate(DirectX::XMVector3Normalize(xmPosition));

	DirectX::XMStoreFloat3(&fPosition, xmPosition);
	DirectX::XMStoreFloat3(&fDirection, xmDirection);

	auto frustPos = DirectX::XMLoadFloat3(&mainCamera.frustrum.dBF.Origin);

	shadowNear->OnUpdate(renderer, xmDirection, mainCamera, frustPos);
	shadowMid->OnUpdate(renderer, xmDirection, mainCamera, frustPos);
	shadowFar->OnUpdate(renderer, xmDirection, mainCamera, frustPos);


	auto& data = GlobalCBuffer::GetData(renderer);
	DirectX::XMStoreFloat3(&data.globalLightDirectionWorld, xmDirection);
	DirectX::XMStoreFloat4(&data.globalLightDirectionView, DirectX::XMVector4Transform(xmDirection, PointOfView::GetMainCamera().GetViewMatrix()));

	data.globalLightVP = DirectX::XMMatrixTranspose(shadowNear->matrices.viewProjection);
	data.globalLightVPInv = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, shadowNear->matrices.viewProjection));

	DirectX::XMStoreFloat3(&data.globalLightPos, xmPosition);
	DirectX::XMStoreFloat3(&data.globalLightRight, xmRight);
	DirectX::XMStoreFloat3(&data.globalLightUp, xmUp);

}

void GlobalLight::BindShadowMap(Rendering & renderer)
{
	shadowNear->BindShadowMap(renderer);
}

void GlobalLight::SetShadowMapAsPSResourceView(Rendering & renderer, UINT slot)
{
	shadowNear->shadowMap.SetAsPixelShaderResourceView(renderer, slot);
}
