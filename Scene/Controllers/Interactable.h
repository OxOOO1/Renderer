#pragma once

#include "Components/Drawable/Model.h"
#include "src/System/Events/Event.h"
#include "src/System/Events/MouseEvents.h"
#include "src\Components\Drawable\Plane.h"

//Controller over interactable Animated Meshes
class Interactables
{
	friend class Scene;

protected:
	Interactables(Rendering& renderer)
	:
	uiSpriteOpen{ renderer, {0,0,0}, "assets\\textures\\billboards\\UIOpenBlack.png", 5.f },
	uiSpriteClose{ renderer, {0,0,0}, "assets\\textures\\billboards\\UICloseBlack.png", 5.f }
	{
	
		//allMeshes.emplace_back("MainDoor", renderer, "assets\\finex\\Interactables\\models\\MainDoor.fbx", 1.f, false, true);

	}

	void Add(ModelSkinned* object)
	{
		mObjects.push_back(object);
	}

	void OnUpdate(Rendering& renderer, Camera& mainCamera, SceneTree& octree);

	void DrawUI(Rendering& renderer);

	void OnEvent(Event& event)
	{
		if (event.CheckTypeAndHandle(EventType::MouseEvent))
		{
			OnMouseEvent(reinterpret_cast<MouseEvent&>(event));
		}
	}

	void OnMouseEvent(MouseEvent& event)
	{
		if (event.GetButton() == MouseEvent::Button::Left && event.IsPressed())
		{
			if (indexToActiveObject != -1)
			{
				mObjects.at(indexToActiveObject)->StartAnimation();
			}
		}
	}

private:
	short indexToActiveObject = -1;

	std::vector<ModelSkinned*> mObjects;

	Billboard uiSpriteOpen;
	Billboard uiSpriteClose;
	
	bool bUIEnabled = false;

};