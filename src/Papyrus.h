#pragma once

namespace Papyrus
{
	class TrueDirectionalMovement_MCM
	{
	public:
		static void OnConfigClose(RE::TESQuest*);

		static bool Register(RE::BSScript::IVirtualMachine* a_vm);
	};

	class TrueDirectionalMovement
	{
	public:
		static bool GetDirectionalMovementState(RE::StaticFunctionTag*);
		static bool GetTargetLockState(RE::StaticFunctionTag*);
		static RE::Actor* GetCurrentTarget(RE::StaticFunctionTag*);
		static void ToggleDisableDirectionalMovement(RE::StaticFunctionTag*, RE::BSFixedString a_modName, bool a_bDisable);
		static void ToggleDisableTargetLock(RE::StaticFunctionTag*, RE::BSFixedString a_modName, bool a_bDisable);
		static void ToggleDisableHeadtracking(RE::StaticFunctionTag*, RE::BSFixedString a_modName, bool a_bDisable);

		static bool Register(RE::BSScript::IVirtualMachine* a_vm);
	};

	void Register();
}
