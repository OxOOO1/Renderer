#pragma once
#include "Window.h"
#include "Utilities/Time.h"
#include "Utilities/ImGuiManager.h"

#include "System/Painter.h"
#include "Events/Event.h"

class Application
{
public:
	static Application& Get()
	{
		return *pInstance;
	}

	Application(int windowWidth = 1024, int windowHeight = 768, std::string name = "Default Application");
	Application() = delete;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	Application(const Application&&) = delete;
	Application& operator=(const Application&&) = delete;

	int Run();
	bool OnEvent(Event& event);
private:
	void DoFrame();
private:
	ImGuiManager m_ImGuiManager;

	Window mWindow;

	std::unique_ptr<Painter> pPainter;
	

	bool show_demo_window = true;
	char buffer[100];

	static Application* pInstance;



};