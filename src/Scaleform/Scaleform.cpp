#include "Scaleform/Scaleform.h"

#include "Scaleform/TargetLockMenu.h"
#include "Scaleform/BossMenu.h"

namespace Scaleform
{
	void Register()
	{
		TargetLockMenu::Register();
		BossMenu::Register();
		logger::info("Registered all movies");
	}
}

