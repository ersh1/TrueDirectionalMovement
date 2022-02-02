#include "Settings.h"

#include "DirectionalMovementHandler.h"

void Settings::Initialize()
{
	logger::info("Initializing...");

	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		kywd_magicWard = dataHandler->LookupForm<RE::BGSKeyword>(0x1EA69, "Skyrim.esm");
		spel_targetLockSpell = dataHandler->LookupForm<RE::SpellItem>(0x805, "TrueDirectionalMovement.esp");
		glob_directionalMovement = dataHandler->LookupForm<RE::TESGlobal>(0x807, "TrueDirectionalMovement.esp");
		glob_targetLockHint = dataHandler->LookupForm<RE::TESGlobal>(0x808, "TrueDirectionalMovement.esp");
	}

	logger::info("...success");
}

void Settings::ReadSettings()
{
	constexpr auto defaultSettingsPath = L"Data/MCM/Config/TrueDirectionalMovement/settings.ini";
	constexpr auto mcmPath = L"Data/MCM/Settings/TrueDirectionalMovement.ini";

	const auto readMCM = [&](std::filesystem::path path) {
		CSimpleIniA mcm;
		mcm.SetUnicode();

		mcm.LoadFile(path.string().c_str());

		// General
		ReadBoolSetting(mcm, "General", "bDirectionalMovementSheathed", bDirectionalMovementSheathed);
		ReadBoolSetting(mcm, "General", "bDirectionalMovementDrawn", bDirectionalMovementDrawn);
		ReadUInt32Setting(mcm, "General", "uDialogueMode", (uint32_t&)uDialogueMode);
		ReadFloatSetting(mcm, "General", "fMeleeMagnetismAngle", fMeleeMagnetismAngle);

		// Directional Movement related
		ReadBoolSetting(mcm, "DirectionalMovement", "bFaceCrosshairWhileAttacking", bFaceCrosshairWhileAttacking);
		ReadBoolSetting(mcm, "DirectionalMovement", "bFaceCrosshairWhileShouting", bFaceCrosshairWhileShouting);
		ReadBoolSetting(mcm, "DirectionalMovement", "bFaceCrosshairWhileBlocking", bFaceCrosshairWhileBlocking);
		ReadBoolSetting(mcm, "DirectionalMovement", "bFaceCrosshairDuringAutoMove", bFaceCrosshairDuringAutoMove);
		ReadBoolSetting(mcm, "DirectionalMovement", "bStopOnDirectionChange", bStopOnDirectionChange);
		ReadUInt32Setting(mcm, "DirectionalMovement", "uAdjustCameraYawDuringMovement", (uint32_t&)uAdjustCameraYawDuringMovement);
		ReadFloatSetting(mcm, "DirectionalMovement", "fRunningRotationSpeedMult", fRunningRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fSprintingRotationSpeedMult", fSprintingRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fAttackStartRotationSpeedMult", fAttackStartRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fAttackMidRotationSpeedMult", fAttackMidRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fAttackEndRotationSpeedMult", fAttackEndRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fAirRotationSpeedMult", fAirRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fWaterRotationSpeedMult", fWaterRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fSwimmingRotationSpeedMult", fSwimmingRotationSpeedMult);
		ReadFloatSetting(mcm, "DirectionalMovement", "fFaceCrosshairRotationSpeedMultiplier", fFaceCrosshairRotationSpeedMultiplier);
		ReadBoolSetting(mcm, "DirectionalMovement", "bFaceCrosshairInstantly", bFaceCrosshairInstantly);
		ReadFloatSetting(mcm, "DirectionalMovement", "fCameraAutoAdjustDelay", fCameraAutoAdjustDelay);
		ReadFloatSetting(mcm, "DirectionalMovement", "fCameraAutoAdjustSpeedMult", fCameraAutoAdjustSpeedMult);
		ReadBoolSetting(mcm, "DirectionalMovement", "bIgnoreSlowTime", bIgnoreSlowTime);
		ReadBoolSetting(mcm, "DirectionalMovement", "bDisableAttackRotationMultipliersForTransformations", bDisableAttackRotationMultipliersForTransformations);
		ReadFloatSetting(mcm, "DirectionalMovement", "fSwimmingPitchSpeed", fSwimmingPitchSpeed);
		ReadBoolSetting(mcm, "DirectionalMovement", "bThumbstickBounceFix", bThumbstickBounceFix);

		// Headtracking
		ReadBoolSetting(mcm, "Headtracking", "bHeadtracking", bHeadtracking);
		ReadBoolSetting(mcm, "Headtracking", "bHeadtrackSpine", bHeadtrackSpine);
		ReadFloatSetting(mcm, "Headtracking", "fDialogueHeadtrackingDuration", fDialogueHeadtrackingDuration);
		ReadBoolSetting(mcm, "Headtracking", "bCameraHeadtracking", bCameraHeadtracking);
		ReadFloatSetting(mcm, "Headtracking", "fCameraHeadtrackingStrength", fCameraHeadtrackingStrength);
		ReadFloatSetting(mcm, "Headtracking", "fCameraHeadtrackingDuration", fCameraHeadtrackingDuration);
		ReadUInt32Setting(mcm, "Headtracking", "uCameraHeadtrackingMode", (uint32_t&)uCameraHeadtrackingMode);

		// Target Lock
		ReadBoolSetting(mcm, "TargetLock", "bAutoTargetNextOnDeath", bAutoTargetNextOnDeath);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockTestLOS", bTargetLockTestLOS);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockHostileActorsOnly", bTargetLockHostileActorsOnly);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockHideCrosshair", bTargetLockHideCrosshair);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockDistance", fTargetLockDistance);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockDistanceMultiplierSmall", fTargetLockDistanceMultiplierSmall);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockDistanceMultiplierLarge", fTargetLockDistanceMultiplierLarge);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockDistanceMultiplierExtraLarge", fTargetLockDistanceMultiplierExtraLarge);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockPitchAdjustSpeed", fTargetLockPitchAdjustSpeed);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockYawAdjustSpeed", fTargetLockYawAdjustSpeed);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockPitchOffsetStrength", fTargetLockPitchOffsetStrength);
		ReadUInt32Setting(mcm, "TargetLock", "uTargetLockArrowAimType", (uint32_t&)uTargetLockArrowAimType);
		ReadUInt32Setting(mcm, "TargetLock", "uTargetLockMissileAimType", (uint32_t&)uTargetLockMissileAimType);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockUsePOVSwitchKeyboard", bTargetLockUsePOVSwitchKeyboard);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockUsePOVSwitchGamepad", bTargetLockUsePOVSwitchGamepad);
		ReadFloatSetting(mcm, "TargetLock", "fTargetLockPOVHoldDuration", fTargetLockPOVHoldDuration);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockUseMouse", bTargetLockUseMouse);
		ReadUInt32Setting(mcm, "TargetLock", "uTargetLockMouseSensitivity", (uint32_t&)uTargetLockMouseSensitivity);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockUseScrollWheel", bTargetLockUseScrollWheel);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockUseRightThumbstick", bTargetLockUseRightThumbstick);
		ReadBoolSetting(mcm, "TargetLock", "bResetCameraWithTargetLock", bResetCameraWithTargetLock);
		ReadBoolSetting(mcm, "TargetLock", "bTargetLockEnableHint", bTargetLockEnableHint);

		// HUD
		ReadBoolSetting(mcm, "HUD", "bEnableTargetLockReticle", bEnableTargetLockReticle);
		ReadUInt32Setting(mcm, "HUD", "uReticleAnchor", (uint32_t&)uReticleAnchor);
		ReadUInt32Setting(mcm, "HUD", "uReticleStyle", (uint32_t&)uReticleStyle);
		ReadFloatSetting(mcm, "HUD", "fReticleScale", fReticleScale);
		ReadBoolSetting(mcm, "HUD", "bReticleUseHUDOpacity", bReticleUseHUDOpacity);
		ReadFloatSetting(mcm, "HUD", "fReticleOpacity", fReticleOpacity);

		// Keys
		ReadUInt32Setting(mcm, "Keys", "uTargetLockKey", uTargetLockKey);
		ReadUInt32Setting(mcm, "Keys", "uSwitchTargetLeftKey", uSwitchTargetLeftKey);
		ReadUInt32Setting(mcm, "Keys", "uSwitchTargetRightKey", uSwitchTargetRightKey);
	};

	logger::info("Reading MCM .ini...");

	readMCM(defaultSettingsPath);  // read the default ini first
	readMCM(mcmPath);

	logger::info("...success");

	DirectionalMovementHandler::GetSingleton()->OnSettingsUpdated();
}

void Settings::ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound)
	{
		a_setting = a_ini.GetBoolValue(a_sectionName, a_settingName);
	}
}

void Settings::ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		a_setting = static_cast<float>(a_ini.GetDoubleValue(a_sectionName, a_settingName));
	}
}

void Settings::ReadUInt32Setting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting)
{
	const char* bFound = nullptr;
	bFound = a_ini.GetValue(a_sectionName, a_settingName);
	if (bFound) {
		a_setting = static_cast<uint32_t>(a_ini.GetLongValue(a_sectionName, a_settingName));
	}
}
