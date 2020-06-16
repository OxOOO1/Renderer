#pragma once

#include "System/Rendering.h"

class Topology
{
public:

	static Topology& Get(Rendering& renderer, D3D_PRIMITIVE_TOPOLOGY type)
	{
		switch (type)
		{
		case D3D_PRIMITIVE_TOPOLOGY_POINTLIST:
			return PointList(renderer);
			break;
		case D3D_PRIMITIVE_TOPOLOGY_LINELIST:
			return LineList(renderer);
			break;
		case D3D_PRIMITIVE_TOPOLOGY_LINESTRIP:
			return LineStrip(renderer);
			break;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
			return TriangleList(renderer);
			break;
		case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
			return TriangleStrip(renderer);
			break;
		default:
			return TriangleList(renderer);
			break;
		}
		
	}
	void Bind(Rendering& renderer) const;

private:

	static Topology& PointList(Rendering& renderer)
	{
		static Topology pl(renderer, D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		return pl;
	}
	static Topology& LineList(Rendering& renderer)
	{
		static Topology ll(renderer, D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		return ll;
	}
	static Topology& LineStrip(Rendering& renderer)
	{
		static Topology ls(renderer, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		return ls;
	}
	static Topology& TriangleList(Rendering& renderer)
	{
		static Topology tl(renderer, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return tl;
	}
	static Topology& TriangleStrip(Rendering& renderer)
	{
		static Topology ts(renderer, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		return ts;
	}



private:
	Topology(Rendering& renderer, D3D_PRIMITIVE_TOPOLOGY t);
	
	D3D_PRIMITIVE_TOPOLOGY topologyType;

	static D3D_PRIMITIVE_TOPOLOGY currentlyBoundType;
	
};