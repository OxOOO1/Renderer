#include "Painter.h"

#include <thread>
#include "System/Utilities/Console.h"
#include "System/Utilities/DebugDraw.h"

#include "PostProcess.h"

#include "src/Components/Drawable/Model.h"
#include "Scene/Scene.h"
#include "Scene/Lights/PointLight.h"
#include "System/GlobalCBuffer.h"


Painter::Painter(Window & window)
	:renderer(window)
{
	pMainCamera = &mainCamera;
	pGlobalLight = &mGlobalLight;

	GlobalCBuffer::Bind(renderer);

	pScene = std::make_shared<Scene>(renderer);

}

void Painter::Draw()
{

	renderer.Begin();

	mainCamera.OnUpdate(renderer);
	mGlobalLight.OnUpdate(renderer, mainCamera);
	mWind.OnUpdate(Time::Get().GetDeltaTimeSec());

	pScene->octree.OnUpdate(mainCamera);
	pScene->OnUpdate(renderer, mGlobalLight, mainCamera);

	GlobalCBuffer::Bind(renderer);
	GlobalCBuffer::Update(renderer);

	/////////////////////////////////////////////////////----------------------Global Light Shadow Map Pass---------------------------///////////////////////////////////

	mGlobalLight.BindShadowMap(renderer);
	pScene->DrawDepth(renderer, mGlobalLight, mWind.GetPosition());
	
	/////////////////////////////////////////////////////-------------------------------Draw Scene---------------------------///////////////////////////////////

	renderer.BindDefaultRenderTargetAndViewport();
	//rtHDR.Bind(renderer);
	
	mGlobalLight.ShowControlWindow(renderer);
	mGlobalLight.SetShadowMapAsPSResourceView(renderer, 3u);

	pScene->Draw(renderer, mWind.GetPosition());

	//rtLightScatter1.BindClear(renderer);
	mGlobalLight.DrawMesh(renderer);
	//renderer.BindDefaultRenderTarget();

	/*particles.ShowControlWindow();
	Particles::OnUpdate(renderer);
	particles.Draw(renderer);*/

	mWind.Draw();

	if (bImGuiEnabled)
	{
		static Console console;
		console.Draw("Example: Console", &bImGuiEnabled);
		renderer.ShowTextureWindow();
		renderer.ShowSettingsWindow();
		mainCamera.SpawnControlWindow();
	}

	renderer.End();

}

void Painter::OnEvent(Event & event)
{
	if (event.CheckTypeAndHandle(EventType::KeyboardEvent))
	{
		OnKeyboardEvent(reinterpret_cast<KeyboardEvent&>(event));
	}

	mainCamera.OnEvent(event);

	pScene->OnEvent(event);
}

Camera* Painter::pMainCamera = nullptr;
PointOfView* PointOfView::pMainCamera = nullptr;
GlobalLight* Painter::pGlobalLight = nullptr;