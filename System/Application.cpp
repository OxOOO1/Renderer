#define _CRT_SECURE_NO_WARNINGS
#include "Application.h"

#include <d3dcompiler.h>
#include <sstream>
#include <iomanip>

#include "src/thirdParty/ImGUI/imgui.h"
#include "src/thirdParty/ImGUI/imgui_impl_dx11.h"
#include "src/thirdParty/ImGUI/imgui_impl_win32.h"

#include "Rendering.h"
#include "System/Utilities/FileIO.h"
#include "System/Utilities/Console.h"
#include "System/Utilities/ImageLoaders/ImageLoader.h"

#include "Scene/Camera.h"
#include "Bindable/ConstantBuffer.h"
#include "System/Utilities/DebugDraw.h"

#include "System/Input/Mouse.h"
#include "System/Input/Keyboard.h"

Application::Application(int windowWidth, int windowHeight, std::string name)
	: 
	mWindow(windowWidth, windowHeight, name.c_str())
{
	pInstance = this;

	pPainter = std::make_unique<Painter>(mWindow);

}

bool Application::OnEvent(Event& event)
{

	mWindow.OnEvent(event);
	pPainter->OnEvent(event);

	return event.Handled;

}

int Application::Run()
{
	while (true)
	{

		const auto exitcode = Window::ProcessMessage();
		if (exitcode)
		{
			return *exitcode;
		}

		DoFrame();

	}
	
}

void Application::DoFrame()
{
	Time::Get().OnUpdate();

	pPainter->Draw();

	Mouse::Get().FlushPosOffset();

}

Application* Application::pInstance = nullptr;