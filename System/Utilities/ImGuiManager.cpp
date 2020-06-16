#include "ImGuiManager.h"
#include "thirdParty/ImGUI/imgui.h"

ImGuiManager::ImGuiManager()
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

}

ImGuiManager::~ImGuiManager()
{
	ImGui::DestroyContext();
}
