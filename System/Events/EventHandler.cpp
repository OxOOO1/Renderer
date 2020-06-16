#include "EventHandler.h"

#include "System/Application.h"

bool EventHandler::Dispatch(Event & event)
{
	return Application::Get().OnEvent(event);
}
