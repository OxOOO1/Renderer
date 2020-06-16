#include "Transformator.h"

#include "ImGUI/imgui.h"
#include "Scene/Camera.h"

void Transform::ShowUIWindow(std::string_view windowName)
{

	if (ImGui::Begin(windowName.data()))
	{

		ImGui::Columns(2, nullptr, true);

		ExpandNodeTreeUI(0, selectedNodeGUIIndex);

		ImGui::NextColumn();

		if (selectedNodeGUIIndex)
		{
			auto& node = mNodes.at(selectedNodeGUIIndex.value());
			auto nodeID = selectedNodeGUIIndex.value();

			if (ImGui::DragFloat3("Position", &node.transform.position.x)) {
				node.UpdatePositionXM();
				UpdateLocalTransform(nodeID);
			}

			if (ImGui::DragFloat("Scale", &node.transform.scale)) {
				UpdateLocalTransform(nodeID);
			}

			if (ImGui::Button("AttachToCamera")) {
				node.transform.positionXM = Camera::GetMainCamera().GetPositionXM();
				UpdateLocalTransform(nodeID);
			}

			if (ImGui::SliderAngle("X", &node.transform.rotationEuler.x)) {
				node.UpdateOrientationQuat();
				UpdateLocalTransform(nodeID);
			}

			if (ImGui::SliderAngle("Y", &node.transform.rotationEuler.y)) {
				node.UpdateOrientationQuat();
				UpdateLocalTransform(nodeID);
			}

			if (ImGui::SliderAngle("Z", &node.transform.rotationEuler.z)) {
				node.UpdateOrientationQuat();
				UpdateLocalTransform(nodeID);
			}

		}

	}
	ImGui::End();


}



void Transform::UpdateLocalTransform(UINT nodeIndex)
{
	auto& node = mNodes.at(nodeIndex);
	auto mat = DirectX::XMMatrixIdentity();
	mat = DirectX::XMMatrixMultiply(mat, DirectX::XMMatrixScaling(node.transform.scale, node.transform.scale, node.transform.scale));
	mat = DirectX::XMMatrixMultiply(mat, DirectX::XMMatrixRotationQuaternion(node.transform.orientationQuat));
	node.matLocalTransform = DirectX::XMMatrixMultiply(mat, DirectX::XMMatrixTranslationFromVector(node.transform.positionXM));
	bNeedsUpdate = true;
}

void Transform::UpdateTransforms(UINT nodeIndex, const DirectX::XMMATRIX & parentTransform)
{
	auto& bone = mNodes[nodeIndex];

	auto t = bone.matLocalTransform;

	//if(DirectX::XMMatrixIsIdentity(matLocalTransform))
	if (!bone.isAnimated)
		t = DirectX::XMMatrixMultiply(bone.matLocalTransform, bone.matParentSpace);

	bone.matGlobalTransform = DirectX::XMMatrixMultiply(t, parentTransform);

	if (!bone.isJoint)
		allNodesFinalTransforms.at(nodeIndex) = DirectX::XMMatrixTranspose(bone.matGlobalTransform);
	else
		allNodesFinalTransforms.at(nodeIndex) = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(bone.matInvBindPose, bone.matGlobalTransform));

	for (const auto i : bone.indicesToChildren)
	{
		UpdateTransforms(i, bone.matGlobalTransform);
	}

}

void Transform::ExpandNodeTreeUI(UINT indexToNode, std::optional<int>& selectedIndex) const noexcept
{
	auto& node = mNodes.at(indexToNode);

	const auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow |
		((node.index == selectedIndex) ? ImGuiTreeNodeFlags_Selected : 0)
		| ((node.indicesToChildren.empty()) ? ImGuiTreeNodeFlags_Leaf : 0)
		;

	const auto expanded = ImGui::TreeNodeEx((void*)(intptr_t)node.index, node_flags, node.name.c_str());
	if (ImGui::IsItemClicked())
	{
		selectedIndex = node.index;
	}
	if (expanded)
	{
		for (const auto i : node.indicesToChildren)
		{
			ExpandNodeTreeUI(i, selectedIndex);
		}
		ImGui::TreePop();
	}

}
