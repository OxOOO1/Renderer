#include "Topology.h"
#include "System/Rendering.h"

 Topology::Topology(Rendering & renderer, D3D_PRIMITIVE_TOPOLOGY t) : topologyType(t)
{

}

 void Topology::Bind(Rendering & renderer) const
{
	 if (topologyType != currentlyBoundType)
	 {
		 renderer.GetContext()->IASetPrimitiveTopology(topologyType);
		 currentlyBoundType = topologyType;
	 }
	
}

 D3D_PRIMITIVE_TOPOLOGY Topology::currentlyBoundType = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;