#pragma once

#include <Windows.h>
#include <optional>
#include <memory>
#include "Exception.h"

#include "Rendering.h"

#include "System/Events/Event.h"
#include "System/Events/KeyboardEvents.h"
#include "System/Events/WindowEvents.h"

constexpr const char* window_Name = "Direct Window\0";


class Window
{
public:
	
	void OnEvent(Event& event)
	{
		if (event.CheckTypeAndHandle(EventType::KeyboardEvent))
		{
			OnKeyboardEvent(reinterpret_cast<KeyboardEvent&>(event));
		}
	}

	void OnKeyboardEvent(KeyboardEvent& event);

private:

	class WindowClass
	{
	public:
		static const char* GetName() noexcept { return wndClassName; };
		static HINSTANCE GetInstance() noexcept { return Get().hInst; }
		static WindowClass& Get()
		{
			return winClassInstance;
		}
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "DirectX Window";
		static WindowClass winClassInstance;
		HINSTANCE hInst;
	};

public:
	Window(int width, int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	void SetTitle(const std::string& title);
	HWND GetHandle() const noexcept { return hWnd; }
	std::pair<int, int> GetSize() const { return { width, height }; }
	inline void SetSize(int _width, int _height) { width = _width; height = _height; }
	static std::optional<int> ProcessMessage();
	void EnableCursor();
	void DisableCursor();
private:
	void ConfineCursor();
	void FreeCursor();
	void HideCursor();
	void ShowCursor();
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI GetPtrToMessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
public:
	bool bCursorEnabled = false;
	
private:
	int width, height;
	HWND hWnd;
	std::vector<char> rawBuffer;

public:
	class WinException : public Exception
	{
	public:
		WinException(int line, const char* file, HRESULT hr) noexcept;
		const char* what() const noexcept override;
		virtual const char* GetType() const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;

	private:
		HRESULT hr;
	};
};

#define WND_EXCEPT(hr) Window::WinException(__LINE__, __FILE__, hr)
#define WND_EXCEPT_LAST(hr) Window::WinException(__LINE__, __FILE__, GetLastError())