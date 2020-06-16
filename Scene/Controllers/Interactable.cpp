#include "Interactable.h"



#include "Scene/Camera.h"

#include "src/System/Utilities/Importer.h"

void Interactables::OnUpdate(Rendering & renderer, Camera & mainCamera, SceneTree& octree)
{
	float distance = 0;
	for(int i = 0; i < mObjects.size(); i++)
	{	
		auto object = mObjects[i];
		auto& bbox = octree.allModelsAABBoxes[object->mGeometry.indexToBBox];

		if (mainCamera.Intersects(bbox.dAABB, distance))
		{
			if (distance < 1000)
			{
				indexToActiveObject = i;
				bbox.Draw();

				if (!object->mAnimator.mAnimations.front().isRunning && !object->mAnimator.mAnimations.front().isFinished)
				{
					uiSpriteOpen.SetPosition(renderer, bbox.dAABB.Center);
					
					float size = distance / 10.;
					//size = std::clamp(size, bbox.size / 5.f, bbox.size);
					uiSpriteOpen.SetScale(renderer, { size, size / 3.4f });
					bUIEnabled = true;
					return;
				}
			}
		}

	}

	bUIEnabled = false;
	indexToActiveObject = -1;

}

void Interactables::DrawUI(Rendering & renderer)
{
	if (bUIEnabled)
	{
		DepthStencilState::BindDefaultNoDepthCheck(renderer);
		uiSpriteOpen.Draw(renderer);
		DepthStencilState::BindDefault(renderer);
	}
}


