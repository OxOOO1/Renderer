#pragma once
#include <utility>

#include "System/Window.h"

class Mouse
{
	friend class Window;
	friend class Application;
public:

	static Mouse& Get()
	{
		static Mouse mouse;
		return mouse;
	}
	
	const DirectX::XMFLOAT2 GetRawOffset() const noexcept
	{
		return rawOffset;
	}

	const DirectX::XMFLOAT2 GetCursorPosition() const noexcept
	{
		return newPosition;
	}
	
	bool IsLButtonPressed() { return LButtonDown; }
	bool IsRButtonPressed() { return RButtonDown; }


	float sensitivity = 5.f;

protected:

	void FlushPosOffset()
	{
		positionDif = { 0.f, 0.f };
		rawOffset = { 0.f, 0.f };
	}

	void SetPosition(float x, float y) noexcept;

	void MouseMoveEvent(WPARAM wParam, LPARAM lParam, Window& window);

	void RawMouseMoveEvent(LONG x, LONG y);

	void LMouseButtonDownEvent(WPARAM wParam, LPARAM lParam);

	void LMouseButtonUpEvent(WPARAM wParam, LPARAM lParam);

	void RMouseButtonDownEvent(WPARAM wParam, LPARAM lParam);

	void RMouseButtonUpEvent(WPARAM wParam, LPARAM lParam);

	void MouseWheelEvent(WPARAM keycode, LPARAM lParam);

	void MouseWheelPressedEvent(WPARAM wParam, LPARAM lParam);

private:

	Mouse() = default;

	bool LButtonDown = false;
	bool RButtonDown = false;
	bool MButtonDown = false;

	DirectX::XMFLOAT2 oldPosition;
	DirectX::XMFLOAT2 newPosition;
	DirectX::XMFLOAT2 positionDif;

	DirectX::XMFLOAT2 rawOffset;

};