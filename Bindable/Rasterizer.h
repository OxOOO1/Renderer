#pragma once

#include <array>
#include "System/Rendering.h"
#include "src/thirdParty/ImGUI/imgui.h"

class Rasterizer
{

public:
	
	static Rasterizer& Get(Rendering& renderer, bool isTwoSided = false)
	{
		if (!isTwoSided)
		{
			static Rasterizer back_cull(renderer, D3D11_CULL_BACK);
			return back_cull;
		}
		else {
			static Rasterizer no_cull(renderer, D3D11_CULL_NONE);
			return no_cull;
		}
	}

	static Rasterizer& GetWithDepthBias(Rendering& renderer, bool isTwoSided = false)
	{
		if (!isTwoSided)
		{
			static Rasterizer back_culldb(renderer, D3D11_CULL_BACK, true);
			return back_culldb;
		}
		else {
			static Rasterizer no_culldb(renderer, D3D11_CULL_NONE, true);
			return no_culldb;
		}
	}

	void Bind(Rendering& renderer) const ;

private:

private:

	Rasterizer(Rendering& renderer, D3D11_CULL_MODE cullMode, bool DepthBias = false);

	Rasterizer() = default;

protected:
	
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> pRasterState;
	
};