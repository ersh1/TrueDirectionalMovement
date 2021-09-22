#include "ViewHandler.h"
#include "Scaleform/TargetLockMenu.h"
#include "Scaleform/BossMenu.h"

#include "WidgetHandler.h"

void ViewHandler::SetVisible(bool a_visible)
{
	if (_menu) {
		switch (_menuType) {
			case kTargetLockMenu:
			{
				auto targetLockMenu = static_cast<Scaleform::TargetLockMenu*>(_menu);
				if (targetLockMenu) {
					if (a_visible) {
						targetLockMenu->Show();
					} else {
						targetLockMenu->Hide();
					}
				}
				break;
			}
			
			case kBossMenu:
			{
				auto bossMenu = static_cast<Scaleform::BossMenu*>(_menu);
				if (bossMenu) {
					if (a_visible) {
						bossMenu->Show();
					} else {
						bossMenu->Hide();
					}
				}
				break;
			}
		}
	}
	/*if (_view) {
		const auto prev = _view->GetVisible();
		if (prev != a_visible) {
			_view->SetVisible(a_visible);
		}
	} else {
		assert(false);
	}*/
}

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
