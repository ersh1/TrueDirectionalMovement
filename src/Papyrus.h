#pragma once

namespace Papyrus
{
	class TrueDirectionalMovement
	{
	public:
		static bool GetFreeCamera(RE::StaticFunctionTag*);
		static void SetFreeCamera(RE::StaticFunctionTag*, bool a_enable);
		static uint32_t GetDialogueMode(RE::StaticFunctionTag*);
		static void SetDialogueMode(RE::StaticFunctionTag*, uint32_t a_mode);
		static bool GetHeadtracking(RE::StaticFunctionTag*);
		static void SetHeadtracking(RE::StaticFunctionTag*, bool a_enable);
		static float GetDialogueHeadtrackingDuration(RE::StaticFunctionTag*);
		static void SetDialogueHeadtrackingDuration(RE::StaticFunctionTag*, float a_duration);
		static bool GetCameraHeadtracking(RE::StaticFunctionTag*);
		static void SetCameraHeadtracking(RE::StaticFunctionTag*, bool a_enable);
		static float GetCameraHeadtrackingStrength(RE::StaticFunctionTag*);
		static void SetCameraHeadtrackingStrength(RE::StaticFunctionTag*, float a_strength);
		static bool GetStopCameraHeadtrackingBehindPlayer(RE::StaticFunctionTag*);
		static void SetStopCameraHeadtrackingBehindPlayer(RE::StaticFunctionTag*, bool a_enable);
		static bool GetFaceCrosshairWhileBlocking(RE::StaticFunctionTag*);
		static void SetFaceCrosshairWhileBlocking(RE::StaticFunctionTag*, bool a_enable);
		static float GetRunningRotationSpeedMult(RE::StaticFunctionTag*);
		static void SetRunningRotationSpeedMult(RE::StaticFunctionTag*, float a_mult);
		static float GetSprintingRotationSpeedMult(RE::StaticFunctionTag*);
		static void SetSprintingRotationSpeedMult(RE::StaticFunctionTag*, float a_mult);
		static float GetAttackStartRotationSpeedMult(RE::StaticFunctionTag*);
		static void SetAttackStartRotationSpeedMult(RE::StaticFunctionTag*, float a_mult);
		static float GetAttackMidRotationSpeedMult(RE::StaticFunctionTag*);
		static void SetAttackMidRotationSpeedMult(RE::StaticFunctionTag*, float a_mult);
		static float GetAttackEndRotationSpeedMult(RE::StaticFunctionTag*);
		static void SetAttackEndRotationSpeedMult(RE::StaticFunctionTag*, float a_mult);
		static float GetAirRotationSpeedMult(RE::StaticFunctionTag*);
		static void SetAirRotationSpeedMult(RE::StaticFunctionTag*, float a_mult);
		static bool GetFaceCrosshairInstantly(RE::StaticFunctionTag*);
		static void SetFaceCrosshairInstantly(RE::StaticFunctionTag*, bool a_enable);
		static bool GetDisableAttackRotationMultipliersForTransformations(RE::StaticFunctionTag*);
		static void SetDisableAttackRotationMultipliersForTransformations(RE::StaticFunctionTag*, bool a_enable);
		static bool GetStopOnDirectionChange(RE::StaticFunctionTag*);
		static void SetStopOnDirectionChange(RE::StaticFunctionTag*, bool a_enable);

		static float GetTargetLockDistance(RE::StaticFunctionTag*);
		static void SetTargetLockDistance(RE::StaticFunctionTag*, float a_distance);
		static float GetTargetLockPitchAdjustSpeed(RE::StaticFunctionTag*);
		static void SetTargetLockPitchAdjustSpeed(RE::StaticFunctionTag*, float a_speed);
		static float GetTargetLockYawAdjustSpeed(RE::StaticFunctionTag*);
		static void SetTargetLockYawAdjustSpeed(RE::StaticFunctionTag*, float a_speed);
		static float GetTargetLockPitchOffsetStrength(RE::StaticFunctionTag*);
		static void SetTargetLockPitchOffsetStrength(RE::StaticFunctionTag*, float a_offsetStrength);
		static bool GetTargetLockUseMouse(RE::StaticFunctionTag*);
		static void SetTargetLockUseMouse(RE::StaticFunctionTag*, bool a_enable);
		static bool GetTargetLockUseScrollWheel(RE::StaticFunctionTag*);
		static void SetTargetLockUseScrollWheel(RE::StaticFunctionTag*, bool a_enable);
		static uint32_t GetTargetLockArrowAimType(RE::StaticFunctionTag*);
		static void SetTargetLockArrowAimType(RE::StaticFunctionTag*, uint32_t a_type);
		static uint32_t GetTargetLockMissileAimType(RE::StaticFunctionTag*);
		static void SetTargetLockMissileAimType(RE::StaticFunctionTag*, uint32_t a_type);
		static bool GetAutoTargetNextOnDeath(RE::StaticFunctionTag*);
		static void SetAutoTargetNextOnDeath(RE::StaticFunctionTag*, bool a_enable);
		static bool GetTargetLockTestLOS(RE::StaticFunctionTag*);
		static void SetTargetLockTestLOS(RE::StaticFunctionTag*, bool a_enable);
		static bool GetTargetLockHostileActorsOnly(RE::StaticFunctionTag*);
		static void SetTargetLockHostileActorsOnly(RE::StaticFunctionTag*, bool a_enable);

		static bool GetWidgetShowReticle(RE::StaticFunctionTag*);
		static void SetWidgetShowReticle(RE::StaticFunctionTag*, bool a_show);
		static uint32_t GetWidgetReticleStyle(RE::StaticFunctionTag*);
		static void SetWidgetReticleStyle(RE::StaticFunctionTag*, uint32_t a_style);
		static bool GetWidgetShowTargetBar(RE::StaticFunctionTag*);
		static void SetWidgetShowTargetBar(RE::StaticFunctionTag*, bool a_show);
		static bool GetWidgetShowSoftTargetBar(RE::StaticFunctionTag*);
		static void SetWidgetShowSoftTargetBar(RE::StaticFunctionTag*, bool a_show);
		static bool GetWidgetShowTargetName(RE::StaticFunctionTag*);
		static void SetWidgetShowTargetName(RE::StaticFunctionTag*, bool a_show);
		static uint32_t GetWidgetTargetLevelMode(RE::StaticFunctionTag*);
		static void SetWidgetTargetLevelMode(RE::StaticFunctionTag*, uint32_t a_mode);
		static uint32_t GetWidgetTargetLevelThreshold(RE::StaticFunctionTag*);
		static void SetWidgetTargetLevelThreshold(RE::StaticFunctionTag*, uint32_t a_threshold);
		static bool GetWidgetShowDamage(RE::StaticFunctionTag*);
		static void SetWidgetShowDamage(RE::StaticFunctionTag*, bool a_hide);
		static bool GetWidgetShowHealthPhantom(RE::StaticFunctionTag*);
		static void SetWidgetShowHealthPhantom(RE::StaticFunctionTag*, bool a_show);
		static bool GetWidgetHideVanillaTargetBar(RE::StaticFunctionTag*);
		static void SetWidgetHideVanillaTargetBar(RE::StaticFunctionTag*, bool a_show);
		static float GetWidgetHealthPhantomDuration(RE::StaticFunctionTag*);
		static void SetWidgetHealthPhantomDuration(RE::StaticFunctionTag*, float a_duration);
		static float GetWidgetDamageDuration(RE::StaticFunctionTag*);
		static void SetWidgetDamageDuration(RE::StaticFunctionTag*, float a_duration);
		static float GetWidgetReticleScale(RE::StaticFunctionTag*);
		static void SetWidgetReticleScale(RE::StaticFunctionTag*, float a_scale);
		static float GetWidgetTargetBarScale(RE::StaticFunctionTag*);
		static void SetWidgetTargetBarScale(RE::StaticFunctionTag*, float a_scale);
		static bool GetWidgetUseHUDOpacityForReticle(RE::StaticFunctionTag*);
		static void SetWidgetUseHUDOpacityForReticle(RE::StaticFunctionTag*, bool a_enable);
		static float GetWidgetReticleOpacity(RE::StaticFunctionTag*);
		static void SetWidgetReticleOpacity(RE::StaticFunctionTag*, float a_opacity);
		static bool GetWidgetUseHUDOpacityForTargetBar(RE::StaticFunctionTag*);
		static void SetWidgetUseHUDOpacityForTargetBar(RE::StaticFunctionTag*, bool a_enable);
		static float GetWidgetTargetBarOpacity(RE::StaticFunctionTag*);
		static void SetWidgetTargetBarOpacity(RE::StaticFunctionTag*, float a_opacity);

		static bool GetShowBossBar(RE::StaticFunctionTag*);
		static void SetShowBossBar(RE::StaticFunctionTag*, bool a_show);
		static uint32_t GetBossNameAlignment(RE::StaticFunctionTag*);
		static void SetBossNameAlignment(RE::StaticFunctionTag*, uint32_t a_alignment);
		static uint32_t GetBossLevelMode(RE::StaticFunctionTag*);
		static void SetBossLevelMode(RE::StaticFunctionTag*, uint32_t a_mode);
		static uint32_t GetBossLevelThreshold(RE::StaticFunctionTag*);
		static void SetBossLevelThreshold(RE::StaticFunctionTag*, uint32_t a_threshold);
		static bool GetShowBossDamage(RE::StaticFunctionTag*);
		static void SetShowBossDamage(RE::StaticFunctionTag*, bool a_show);
		static bool GetShowBossHealthPhantom(RE::StaticFunctionTag*);
		static void SetShowBossHealthPhantom(RE::StaticFunctionTag*, bool a_show);
		static float GetBossHealthPhantomDuration(RE::StaticFunctionTag*);
		static void SetBossHealthPhantomDuration(RE::StaticFunctionTag*, float a_duration);
		static float GetBossDamageDuration(RE::StaticFunctionTag*);
		static void SetBossDamageDuration(RE::StaticFunctionTag*, float a_duration);
		static float GetBossBarScale(RE::StaticFunctionTag*);
		static void SetBossBarScale(RE::StaticFunctionTag*, float a_scale);
		static bool GetUseHUDOpacityForBossBar(RE::StaticFunctionTag*);
		static void SetUseHUDOpacityForBossBar(RE::StaticFunctionTag*, bool a_enable);
		static float GetBossBarOpacity(RE::StaticFunctionTag*);
		static void SetBossBarOpacity(RE::StaticFunctionTag*, float a_opacity);
		static float GetBossBarX(RE::StaticFunctionTag*);
		static void SetBossBarX(RE::StaticFunctionTag*, float a_position);
		static float GetBossBarY(RE::StaticFunctionTag*);
		static void SetBossBarY(RE::StaticFunctionTag*, float a_position);
		static bool GetBossBarOffsetSubtitles(RE::StaticFunctionTag*);
		static void SetBossBarOffsetSubtitles(RE::StaticFunctionTag*, bool a_enable);
		static float GetMultipleBossBarsOffset(RE::StaticFunctionTag*);
		static void SetMultipleBossBarsOffset(RE::StaticFunctionTag*, float a_offset);
		static bool GetMultipleBossBarsStackUpwards(RE::StaticFunctionTag*);
		static void SetMultipleBossBarsStackUpwards(RE::StaticFunctionTag*, bool a_enable);

		static uint32_t GetTargetLockKey(RE::StaticFunctionTag*);
		static void SetTargetLockKey(RE::StaticFunctionTag*, uint32_t a_key);
		static uint32_t GetSwitchTargetLeftKey(RE::StaticFunctionTag*);
		static void SetSwitchTargetLeftKey(RE::StaticFunctionTag*, uint32_t a_key);
		static uint32_t GetSwitchTargetRightKey(RE::StaticFunctionTag*);
		static void SetSwitchTargetRightKey(RE::StaticFunctionTag*, uint32_t a_key);

		static bool Register(RE::BSScript::IVirtualMachine* a_vm);
	};

	void Register();
}
