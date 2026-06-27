#include "Papyrus.h"

#include "DirectionalMovementHandler.h"
#include "Settings.h"

namespace Papyrus
{
	void TrueDirectionalMovement_MCM::OnConfigClose(RE::TESQuest*)
	{
		Settings::ReadSettings();
	}

	bool TrueDirectionalMovement_MCM::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("OnConfigClose", "TrueDirectionalMovement_MCM", OnConfigClose);

		logger::info("Registered TrueDirectionalMovement_MCM class");
		return true;
	}

    bool TrueDirectionalMovement::GetDirectionalMovementState(RE::StaticFunctionTag*)
	{
	    return DirectionalMovementHandler::GetSingleton()->IsFreeCamera();
	}

    bool TrueDirectionalMovement::GetTargetLockState(RE::StaticFunctionTag*)
	{
		return DirectionalMovementHandler::GetSingleton()->HasTargetLocked();
	}

    RE::Actor* TrueDirectionalMovement::GetCurrentTarget(RE::StaticFunctionTag*)
	{
		if (const auto currentTarget = DirectionalMovementHandler::GetSingleton()->GetTarget()) {
			return currentTarget.get().get();
		}

		return nullptr;
	}

    void TrueDirectionalMovement::ToggleDisableDirectionalMovement(RE::StaticFunctionTag*, RE::BSFixedString a_modName, bool a_bDisable)
	{
		if (a_modName.empty()) {
		    return;
		}

		DirectionalMovementHandler::GetSingleton()->PapyrusDisableDirectionalMovement(a_modName, a_bDisable);
	}

    void TrueDirectionalMovement::ToggleDisableTargetLock(RE::StaticFunctionTag*, RE::BSFixedString a_modName, bool a_bDisable)
	{
		if (a_modName.empty()) {
		    return;
		}

		DirectionalMovementHandler::GetSingleton()->PapyrusDisableTargetLock(a_modName, a_bDisable);
	}

    void TrueDirectionalMovement::ToggleDisableHeadtracking(RE::StaticFunctionTag*, RE::BSFixedString a_modName, bool a_bDisable)
	{
		if (a_modName.empty()) {
			return;
		}

		DirectionalMovementHandler::GetSingleton()->PapyrusDisableHeadtracking(a_modName, a_bDisable);
	}

    bool TrueDirectionalMovement::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetDirectionalMovementState", "TrueDirectionalMovement", GetDirectionalMovementState);
		a_vm->RegisterFunction("GetTargetLockState", "TrueDirectionalMovement", GetTargetLockState);
		a_vm->RegisterFunction("GetCurrentTarget", "TrueDirectionalMovement", GetCurrentTarget);
		a_vm->RegisterFunction("ToggleDisableDirectionalMovement", "TrueDirectionalMovement", ToggleDisableDirectionalMovement);
		a_vm->RegisterFunction("ToggleDisableTargetLock", "TrueDirectionalMovement", ToggleDisableTargetLock);
		a_vm->RegisterFunction("ToggleDisableHeadtracking", "TrueDirectionalMovement", ToggleDisableHeadtracking);

		logger::info("Registered TrueDirectionalMovement class");
		return true;
	}

    void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(TrueDirectionalMovement_MCM::Register);
		papyrus->Register(TrueDirectionalMovement::Register);
		logger::info("Registered papyrus functions");
	}
}
