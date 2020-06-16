#pragma once

#include <future>

#include "System/Rendering.h"
#include "System/Events/Event.h"
#include "System/Events/KeyboardEvents.h"
#include "Scene/Lights/GlobalLight.h"

#include "Scene/Wind.h"

#include "System/RenderTarget.h"
#include "System/PostProcess.h"
#include "src/Components/Drawable/Particles.h"

class Scene;

class Painter
{
public:

	Painter(Window& window);
	Painter() = delete;
	Painter(const Painter&) = delete;
	Painter& operator=(const Painter&) = delete;
	Painter(const Painter&&) = delete;
	Painter& operator=(const Painter&&) = delete;

	~Painter()
	{
		//save state
	}
	
	void Draw();

	void OnEvent(Event& event);

	void OnKeyboardEvent(KeyboardEvent& event)
	{
		if (event.GetKey() == 'C' && event.IsPressed())
		{
			bImGuiEnabled = !bImGuiEnabled;
		}
	}

	static Camera* GetMainCamera()
	{
		return pMainCamera;
	}
	static GlobalLight* GetGlobalLight()
	{
		return pGlobalLight;
	}

private:
	Rendering renderer;

	std::shared_ptr<Scene> pScene;

private:
	Wind mWind;

	Camera mainCamera{ renderer, renderer.GetViewportSize().first, renderer.GetViewportSize().second, 10.1f, 50000.f };
	static Camera* pMainCamera;
	
	GlobalLight mGlobalLight{ renderer, mainCamera};
	static GlobalLight* pGlobalLight;

	bool bImGuiEnabled = false;

	//Particles particles{ renderer, 5000 };

	/////////////////////////////////////////////////////----------------------------------Post Processing---------------------------///////////////////////////////////

	RenderTarget rtHDR{ renderer, renderer.GetViewportSize().first, renderer.GetViewportSize().second, DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE};
	RenderTarget rtHDR2{ renderer, renderer.GetViewportSize().first, renderer.GetViewportSize().second, DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE };

	RenderTarget rtLight{ renderer, renderer.GetViewportSize().first / 4, renderer.GetViewportSize().second /4 , DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };

	PostProcess::VolumeLight fxVolumeLight{ renderer, mGlobalLight };
	RenderTarget rtVolumeLight{ renderer, renderer.GetViewportSize().first / 2, renderer.GetViewportSize().second / 2 , DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };

	PostProcess::AntiAliasing fxFXAA{ renderer };

	PostProcess::BlurFilter mBlurFilter{ renderer, rtLight };

	//RenderTargetSSAO rtSSAO{mainCamera.GetProjectionMatrix(), renderer, renderer.GetViewportSize().first, renderer.GetViewportSize().second, DXGI_FORMAT_R16_UNORM, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE };

	PostProcess::LightScattering fxLightScatter{ renderer };
	RenderTarget rtLightScatterInput{ renderer, renderer.GetViewportSize().first , renderer.GetViewportSize().second  , DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };
	RenderTarget rtLightScatterOutput{ renderer, renderer.GetViewportSize().first / 4., renderer.GetViewportSize().second / 4. , DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };

	PostProcess::LightAbberation fxLightAbberation{ renderer };
	RenderTarget rtAbberation{ renderer, renderer.GetViewportSize().first / 4, renderer.GetViewportSize().second  /4 , DXGI_FORMAT_R11G11B10_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS };
	
};