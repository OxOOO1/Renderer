#pragma once
#include "System/Rendering.h"
#include "System/Utilities/Time.h"

class AnimationClip
{
public:

	void ShowControlWindow();
	bool bShowControlWindow = false;

	void StartAnimation();
	void EndAnimation();
	void PauseAnimation()
	{
		isRunning = false;
		isPaused = true;
	}

	virtual bool OnUpdate(float dTimeSec);

	virtual void SetBoneTransforms();

	struct Channel
	{
		std::string nodeName;

		struct Position
		{
			double time;
			DirectX::XMFLOAT3 value;
		};
		struct Orientation
		{
			double time;
			DirectX::XMVECTOR value;
		};
		struct Scale
		{
			double time;
			DirectX::XMFLOAT3 value;
		};

		std::vector<Position> positions;
		std::vector<Orientation> orientations;
		std::vector<Scale> scalings;

		DirectX::XMMATRIX* pToBoneTransform = nullptr;

	};

	std::vector<Channel> mChannels;

	double durationInTicks;
	double ticksPerSec;
	double speedModifier = 1.f;

	double startTime;

	double curRunTime = 0.f;
	double curRunTimeInTicks = 0.f;

	std::tuple<UINT, UINT, UINT> prevFrameIndex{ 0,0,0 };

	bool isLooping = false;
	bool isRunning = false;
	bool isPaused = false;
	bool isFinished = false;

	std::string name;
};

//
//
//class AnimationClipTwoPhase : public AnimationClip
//{
//
//public:
//
//	AnimationClipTwoPhase(aiAnimation* animation, float speed = 1.f, bool isLooping = false)
//		:AnimationClip(animation, speed, isLooping)
//	{
//		secondPhaseStart = durationInTicks / 2.f;
//	}
//
//	bool OnUpdate(float dTimeSec) override;
//	void SetBoneTransforms(ModelSkinned* pToModel) override;
//
//protected:
//
//	bool isSecondPhase = false;
//	double secondPhaseStart;
//
//};


