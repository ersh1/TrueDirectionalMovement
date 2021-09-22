#include "Settings.h"

#include "DirectionalMovementHandler.h"
#include "WidgetHandler.h"

void Settings::ReadSettings()
{
	constexpr auto path = L"Data/MCM/Settings/TrueDirectionalMovement.ini";

	CSimpleIniA ini;
	ini.SetUnicode();

	ini.LoadFile(path);

	// General
	ReadBoolSetting(ini, "General", "bDirectionalMovementSheathed", bDirectionalMovementSheathed);
	ReadBoolSetting(ini, "General", "bDirectionalMovementDrawn", bDirectionalMovementDrawn);
	ReadUInt32Setting(ini, "General", "uDialogueMode", (uint32_t&)uDialogueMode);
	ReadFloatSetting(ini, "General", "fMeleeMagnetismAngle", fMeleeMagnetismAngle);

	// Directional Movement related
	ReadBoolSetting(ini, "DirectionalMovement", "bFaceCrosshairWhileAttacking", bFaceCrosshairWhileAttacking);
	ReadBoolSetting(ini, "DirectionalMovement", "bFaceCrosshairWhileShouting", bFaceCrosshairWhileShouting);
	ReadBoolSetting(ini, "DirectionalMovement", "bFaceCrosshairWhileBlocking", bFaceCrosshairWhileBlocking);
	ReadBoolSetting(ini, "DirectionalMovement", "bFaceCrosshairDuringAutoMove", bFaceCrosshairDuringAutoMove);
	ReadBoolSetting(ini, "DirectionalMovement", "bStopOnDirectionChange", bStopOnDirectionChange);
	ReadFloatSetting(ini, "DirectionalMovement", "fRunningRotationSpeedMult", fRunningRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fSprintingRotationSpeedMult", fSprintingRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fAttackStartRotationSpeedMult", fAttackStartRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fAttackMidRotationSpeedMult", fAttackMidRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fAttackEndRotationSpeedMult", fAttackEndRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fAirRotationSpeedMult", fAirRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fWaterRotationSpeedMult", fWaterRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fSwimmingRotationSpeedMult", fSwimmingRotationSpeedMult);
	ReadFloatSetting(ini, "DirectionalMovement", "fFaceCrosshairRotationSpeedMultiplier", fFaceCrosshairRotationSpeedMultiplier);
	ReadBoolSetting(ini, "DirectionalMovement", "bFaceCrosshairInstantly", bFaceCrosshairInstantly);
	ReadBoolSetting(ini, "DirectionalMovement", "bDisableAttackRotationMultipliersForTransformations", bDisableAttackRotationMultipliersForTransformations);
	ReadFloatSetting(ini, "DirectionalMovement", "fSwimmingPitchSpeed", fSwimmingPitchSpeed);
	ReadBoolSetting(ini, "DirectionalMovement", "bThumbstickBounceFix", bThumbstickBounceFix);

	// Headtracking
	ReadBoolSetting(ini, "Headtracking", "bHeadtracking", bHeadtracking);
	ReadBoolSetting(ini, "Headtracking", "bHeadtrackSpine", bHeadtrackSpine);
	ReadFloatSetting(ini, "Headtracking", "fDialogueHeadtrackingDuration", fDialogueHeadtrackingDuration);
	ReadBoolSetting(ini, "Headtracking", "bCameraHeadtracking", bCameraHeadtracking);
	ReadFloatSetting(ini, "Headtracking", "fCameraHeadtrackingStrength", fCameraHeadtrackingStrength);
	ReadFloatSetting(ini, "Headtracking", "fCameraHeadtrackingDuration", fCameraHeadtrackingDuration);
	ReadUInt32Setting(ini, "Headtracking", "uCameraHeadtrackingMode", (uint32_t&)uCameraHeadtrackingMode);

	// Target Lock
	ReadBoolSetting(ini, "TargetLock", "bAutoTargetNextOnDeath", bAutoTargetNextOnDeath);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockTestLOS", bTargetLockTestLOS);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockHostileActorsOnly", bTargetLockHostileActorsOnly);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockHideCrosshair", bTargetLockHideCrosshair);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockDistance", fTargetLockDistance);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockDistanceMultiplierSmall", fTargetLockDistanceMultiplierSmall);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockDistanceMultiplierLarge", fTargetLockDistanceMultiplierLarge);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockDistanceMultiplierExtraLarge", fTargetLockDistanceMultiplierExtraLarge);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockPitchAdjustSpeed", fTargetLockPitchAdjustSpeed);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockYawAdjustSpeed", fTargetLockYawAdjustSpeed);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockPitchOffsetStrength", fTargetLockPitchOffsetStrength);
	ReadUInt32Setting(ini, "TargetLock", "uTargetLockArrowAimType", (uint32_t&)uTargetLockArrowAimType);
	ReadUInt32Setting(ini, "TargetLock", "uTargetLockMissileAimType", (uint32_t&)uTargetLockMissileAimType);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockUsePOVSwitch", bTargetLockUsePOVSwitch);
	ReadFloatSetting(ini, "TargetLock", "fTargetLockPOVHoldDuration", fTargetLockPOVHoldDuration);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockUseMouse", bTargetLockUseMouse);
	ReadUInt32Setting(ini, "TargetLock", "uTargetLockMouseSensitivity", (uint32_t&)uTargetLockMouseSensitivity);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockUseScrollWheel", bTargetLockUseScrollWheel);
	ReadBoolSetting(ini, "TargetLock", "bTargetLockUseRightThumbstick", bTargetLockUseRightThumbstick);
	ReadBoolSetting(ini, "TargetLock", "bResetCameraWithTargetLock", bResetCameraWithTargetLock);

	// Target Lock Widget
	ReadBoolSetting(ini, "TargetLockWidget", "bShowReticle", bShowReticle);
	ReadBoolSetting(ini, "TargetLockWidget", "bShowTargetBar", bShowTargetBar);
	ReadBoolSetting(ini, "TargetLockWidget", "bShowSoftTargetBar", bShowSoftTargetBar);
	ReadBoolSetting(ini, "TargetLockWidget", "bShowTargetName", bShowTargetName);
	ReadBoolSetting(ini, "TargetLockWidget", "bShowHealthPhantom", bShowHealthPhantom);
	ReadBoolSetting(ini, "TargetLockWidget", "bShowDamage", bShowDamage);
	ReadBoolSetting(ini, "TargetLockWidget", "bHideVanillaTargetBar", bHideVanillaTargetBar);
	ReadUInt32Setting(ini, "TargetLockWidget", "uReticleAnchor", (uint32_t&)uReticleAnchor);
	ReadUInt32Setting(ini, "TargetLockWidget", "uReticleStyle", (uint32_t&)uReticleStyle);
	ReadUInt32Setting(ini, "TargetLockWidget", "uTargetBarAnchor", (uint32_t&)uTargetBarAnchor);
	ReadFloatSetting(ini, "TargetLockWidget", "fTargetBarZOffset", fTargetBarZOffset);
	ReadUInt32Setting(ini, "TargetLockWidget", "uTargetLevelMode", (uint32_t&)uTargetLevelMode);
	ReadUInt32Setting(ini, "TargetLockWidget", "uTargetLevelThreshold", (uint32_t&)uTargetLevelThreshold);
	ReadFloatSetting(ini, "TargetLockWidget", "fHealthPhantomDuration", fHealthPhantomDuration);
	ReadFloatSetting(ini, "TargetLockWidget", "fDamageDuration", fDamageDuration);
	ReadFloatSetting(ini, "TargetLockWidget", "fReticleScale", fReticleScale);
	ReadBoolSetting(ini, "TargetLockWidget", "bUseHUDOpacityForReticle", bUseHUDOpacityForReticle);
	ReadFloatSetting(ini, "TargetLockWidget", "fReticleOpacity", fReticleOpacity);
	ReadFloatSetting(ini, "TargetLockWidget", "fTargetBarScale", fTargetBarScale);
	ReadBoolSetting(ini, "TargetLockWidget", "bUseHUDOpacityForTargetBar", bUseHUDOpacityForTargetBar);
	ReadFloatSetting(ini, "TargetLockWidget", "fTargetBarOpacity", fTargetBarOpacity);

	// Boss Bar Widget
	ReadBoolSetting(ini, "BossBarWidget", "bShowBossBar", bShowBossBar);
	ReadBoolSetting(ini, "BossBarWidget", "bShowBossHealthPhantom", bShowBossHealthPhantom);
	ReadBoolSetting(ini, "BossBarWidget", "bShowBossDamage", bShowBossDamage);
	ReadBoolSetting(ini, "BossBarWidget", "bBossHideVanillaTargetBar", bBossHideVanillaTargetBar);
	ReadUInt32Setting(ini, "BossBarWidget", "uBossNameAlignment", (uint32_t&)uBossNameAlignment);
	ReadUInt32Setting(ini, "BossBarWidget", "uBossLevelMode", (uint32_t&)uBossLevelMode);
	ReadUInt32Setting(ini, "BossBarWidget", "uBossLevelThreshold", (uint32_t&)uBossLevelThreshold);
	ReadFloatSetting(ini, "BossBarWidget", "fBossHealthPhantomDuration", fBossHealthPhantomDuration);
	ReadFloatSetting(ini, "BossBarWidget", "fBossDamageDuration", fBossDamageDuration);
	ReadFloatSetting(ini, "BossBarWidget", "fBossBarScale", fBossBarScale);
	ReadBoolSetting(ini, "BossBarWidget", "bUseHUDOpacityForBossBar", bUseHUDOpacityForBossBar);
	ReadFloatSetting(ini, "BossBarWidget", "fBossBarOpacity", fBossBarOpacity);
	ReadFloatSetting(ini, "BossBarWidget", "fBossBarX", fBossBarX);
	ReadFloatSetting(ini, "BossBarWidget", "fBossBarY", fBossBarY);
	ReadBoolSetting(ini, "BossBarWidget", "bBossBarOffsetSubtitles", bBossBarOffsetSubtitles);
	ReadFloatSetting(ini, "BossBarWidget", "fMultipleBossBarsOffset", fMultipleBossBarsOffset);
	ReadBoolSetting(ini, "BossBarWidget", "bMultipleBossBarsStackUpwards", bMultipleBossBarsStackUpwards);

	// Keys
	ReadUInt32Setting(ini, "Keys", "uTargetLockKey", (uint32_t&)uTargetLockKey);
	ReadUInt32Setting(ini, "Keys", "uSwitchTargetLeftKey", (uint32_t&)uSwitchTargetLeftKey);
	ReadUInt32Setting(ini, "Keys", "uSwitchTargetRightKey", (uint32_t&)uSwitchTargetRightKey);

	DirectionalMovementHandler::GetSingleton()->OnSettingsUpdated();
	WidgetHandler::GetSingleton()->OnSettingsUpdated();
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
