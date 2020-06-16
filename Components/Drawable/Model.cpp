#include "Model.h"
#include "Bindable/BindablesH.h"
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <DirectXCollision.h>

#include "System/Utilities/FileIO.h"

#include "src/System/Utilities/Time.h"
#include "System/Utilities/Console.h"
#include "System/Rendering.h"
#include "Bindable/StreamOutput.h"
#include "System/Painter.h"
#include "Scene/Camera.h"
#include "System/Utilities/DebugDraw.h"


#include "System/RenderTarget.h" 

 void Model::OnUpdate(SceneTree & octree)
 {
	 mGeometry.PopulateDrawQueues(octree);
 }

 void Model::Draw(Rendering & renderer)
 {
	 mGeometry.DrawShadedFromQueue(renderer, mShading);
 }

 void Model::DrawDepth(Rendering & renderer)
 {
	mGeometry.DrawDepthFromQueue(renderer, mShading);
 }

 void ModelDynamic::OnUpdate(Rendering& renderer, SceneTree & octree)
 {
	 mTransformator.OnUpdate(renderer);
	 //mGeometry.PopulateDrawQueues(octree);
 }

 void ModelDynamic::Draw(Rendering & renderer)
 {
	 mTransformator.Bind(renderer);
	 mTransformator.ShowUIWindow(mName);
	 mGeometry.DrawShaded(renderer, mShading);
 }

 void ModelDynamic::DrawDepth(Rendering & renderer)
 {
	 mTransformator.Bind(renderer);

	 mGeometry.DrawDepth(renderer, mShading);
 }


 void ModelSkinned::OnUpdate(Rendering & renderer, SceneTree & octree)
 {
	 mAnimator.OnUpdate(renderer);
 }

 void ModelSkinned::Draw(Rendering & renderer)
 {
	 mAnimator.Bind(renderer);
	 mAnimator.ShowUIWindow(mName);
	 mAnimator.ShowAnimationUIWindow(mName);
	 mGeometry.DrawShaded(renderer, mShading);
 }

 void ModelSkinned::DrawDepth(Rendering & renderer)
 {
	 mAnimator.Bind(renderer);
	 mGeometry.DrawDepth(renderer, mShading);
 }


void ModelLOD::OnUpdate(SceneTree & octree)
{

	auto& bbox = octree.allModelsAABBoxes[indexToMainBBox];

	//TODO get distance from MainBox size
	static constexpr auto LODLevelDistance = 1.f / 5000.f;

	if (bbox.bToDraw)
	{
		auto numOfLODLevels = mGeometryLOD.size();

		indexToActiveLODNode = bbox.distanceToCamera * LODLevelDistance;
		if (indexToActiveLODNode > numOfLODLevels - 1) indexToActiveLODNode = numOfLODLevels - 1;

		mGeometryLOD.at(indexToActiveLODNode).PopulateDrawQueues(octree);
	}

}

void ModelLOD::Draw(Rendering & renderer)
{
	mGeometryLOD[indexToActiveLODNode].DrawShadedFromQueue(renderer, mShading);
}

void ModelLOD::DrawDepth(Rendering & renderer)
{
	mGeometryLOD[indexToActiveLODNode].DrawDepthFromQueue(renderer, mShading);
}
