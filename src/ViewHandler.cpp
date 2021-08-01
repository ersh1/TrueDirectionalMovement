#include "ViewHandler.h"

#include "WidgetHandler.h"

void ViewHandler::Close()
{
	auto widgetHandler = WidgetHandler::GetSingleton();
	switch (_menuType) {
	case kTargetLockMenu:
		widgetHandler->CloseTargetLockMenu();
	case kBossMenu:
		widgetHandler->CloseBossMenu();
		break;
	}
}
