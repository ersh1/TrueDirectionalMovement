#include "OARFunctions.h"
#include "DirectionalMovementHandler.h"

namespace Functions
{
	ToggleHeadtrackingFunction::ToggleHeadtrackingFunction()
	{
		boolComponent = static_cast<IBoolFunctionComponent*>(AddBaseComponent(FunctionComponentType::kBool, "Toggle off or back on"));
	}

	RE::BSString ToggleHeadtrackingFunction::GetArgument() const
	{
		return boolComponent->GetBoolValue() ? "Toggle back on" : "Toggle off";
	}

	bool ToggleHeadtrackingFunction::RunImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator*, void*, Trigger*) const
	{
		if (a_refr != RE::PlayerCharacter::GetSingleton()) {
			return false;
		}

		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		directionalMovementHandler->SetOARDisableHeadtracking(!boolComponent->GetBoolValue());

		return true;
	}

	bool ReleaseTargetLockFunction::RunImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator*, void*, Trigger*) const
	{
		if (a_refr != RE::PlayerCharacter::GetSingleton()) {
			return false;
		}

		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		return directionalMovementHandler->ToggleTargetLock(false);
	}
}
