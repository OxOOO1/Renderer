#include <stdio.h>
#include <io.h>
#include <fcntl.h>

#include <Windows.h>
#include "Window.h"
#include <iostream>
#include <sstream>
#include "Application.h"

constexpr auto ScreenWidth = 1280;
constexpr auto ScreenHeight = 720;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hprevInstance, LPSTR pCmdLine, int nCmdShow)
{
	
	try
	{
		Time::Get();

		Application application(ScreenWidth, ScreenHeight);

		return application.Run();

	}

	catch (const Window::WinException& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "C++ exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details", "Unknown exception", MB_OK | MB_ICONEXCLAMATION);
	}

	//return -1;
	//return (WPARAM)message.wParam; 
	
}