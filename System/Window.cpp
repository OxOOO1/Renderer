#include <Windows.h>
#include "Window.h"
#include <sstream>
#include <optional>
//#include "resource.h"
#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include <windowsx.h>
#include "thirdParty/ImGUI/imgui_impl_win32.h"
#include "thirdParty/ImGUI/imgui.h"
#include "Utilities/Time.h"
#include "Events/EventHandler.h"

Window::WindowClass Window::WindowClass::winClassInstance;

LRESULT  ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Window::OnKeyboardEvent(KeyboardEvent & event)
{
	if (event.IsPressed() && event.GetKey() == VK_SPACE)
	{
		EnableCursor();
		MouseCursorEvent cursorEnabled(MouseCursorEvent::CursorState::Shown);
		EventHandler::Dispatch(cursorEnabled);
		
	}
	if (event.IsPressed() && event.GetKey() == VK_CONTROL)
	{
		DisableCursor();
		MouseCursorEvent cursorDisabled(MouseCursorEvent::CursorState::Hidden);
		EventHandler::Dispatch(cursorDisabled);
	}
}

Window::Window(int width, int height, const char* name)
	: 
	width(width),
	height(height)
{
	Benchmark t("Creating Window");

	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw WND_EXCEPT_LAST(void);
	}

	hWnd = CreateWindow(
		Window::WindowClass::GetName(),
		name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_SIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		nullptr,
		nullptr,
		Window::WindowClass::GetInstance(),
		this
	);
	if (!hWnd)
	{
		throw WND_EXCEPT_LAST(void);
	}

	

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; //mouse
	rid.usUsage = 0x02; //mouse
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
	{
		throw WND_EXCEPT_LAST(void);
	}


	/*  SW_HIDE             0
		SW_SHOWNORMAL       1
		SW_NORMAL           1
		SW_SHOWMINIMIZED    2
		SW_SHOWMAXIMIZED    3
		SW_MAXIMIZE         3
		SW_SHOWNOACTIVATE   4
		SW_SHOW             5
		SW_MINIMIZE         6
		SW_SHOWMINNOACTIVE  7
		SW_SHOWNA           8
		SW_RESTORE          9
		SW_SHOWDEFAULT      10
		SW_FORCEMINIMIZE    11
		SW_MAX              11*/

	ShowWindow(hWnd, SW_SHOWDEFAULT);

	HideCursor();

	ImGui_ImplWin32_Init(hWnd);

}

void Window::EnableCursor()
{
	bCursorEnabled = true;
	ShowCursor();
}

void Window::DisableCursor()
{
	bCursorEnabled = false;
	HideCursor();
}

void Window::ConfineCursor()
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::FreeCursor()
{
	ClipCursor(nullptr);
}

void Window::HideCursor()
{
	while (::ShowCursor(FALSE) >= 0);
	ConfineCursor();
}

void Window::ShowCursor()
{
	while (::ShowCursor(TRUE) < 0);
	FreeCursor();
}

Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}


void Window::SetTitle(const std::string & title)
{
	if (SetWindowText(hWnd, title.c_str()) == 0)
	{
		throw WND_EXCEPT_LAST(void);
	}
}



LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	//wm_nccreate has a pointer to a struct of info about created window
	if (msg == WM_NCCREATE)
	{
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		//extract ptr to window class
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		//set WINAPI user data to store ptr to window
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd) );
		//set message proc to normal handler now that setup is finished
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::GetPtrToMessageHandler));
		//forward message to window class handler
		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);

}

LRESULT WINAPI Window::GetPtrToMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//retrieve ptr to window class 
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}


////////////////////////////MESSAGES///////////////////////////////////////////////////////////////////////////////////MESSAGES////////////////////////////////////////////////////////

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	
	switch (msg)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_KILLFOCUS:
		{
			Keyboard::Get().ResetKeyboardState();
			break;
		}

		case WM_ACTIVATE:
		{
			if (!bCursorEnabled)
			{
				if(wParam & WA_ACTIVE || wParam & WA_CLICKACTIVE)
				ConfineCursor();
			}
			else
			{
				FreeCursor();
			}
			break;
		}
		
		//Window
		case WM_SIZE:
		{
			SetSize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			Rendering::GetCurrent().OnWindowResize(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			break;
		}

		//RAW INPUT
		case WM_INPUT:
		{
			UINT size;
			//get the size of the input data
			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				nullptr,
				&size,
				sizeof(RAWINPUTHEADER)
				) == -1)
			{
				break;
			}

			rawBuffer.resize(size);

			if (GetRawInputData(
				reinterpret_cast<HRAWINPUT>(lParam),
				RID_INPUT,
				rawBuffer.data(),
				&size,
				sizeof(RAWINPUTHEADER)) != size)
			{
				break;
			}
			
			//process data
			auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
			if (ri.header.dwType == RIM_TYPEMOUSE &&
				(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
			{
				Mouse::Get().RawMouseMoveEvent(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
			}
			break;
		}

			//MOUSE
		case WM_MOUSEHWHEEL:
		{
			Mouse::Get().MouseWheelEvent(wParam, lParam);
			break;
		}
		case WM_LBUTTONDOWN:
		{
			Mouse::Get().LMouseButtonDownEvent(wParam, lParam);
			break;
		}
		case WM_LBUTTONUP:
		{
			Mouse::Get().LMouseButtonUpEvent(wParam, lParam);
			break;
		}
		case WM_RBUTTONDOWN:
		{
			Mouse::Get().RMouseButtonDownEvent(wParam, lParam);
			break;
		}
		case WM_RBUTTONUP:
		{
			Mouse::Get().RMouseButtonUpEvent(wParam, lParam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			if (ImGui::GetIO().WantCaptureMouse)
			{
				break;
			}

			Mouse::Get().MouseMoveEvent( wParam,  lParam, *this);
			break;
		}
		case WM_MBUTTONDOWN:
		{
			Mouse::Get().MouseWheelPressedEvent(wParam, lParam);
			break;
		}

			//KEYBOARD
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				break;
			}
			if (static_cast<unsigned char>(wParam) == VK_ESCAPE)
			{
				PostQuitMessage(0);
				return 0;
			}
			//m_Keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
			//if (!(lParam & 0x40000000) || m_Keyboard.AutorepeatIsEnabled())
			if (wParam == VK_LSHIFT)
			{
				auto bb = 100;
			}
			Keyboard::Get().OnKeyPressed(static_cast<unsigned char>(wParam));
			break;
		}
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (ImGui::GetIO().WantCaptureKeyboard)
			{
				break;
			}
			Keyboard::Get().OnKeyReleased((unsigned char)wParam);
			break;
		}
		case WM_CHAR:
		{
			Keyboard::Get().OnChar((unsigned char)wParam);
			break;
		}

	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

std::optional<int> Window::ProcessMessage()
{
	MSG message;

	while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
	{
		if (message.message == WM_QUIT)
		{
			return message.wParam;
		}

		TranslateMessage(&message);
		DispatchMessage(&message);

	}

	return std::nullopt;
}

///////////////////////////////////////////////////////////////////////////////////////WINCLASS/////////////////////////////////////////////////////


Window::WindowClass::WindowClass() noexcept
	: hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX winClass;
	winClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	winClass.lpfnWndProc = HandleMsgSetup;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hInstance = Window::WindowClass::GetInstance();
	winClass.hIcon = nullptr;
	winClass.hCursor = LoadCursor(hInst, MAKEINTRESOURCE(102));
	winClass.hbrBackground = nullptr;
	winClass.lpszMenuName = nullptr;
	winClass.lpszClassName = Window::WindowClass::GetName(); 
	winClass.hIconSm = nullptr;
	winClass.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&winClass);
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(Window::WindowClass::GetName(), Window::WindowClass::GetInstance());
}




//////////////////////////////////////////////////////////////////////EXCEPTION///////////////////////////////////////////////////////////////////


Window::WinException::WinException(int line, const char * file, HRESULT hr) noexcept :
	Exception(line, file), hr(hr)
{

}

const char* Window::WinException::what() const noexcept
{
	
	std::ostringstream ss;
	ss << GetType() << std::endl
		<< "[Error Code]" << GetErrorCode() << std::endl
		<< "[Description]" << GetErrorString() << std::endl
		<< GetOriginString();
	whatBuffer = ss.str();
	return whatBuffer.c_str();
}

const char* Window::WinException::GetType() const noexcept
{
	return "Window Error Exception";
}

std::string Window::WinException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
	);
	if (nMsgLen == 0)
	{
		return "Unindentified error code";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}

HRESULT Window::WinException::GetErrorCode() const noexcept
{
	return hr;
}

std::string Window::WinException::GetErrorString() const noexcept
{
	return TranslateErrorCode(hr);
}





