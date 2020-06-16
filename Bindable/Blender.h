#pragma once


#include "System/Rendering.h"

//Finite state generic; 

class Blender
{
public:

	static Blender& Get(Rendering& renderer, bool enabled = false, bool alphaToCoverage = false)
	{
		if (enabled)
		{
			if (alphaToCoverage)
			{
				static Blender enal(renderer, enabled, alphaToCoverage);
				return enal;
			}
			else
			{
				static Blender ennoal(renderer, enabled, alphaToCoverage);
				return ennoal;
			}
		}
		else if (!alphaToCoverage)
		{
			static Blender dis(renderer);
			return dis;
		}
		else {

		}
		
			
	}

	static Blender& GetParticlesBlender(Rendering& renderer, bool addOp = true, bool alphaPremult = false)
	{
		if (addOp)
		{
			if (alphaPremult)
			{
				static Blender partBladal;
				partBladal.CreateForParticles(renderer, D3D11_BLEND_OP_ADD, alphaPremult);
				return partBladal;
			}
			else
			{
				static Blender partblad;
				partblad.CreateForParticles(renderer, D3D11_BLEND_OP_ADD, alphaPremult);
				return partblad;
			}
		}
		else
		{
			if (alphaPremult)
			{
				static Blender partrevsubal;
				partrevsubal.CreateForParticles(renderer, D3D11_BLEND_OP_REV_SUBTRACT, alphaPremult);
				return partrevsubal;
			}
			else
			{
				static Blender partrevsub;
				partrevsub.CreateForParticles(renderer, D3D11_BLEND_OP_REV_SUBTRACT, alphaPremult);
				return partrevsub;
			}
		}
		
	}

	void Bind(Rendering& renderer) const;

private:

	int id;
	static int currentlyBoundId;
	bool isInit = false;

private:

	Blender() = default;

	Blender(Rendering& renderer, bool Enabled = false, bool AlphaToCoverage = false);
	
	void CreateForParticles(Rendering& renderer, D3D11_BLEND_OP blendOp, bool alphaPremult);

	
protected:
	
	Microsoft::WRL::ComPtr<ID3D11BlendState> pBlender;
};