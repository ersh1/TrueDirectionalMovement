#pragma once
#include "API/OpenAnimationReplacerAPI-Functions.h"

namespace Functions
{
	class ToggleHeadtrackingFunction : public CustomFunction
	{
	public:
		constexpr static inline std::string_view FUNCTION_NAME = "ToggleHeadtracking"sv;

		ToggleHeadtrackingFunction();

		RE::BSString GetArgument() const override;
		RE::BSString GetName() const override { return FUNCTION_NAME.data(); }
		RE::BSString GetDescription() const override { return "Toggles headtracking on or off. Only works for the player, does nothing for other characters."sv.data(); }

		constexpr REL::Version GetRequiredVersion() const override { return { 2, 2, 7 }; }

		IBoolFunctionComponent* boolComponent;

	protected:
		bool RunImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_subMod, Trigger* a_trigger = nullptr) const override;
	};

	class ReleaseTargetLockFunction : public CustomFunction
	{
	public:
		constexpr static inline std::string_view FUNCTION_NAME = "ReleaseTargetLock"sv;

		RE::BSString GetName() const override { return FUNCTION_NAME.data(); }
		RE::BSString GetDescription() const override { return "Releases the current target lock, if active. Only works for the player, does nothing for other characters."sv.data(); }

		constexpr REL::Version GetRequiredVersion() const override { return { 2, 2, 7 }; }

	protected:
		bool RunImpl(RE::TESObjectREFR* a_refr, RE::hkbClipGenerator* a_clipGenerator, void* a_subMod, Trigger* a_trigger = nullptr) const override;
	};
}
