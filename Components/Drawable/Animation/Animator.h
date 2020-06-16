#pragma once

#include "Node.h"
#include "Animation.h"
#include "Transformator.h"

class Animator : public Transform
{
public:

	void Init(Rendering& renderer)
	{
		Transform::Init(renderer);

		for (auto& anim : mAnimations)
		{
			AssignBonesToChannels(anim);
		}
	}

	void OnUpdate(Rendering & renderer)
	{
		for (auto& anim : mAnimations)
		{
			if (anim.isRunning)
			{
				if (!anim.OnUpdate(Time::Get().GetDeltaTimeSec()))
				{
					anim.SetBoneTransforms();
				}
				bNeedsUpdate = true;
			}
		}
		
		Transform::OnUpdate(renderer);

	}

	void StartAnimation(UINT clipIndex = 0)
	{
		auto& animation = mAnimations.at(clipIndex);
		if (!animation.isRunning)
		{
			animation.StartAnimation();
		}
	}

	void AssignBonesToChannels(AnimationClip& animation)
	{
		for (auto& anim : mAnimations){
			for (auto& channel : anim.mChannels)
			{
				auto it = mNodeNameToNodeIDMap.find(channel.nodeName);
				assert(it != mNodeNameToNodeIDMap.end());
				unsigned short indexToBone = it->second;
				channel.pToBoneTransform = &mNodes.at(indexToBone).matLocalTransform;
				mNodes.at(indexToBone).isAnimated = true;
			}
		}
	}

	void ShowAnimationUIWindow(std::string_view name)
	{
		using namespace std::string_literals;
		if (ImGui::Begin((name.data() + "_Animation"s ).c_str()))
		{
			ImGui::Columns(2, nullptr, true);

			for (int i = 0; i < mAnimations.size(); i++)
			{
				if (ImGui::Selectable(mAnimations.at(i).name.c_str(), selectedAnimGUIIndex == i))
					selectedAnimGUIIndex = i;
			}

			ImGui::NextColumn();

			if (selectedAnimGUIIndex)
			{
				mAnimations.at(selectedAnimGUIIndex.value()).ShowControlWindow();
			}
		}

		ImGui::End();
	}


public:
	std::vector<AnimationClip> mAnimations;
	std::unordered_map<std::string, UINT> mAnimationNameToIndexMap;

	std::optional<int> selectedAnimGUIIndex;

};