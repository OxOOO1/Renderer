
#include "Node.h"

#include "ImGUI/imgui.h"

#include "Components/BoundingVolumes.h"
#include "Scene/Camera.h"

void Node::AddChild(unsigned short indexToChild) noexcept
{
	indicesToChildren.push_back(indexToChild);
}

