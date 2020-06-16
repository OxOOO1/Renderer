#include "Animation.h"

#include <sstream>
#include "ImGUI/imgui.h"

bool AnimationClip::OnUpdate(float dTimeSec)
{
	if (isRunning)
	{
		curRunTime += dTimeSec;
		curRunTimeInTicks = curRunTime * ticksPerSec * speedModifier;
		if (curRunTimeInTicks > durationInTicks)
		{
			EndAnimation();
			return true;
		}
	}
	return false;
}



void AnimationClip::SetBoneTransforms()
{
	int nn = 0;
	for (auto& channel : mChannels)
	{
		auto transform = DirectX::XMMatrixIdentity();
		
		int index = -1;
		int indexNext;
		double timeNext;

		auto&[prevScaleI, prevRotI, prevPosI] = prevFrameIndex;

		//////////////////////SCALE
		if (channel.scalings.size() != 1)
		{
			for (int i = prevScaleI; i < channel.scalings.size() - 1; i++){
				if (curRunTimeInTicks < channel.scalings.at(i + 1).time){
					index = i;
					prevScaleI = index;
					break;
				}
			}
			if (index == -1) index = channel.scalings.size() - 2;
			indexNext = index + 1;
			timeNext = channel.scalings.at(indexNext).time;
			float dTime = timeNext - channel.scalings.at(index).time;
			float factor = (curRunTimeInTicks - (float)channel.scalings.at(index).time) / dTime;
			auto& startS = channel.scalings.at(index).value;
			auto& endS = channel.scalings.at(indexNext).value;

			transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixScalingFromVector(
				DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&startS), DirectX::XMLoadFloat3(&endS), factor)));
		}
		else
			transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixScaling(
				channel.scalings.at(0).value.x, channel.scalings.at(0).value.y, channel.scalings.at(0).value.z));
			
		//////////////////////ORIENTATION
		if (channel.orientations.size() != 1)
		{
			for (int i = prevRotI; i < channel.orientations.size() - 1; i++){
				if (curRunTimeInTicks < channel.orientations.at(i + 1).time){
					index = i;
					prevRotI = index;
					break;
				}
			}
			if (index == -1) index = channel.orientations.size() - 2;
			indexNext = index + 1;
			timeNext = channel.orientations.at(indexNext).time;
			float dTime = timeNext - channel.orientations.at(index).time;
			float factor = (curRunTimeInTicks - (float)channel.orientations.at(index).time) / dTime;
			auto& startRot = channel.orientations.at(index).value;
			auto& endRot = channel.orientations.at(indexNext).value;

			transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixRotationQuaternion( DirectX::XMQuaternionSlerp(startRot, endRot, factor)));
		}
		else
			transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixRotationQuaternion(channel.orientations.at(0).value));

		///////////////////////POSITION
		if (channel.positions.size() != 1)
		{
			index = -1;

			for (int i = prevPosI; i < channel.positions.size() - 1; i++){
				if (curRunTimeInTicks < channel.positions.at(i + 1).time){
					index = i;
					prevPosI = index;
					break;
				}
			}
			if (index == -1) index = channel.positions.size() - 2;
			indexNext = index + 1;
			timeNext = channel.positions.at(indexNext).time;
			float dTime = timeNext - channel.positions.at(index).time;
			float factor = (curRunTimeInTicks - (float)channel.positions.at(index).time) / dTime;
			auto& startPos = channel.positions.at(index).value;
			auto& endPos = channel.positions.at(indexNext).value;

			transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixTranslationFromVector(DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&startPos), DirectX::XMLoadFloat3(&endPos), factor)));
		}
		else
			transform = DirectX::XMMatrixMultiply(transform, DirectX::XMMatrixTranslation(channel.positions.at(0).value.x, channel.positions.at(0).value.y, channel.positions.at(0).value.z));

		assert(channel.pToBoneTransform);
		*channel.pToBoneTransform = transform;
		
	}

}


void AnimationClip::ShowControlWindow()
{
	if (ImGui::Button("StartAnimation"))
	{
		StartAnimation();
	}
	if (ImGui::Button("EndAnimation"))
	{
		PauseAnimation();
	}
	ImGui::Checkbox("Loop", &isLooping);

	std::stringstream ss;

	ss << curRunTimeInTicks << " / " << durationInTicks;

	ImGui::Text(ss.str().c_str());

}

void AnimationClip::StartAnimation()
{
	isRunning = true;
	if (!isPaused){
		startTime = Time::Get().GetTimeAfterLaunchSec();
		curRunTime = 0.f;
		curRunTimeInTicks = 0.f;
	}
	isPaused = false;
	isFinished = false;
}

void AnimationClip::EndAnimation()
{
	if (isLooping){
		StartAnimation();
	}
	else{
		isFinished = true;
		isRunning = false;
		curRunTime = 0.f;
		curRunTimeInTicks = 0.f;
	}
}


//
//bool AnimationClipTwoPhase::OnUpdate(float dTimeSec)
//{
//	if (isRunning)
//	{
//		curRunTime += dTimeSec;
//		curRunTimeInTicks = curRunTime * ticksPerSec * speedModifier;
//
//		if (curRunTimeInTicks > secondPhaseStart && !isSecondPhase && !isLooping)
//		{
//			PauseAnimation();
//			isSecondPhase = true;
//		}
//
//		if (curRunTimeInTicks > durationInTicks)
//		{
//			EndAnimation();
//			isSecondPhase = false;
//			return true;
//		}
//
//	}
//
//	return false;
//
//}
//
//
//
//void AnimationClipTwoPhase::SetBoneTransforms(ModelSkinned * pToModel)
//{
//	int nn = 0;
//	for (auto& channel : channels)
//	{
//
//		auto indexToBone = pToModel->mBoneNameToIDMap[channel.nodeName];
//
//		auto& boneTransform = pToModel->bones.at(indexToBone).transform;
//
//		if (channel.orientations.size() == 1)
//		{
//			boneTransform.Orientation = channel.orientations.at(0).value;
//
//			if (channel.positions.size() == 1)
//			{
//				boneTransform.Position = channel.positions.at(0).value;
//			}
//			else
//			{
//				assert(curRunTimeInTicks < channel.positions.size());
//				boneTransform.Position = channel.positions.at(curRunTimeInTicks).value;
//			}
//			continue;
//		}
//
//		if (channel.orientations.size() >= durationInTicks)
//		{
//			boneTransform.Orientation = channel.orientations.at(curRunTimeInTicks).value;
//
//			if (channel.positions.size() == 1)
//			{
//				boneTransform.Position = channel.positions.at(0).value;
//			}
//			else
//			{
//				assert(curRunTimeInTicks < channel.positions.size());
//				boneTransform.Position = channel.positions.at(curRunTimeInTicks).value;
//			}
//			continue;
//		}
//
//		int index = -1;
//
//		assert(channel.positions.size() == channel.orientations.size());
//
//		for (int i = 0; i < channel.positions.size() - 1; i++)
//		{
//			if (curRunTimeInTicks < channel.positions.at(i + 1).time)
//			{
//				index = i;
//				break;
//			}
//		}
//
//		int indexNext;
//
//		double lastTime;
//
//		if (index != -1)
//		{
//			indexNext = index + 1;
//
//			lastTime = channel.positions.at(indexNext).time;
//		}
//		else
//		{
//			index = channel.positions.size() - 1;
//
//			indexNext = 0;
//
//			lastTime = durationInTicks;
//		}
//
//		float dTime = lastTime - channel.positions.at(index).time;
//
//		float factor = (curRunTimeInTicks - (float)channel.positions.at(index).time) / dTime;
//
//		//assert(factor >= 0.f && factor <= 1.f);
//
//		auto& startRot = channel.orientations.at(index).value;
//		auto& endRot = channel.orientations.at(indexNext).value;
//
//		auto& startPos = channel.positions.at(index).value;
//		auto& endPos = channel.positions.at(indexNext).value;
//
//		boneTransform.Orientation = DirectX::XMQuaternionSlerp(startRot, endRot, factor);
//
//		DirectX::XMStoreFloat3(&boneTransform.Position, DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&startPos), DirectX::XMLoadFloat3(&endPos), factor));
//
//	}
//
//	//pToModel->bones.at(0).UpdateTransform(pToModel);
//
//}
