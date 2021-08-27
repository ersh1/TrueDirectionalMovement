#include "Papyrus.h"

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

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(TrueDirectionalMovement_MCM::Register);
		logger::info("Registered papyrus functions");
	}
}
