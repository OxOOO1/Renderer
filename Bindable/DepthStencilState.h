#pragma once

#include "System/Rendering.h"


class DepthStencilState
{
public:
	enum class Mode
	{
		Off,
		Write,
		Mask,
		NoDepth,
		DepthNoWrites
	};

	static void BindForStencilWriting(Rendering& renderer)
	{
		static DepthStencilState writeStencil(renderer, Mode::Write);
		writeStencil.Bind(renderer);
	}

	static void BindForStencilMasking(Rendering& renderer)
	{
		static DepthStencilState maskingStencil(renderer, Mode::Mask);
		maskingStencil.Bind(renderer);
	}

	static void BindDefault(Rendering& renderer)
	{
		static DepthStencilState defaultStencil(renderer, Mode::Off);
		defaultStencil.Bind(renderer);
	}

	static void BindDefaultNoDepthCheck(Rendering& renderer)
	{
		static DepthStencilState defaultNoDepth(renderer, Mode::NoDepth);
		defaultNoDepth.Bind(renderer);
	}

	static void BindDefaultNoDepthWrites(Rendering& renderer)
	{
		static DepthStencilState defaultNoDepthWrites(renderer, Mode::DepthNoWrites);
		defaultNoDepthWrites.Bind(renderer);
	}

private:
	DepthStencilState(Rendering& renderer, Mode mode);
	void Bind(Rendering& renderer) const;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> pStencil;
};