#include "Papyrus.h"

#include "Events.h"
#include "DirectionalMovementHandler.h"
#include "WidgetHandler.h"

namespace Papyrus
{
	bool TrueDirectionalMovement::GetFreeCamera(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetFreeCamera();
	}

	void TrueDirectionalMovement::SetFreeCamera(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetFreeCamera(a_enable);
	}

	uint32_t TrueDirectionalMovement::GetDialogueMode(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetDialogueMode();
	}

	void TrueDirectionalMovement::SetDialogueMode(RE::StaticFunctionTag*, uint32_t a_mode)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetDialogueMode(static_cast<DirectionalMovementHandler::DialogueMode>(a_mode));
	}

	bool TrueDirectionalMovement::GetHeadtracking(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetHeadtracking();
	}

	void TrueDirectionalMovement::SetHeadtracking(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetHeadtracking(a_enable);
	}

	float TrueDirectionalMovement::GetDialogueHeadtrackingDuration(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetDialogueHeadtrackingDuration();
	}

	void TrueDirectionalMovement::SetDialogueHeadtrackingDuration(RE::StaticFunctionTag*, float a_duration)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetDialogueHeadtrackingDuration(a_duration);
	}

	bool TrueDirectionalMovement::GetCameraHeadtracking(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetCameraHeadtracking();
	}

	void TrueDirectionalMovement::SetCameraHeadtracking(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetCameraHeadtracking(a_enable);
	}

	float TrueDirectionalMovement::GetCameraHeadtrackingStrength(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetCameraHeadtrackingStrength();
	}

	void TrueDirectionalMovement::SetCameraHeadtrackingStrength(RE::StaticFunctionTag*, float a_strength)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetCameraHeadtrackingStrength(a_strength);
	}

	bool TrueDirectionalMovement::GetStopCameraHeadtrackingBehindPlayer(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetStopCameraHeadtrackingBehindPlayer();
	}

	void TrueDirectionalMovement::SetStopCameraHeadtrackingBehindPlayer(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetStopCameraHeadtrackingBehindPlayer(a_enable);
	}

	bool TrueDirectionalMovement::GetFaceCrosshairWhileBlocking(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetFaceCrosshairWhileBlocking();
	}

	void TrueDirectionalMovement::SetFaceCrosshairWhileBlocking(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetFaceCrosshairWhileBlocking(a_enable);
	}

	float TrueDirectionalMovement::GetRunningRotationSpeedMult(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetRunningRotationSpeedMult();
	}

	void TrueDirectionalMovement::SetRunningRotationSpeedMult(RE::StaticFunctionTag*, float a_mult)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetRunningRotationSpeedMult(a_mult);
	}

	float TrueDirectionalMovement::GetSprintingRotationSpeedMult(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetSprintingRotationSpeedMult();
	}

	void TrueDirectionalMovement::SetSprintingRotationSpeedMult(RE::StaticFunctionTag*, float a_mult)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetSprintingRotationSpeedMult(a_mult);
	}

	float TrueDirectionalMovement::GetAttackStartRotationSpeedMult(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetAttackStartRotationSpeedMult();
	}

	void TrueDirectionalMovement::SetAttackStartRotationSpeedMult(RE::StaticFunctionTag*, float a_mult)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetAttackStartRotationSpeedMult(a_mult);
	}

	float TrueDirectionalMovement::GetAttackMidRotationSpeedMult(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetAttackMidRotationSpeedMult();
	}

	void TrueDirectionalMovement::SetAttackMidRotationSpeedMult(RE::StaticFunctionTag*, float a_mult)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetAttackMidRotationSpeedMult(a_mult);
	}

	float TrueDirectionalMovement::GetAttackEndRotationSpeedMult(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetAttackEndRotationSpeedMult();
	}

	void TrueDirectionalMovement::SetAttackEndRotationSpeedMult(RE::StaticFunctionTag*, float a_mult)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetAttackEndRotationSpeedMult(a_mult);
	}

	float TrueDirectionalMovement::GetAirRotationSpeedMult(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetAirRotationSpeedMult();
	}

	void TrueDirectionalMovement::SetAirRotationSpeedMult(RE::StaticFunctionTag*, float a_mult)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetAirRotationSpeedMult(a_mult);
	}

	bool TrueDirectionalMovement::GetFaceCrosshairInstantly(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetFaceCrosshairInstantly();
	}

	void TrueDirectionalMovement::SetFaceCrosshairInstantly(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetFaceCrosshairInstantly(a_enable);
	}

	bool TrueDirectionalMovement::GetDisableAttackRotationMultipliersForTransformations(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetDisableAttackRotationMultipliersForTransformations();
	}

	void TrueDirectionalMovement::SetDisableAttackRotationMultipliersForTransformations(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetDisableAttackRotationMultipliersForTransformations(a_enable);
	}

	bool TrueDirectionalMovement::GetStopOnDirectionChange(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetStopOnDirectionChange();
	}

	void TrueDirectionalMovement::SetStopOnDirectionChange(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetStopOnDirectionChange(a_enable);
	}

	float TrueDirectionalMovement::GetTargetLockDistance(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockDistance();
	}

	void TrueDirectionalMovement::SetTargetLockDistance(RE::StaticFunctionTag*, float a_distance)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockDistance(a_distance);
	}

	float TrueDirectionalMovement::GetTargetLockPitchAdjustSpeed(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockPitchAdjustSpeed();
	}

	void TrueDirectionalMovement::SetTargetLockPitchAdjustSpeed(RE::StaticFunctionTag*, float a_speed)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockPitchAdjustSpeed(a_speed);
	}

	float TrueDirectionalMovement::GetTargetLockYawAdjustSpeed(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockYawAdjustSpeed();
	}

	void TrueDirectionalMovement::SetTargetLockYawAdjustSpeed(RE::StaticFunctionTag*, float a_speed)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockYawAdjustSpeed(a_speed);
	}

	float TrueDirectionalMovement::GetTargetLockPitchOffsetStrength(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockPitchOffsetStrength();
	}

	void TrueDirectionalMovement::SetTargetLockPitchOffsetStrength(RE::StaticFunctionTag*, float a_offsetStrength)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockPitchOffsetStrength(a_offsetStrength);
	}

	bool TrueDirectionalMovement::GetTargetLockUseMouse(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockUseMouse();
	}

	void TrueDirectionalMovement::SetTargetLockUseMouse(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockUseMouse(a_enable);
	}

	bool TrueDirectionalMovement::GetTargetLockUseScrollWheel(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockUseScrollWheel();
	}

	void TrueDirectionalMovement::SetTargetLockUseScrollWheel(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockUseScrollWheel(a_enable);
	}

	uint32_t TrueDirectionalMovement::GetTargetLockArrowAimType(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockArrowAimType();
	}

	void TrueDirectionalMovement::SetTargetLockArrowAimType(RE::StaticFunctionTag*, uint32_t a_type)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockArrowAimType(static_cast<DirectionalMovementHandler::TargetLockProjectileAimType>(a_type));
	}

	uint32_t TrueDirectionalMovement::GetTargetLockMissileAimType(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockMissileAimType();
	}

	void TrueDirectionalMovement::SetTargetLockMissileAimType(RE::StaticFunctionTag*, uint32_t a_type)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockMissileAimType(static_cast<DirectionalMovementHandler::TargetLockProjectileAimType>(a_type));
	}

	bool TrueDirectionalMovement::GetAutoTargetNextOnDeath(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetAutoTargetNextOnDeath();
	}

	void TrueDirectionalMovement::SetAutoTargetNextOnDeath(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetAutoTargetNextOnDeath(a_enable);
	}

	bool TrueDirectionalMovement::GetTargetLockTestLOS(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockTestLOS();
	}

	void TrueDirectionalMovement::SetTargetLockTestLOS(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockTestLOS(a_enable);
	}

	bool TrueDirectionalMovement::GetTargetLockHostileActorsOnly(RE::StaticFunctionTag*)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		return movementHandler->GetTargetLockHostileActorsOnly();
	}

	void TrueDirectionalMovement::SetTargetLockHostileActorsOnly(RE::StaticFunctionTag*, bool a_enable)
	{
		auto movementHandler = DirectionalMovementHandler::GetSingleton();
		movementHandler->SetTargetLockHostileActorsOnly(a_enable);
	}

	bool TrueDirectionalMovement::GetWidgetShowReticle(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowReticle();
	}

	void TrueDirectionalMovement::SetWidgetShowReticle(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowReticle(a_show);
	}

	uint32_t TrueDirectionalMovement::GetWidgetReticleStyle(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return static_cast<uint32_t>(widgetHandler->GetReticleStyle());
	}

	void TrueDirectionalMovement::SetWidgetReticleStyle(RE::StaticFunctionTag*, uint32_t a_style)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetReticleStyle(static_cast<WidgetHandler::ReticleStyle>(a_style));
	}

	bool TrueDirectionalMovement::GetWidgetShowTargetBar(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowTargetBar();
	}

	void TrueDirectionalMovement::SetWidgetShowTargetBar(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowTargetBar(a_show);
	}

	bool TrueDirectionalMovement::GetWidgetShowSoftTargetBar(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowSoftTargetBar();
	}

	void TrueDirectionalMovement::SetWidgetShowSoftTargetBar(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowSoftTargetBar(a_show);
	}

	bool TrueDirectionalMovement::GetWidgetShowTargetName(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowTargetName();
	}

	void TrueDirectionalMovement::SetWidgetShowTargetName(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowTargetName(a_show);
	}

	uint32_t TrueDirectionalMovement::GetWidgetTargetLevelMode(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return static_cast<uint32_t>(widgetHandler->GetTargetLevelMode());
	}

	void TrueDirectionalMovement::SetWidgetTargetLevelMode(RE::StaticFunctionTag*, uint32_t a_mode)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetTargetLevelMode(static_cast<WidgetHandler::TargetLockLevelMode>(a_mode));
	}

	uint32_t TrueDirectionalMovement::GetWidgetTargetLevelThreshold(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetTargetLevelThreshold();
	}

	void TrueDirectionalMovement::SetWidgetTargetLevelThreshold(RE::StaticFunctionTag*, uint32_t a_threshold)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetTargetLevelThreshold(static_cast<uint16_t>(a_threshold));
	}

	bool TrueDirectionalMovement::GetWidgetShowDamage(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowDamage();
	}

	void TrueDirectionalMovement::SetWidgetShowDamage(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowDamage(a_show);
	}

	bool TrueDirectionalMovement::GetWidgetShowHealthPhantom(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowHealthPhantom();
	}

	void TrueDirectionalMovement::SetWidgetShowHealthPhantom(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowHealthPhantom(a_show);
	}

	bool TrueDirectionalMovement::GetWidgetHideVanillaTargetBar(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetHideVanillaTargetBar();
	}

	void TrueDirectionalMovement::SetWidgetHideVanillaTargetBar(RE::StaticFunctionTag*, bool a_hide)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetHideVanillaTargetBar(a_hide);
	}

	float TrueDirectionalMovement::GetWidgetHealthPhantomDuration(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetHealthPhantomDuration();
	}

	void TrueDirectionalMovement::SetWidgetHealthPhantomDuration(RE::StaticFunctionTag*, float a_duration)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetHealthPhantomDuration(a_duration);
	}

	float TrueDirectionalMovement::GetWidgetDamageDuration(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetDamageDuration();
	}

	void TrueDirectionalMovement::SetWidgetDamageDuration(RE::StaticFunctionTag*, float a_duration)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetDamageDuration(a_duration);
	}

	float TrueDirectionalMovement::GetWidgetReticleScale(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetReticleScale();
	}

	void TrueDirectionalMovement::SetWidgetReticleScale(RE::StaticFunctionTag*, float a_scale)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetReticleScale(a_scale);
	}

	float TrueDirectionalMovement::GetWidgetTargetBarScale(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetTargetBarScale();
	}

	void TrueDirectionalMovement::SetWidgetTargetBarScale(RE::StaticFunctionTag*, float a_scale)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetTargetBarScale(a_scale);
	}

	bool TrueDirectionalMovement::GetWidgetUseHUDOpacityForReticle(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetUseHUDOpacityForReticle();
	}

	void TrueDirectionalMovement::SetWidgetUseHUDOpacityForReticle(RE::StaticFunctionTag*, bool a_enable)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetUseHUDOpacityForReticle(a_enable);
	}

	float TrueDirectionalMovement::GetWidgetReticleOpacity(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetReticleOpacity();
	}

	void TrueDirectionalMovement::SetWidgetReticleOpacity(RE::StaticFunctionTag*, float a_opacity)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetReticleOpacity(a_opacity);
	}

	bool TrueDirectionalMovement::GetWidgetUseHUDOpacityForTargetBar(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetUseHUDOpacityForTargetBar();
	}

	void TrueDirectionalMovement::SetWidgetUseHUDOpacityForTargetBar(RE::StaticFunctionTag*, bool a_enable)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetUseHUDOpacityForTargetBar(a_enable);
	}

	float TrueDirectionalMovement::GetWidgetTargetBarOpacity(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetTargetBarOpacity();
	}

	void TrueDirectionalMovement::SetWidgetTargetBarOpacity(RE::StaticFunctionTag*, float a_opacity)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetTargetBarOpacity(a_opacity);
	}

	bool TrueDirectionalMovement::GetShowBossBar(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowBossBar();
	}

	void TrueDirectionalMovement::SetShowBossBar(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowBossBar(a_show);
	}

	uint32_t TrueDirectionalMovement::GetBossNameAlignment(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return static_cast<uint32_t>(widgetHandler->GetBossNameAlignment());
	}

	void TrueDirectionalMovement::SetBossNameAlignment(RE::StaticFunctionTag*, uint32_t a_alignment)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossNameAlignment(static_cast<WidgetHandler::TextAlignment>(a_alignment));
	}

	uint32_t TrueDirectionalMovement::GetBossLevelMode(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return static_cast<uint32_t>(widgetHandler->GetBossLevelMode());
	}

	void TrueDirectionalMovement::SetBossLevelMode(RE::StaticFunctionTag*, uint32_t a_mode)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossLevelMode(static_cast<WidgetHandler::BossLevelMode>(a_mode));
	}

	uint32_t TrueDirectionalMovement::GetBossLevelThreshold(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossLevelThreshold();
	}

	void TrueDirectionalMovement::SetBossLevelThreshold(RE::StaticFunctionTag*, uint32_t a_threshold)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossLevelThreshold(static_cast<uint16_t>(a_threshold));
	}

	bool TrueDirectionalMovement::GetShowBossDamage(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowBossDamage();
	}

	void TrueDirectionalMovement::SetShowBossDamage(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowBossDamage(a_show);
	}

	bool TrueDirectionalMovement::GetShowBossHealthPhantom(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetShowBossHealthPhantom();
	}

	void TrueDirectionalMovement::SetShowBossHealthPhantom(RE::StaticFunctionTag*, bool a_show)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetShowBossHealthPhantom(a_show);
	}

	float TrueDirectionalMovement::GetBossHealthPhantomDuration(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossHealthPhantomDuration();
	}

	void TrueDirectionalMovement::SetBossHealthPhantomDuration(RE::StaticFunctionTag*, float a_duration)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossHealthPhantomDuration(a_duration);
	}

	float TrueDirectionalMovement::GetBossDamageDuration(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossDamageDuration();
	}

	void TrueDirectionalMovement::SetBossDamageDuration(RE::StaticFunctionTag*, float a_duration)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossDamageDuration(a_duration);
	}

	float TrueDirectionalMovement::GetBossBarScale(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossBarScale();
	}

	void TrueDirectionalMovement::SetBossBarScale(RE::StaticFunctionTag*, float a_scale)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossBarScale(a_scale);
	}

	bool TrueDirectionalMovement::GetUseHUDOpacityForBossBar(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetUseHUDOpacityForBossBar();
	}

	void TrueDirectionalMovement::SetUseHUDOpacityForBossBar(RE::StaticFunctionTag*, bool a_enable)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetUseHUDOpacityForBossBar(a_enable);
	}

	float TrueDirectionalMovement::GetBossBarOpacity(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossBarOpacity();
	}

	void TrueDirectionalMovement::SetBossBarOpacity(RE::StaticFunctionTag*, float a_opacity)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossBarOpacity(a_opacity);
	}

	float TrueDirectionalMovement::GetBossBarX(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossBarX();
	}

	void TrueDirectionalMovement::SetBossBarX(RE::StaticFunctionTag*, float a_position)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossBarX(a_position);
	}

	float TrueDirectionalMovement::GetBossBarY(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossBarY();
	}

	void TrueDirectionalMovement::SetBossBarY(RE::StaticFunctionTag*, float a_position)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossBarY(a_position);
	}

	bool TrueDirectionalMovement::GetBossBarOffsetSubtitles(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetBossBarOffsetSubtitles();
	}

	void TrueDirectionalMovement::SetBossBarOffsetSubtitles(RE::StaticFunctionTag*, bool a_enable)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetBossBarOffsetSubtitles(a_enable);
	}

	float TrueDirectionalMovement::GetMultipleBossBarsOffset(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetMultipleBossBarsOffset();
	}

	void TrueDirectionalMovement::SetMultipleBossBarsOffset(RE::StaticFunctionTag*, float a_offset)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetMultipleBossBarsOffset(a_offset);
	}

	bool TrueDirectionalMovement::GetMultipleBossBarsStackUpwards(RE::StaticFunctionTag*)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		return widgetHandler->GetMultipleBossBarsStackUpwards();
	}

	void TrueDirectionalMovement::SetMultipleBossBarsStackUpwards(RE::StaticFunctionTag*, bool a_enable)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		widgetHandler->SetMultipleBossBarsStackUpwards(a_enable);
	}

	uint32_t TrueDirectionalMovement::GetTargetLockKey(RE::StaticFunctionTag*)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		return inputHandler->GetTargetLockKey();
	}

	void TrueDirectionalMovement::SetTargetLockKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		inputHandler->SetTargetLockKey(a_key);
	}

	uint32_t TrueDirectionalMovement::GetSwitchTargetLeftKey(RE::StaticFunctionTag*)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		return inputHandler->GetSwitchTargetLeftKey();
	}

	void TrueDirectionalMovement::SetSwitchTargetLeftKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		inputHandler->SetSwitchTargetLeftKey(a_key);
	}

	uint32_t TrueDirectionalMovement::GetSwitchTargetRightKey(RE::StaticFunctionTag*)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		return inputHandler->GetSwitchTargetRightKey();
	}

	void TrueDirectionalMovement::SetSwitchTargetRightKey(RE::StaticFunctionTag*, uint32_t a_key)
	{
		auto inputHandler = Events::InputEventHandler::GetSingleton();
		inputHandler->SetSwitchTargetRightKey(a_key);
	}

	bool TrueDirectionalMovement::Register(RE::BSScript::IVirtualMachine* a_vm)
	{
		a_vm->RegisterFunction("GetFreeCamera", "TrueDirectionalMovement", GetFreeCamera);
		a_vm->RegisterFunction("SetFreeCamera", "TrueDirectionalMovement", SetFreeCamera);
		a_vm->RegisterFunction("GetDialogueMode", "TrueDirectionalMovement", GetDialogueMode);
		a_vm->RegisterFunction("SetDialogueMode", "TrueDirectionalMovement", SetDialogueMode);
		a_vm->RegisterFunction("GetHeadtracking", "TrueDirectionalMovement", GetHeadtracking);
		a_vm->RegisterFunction("SetHeadtracking", "TrueDirectionalMovement", SetHeadtracking);
		a_vm->RegisterFunction("GetDialogueHeadtrackingDuration", "TrueDirectionalMovement", GetDialogueHeadtrackingDuration);
		a_vm->RegisterFunction("SetDialogueHeadtrackingDuration", "TrueDirectionalMovement", SetDialogueHeadtrackingDuration);
		a_vm->RegisterFunction("GetCameraHeadtracking", "TrueDirectionalMovement", GetCameraHeadtracking);
		a_vm->RegisterFunction("SetCameraHeadtracking", "TrueDirectionalMovement", SetCameraHeadtracking);
		a_vm->RegisterFunction("GetCameraHeadtrackingStrength", "TrueDirectionalMovement", GetCameraHeadtrackingStrength);
		a_vm->RegisterFunction("SetCameraHeadtrackingStrength", "TrueDirectionalMovement", SetCameraHeadtrackingStrength);
		a_vm->RegisterFunction("GetStopCameraHeadtrackingBehindPlayer", "TrueDirectionalMovement", GetStopCameraHeadtrackingBehindPlayer);
		a_vm->RegisterFunction("SetStopCameraHeadtrackingBehindPlayer", "TrueDirectionalMovement", SetStopCameraHeadtrackingBehindPlayer);
		a_vm->RegisterFunction("GetFaceCrosshairWhileBlocking", "TrueDirectionalMovement", GetFaceCrosshairWhileBlocking);
		a_vm->RegisterFunction("SetFaceCrosshairWhileBlocking", "TrueDirectionalMovement", SetFaceCrosshairWhileBlocking);
		a_vm->RegisterFunction("GetRunningRotationSpeedMult", "TrueDirectionalMovement", GetRunningRotationSpeedMult);
		a_vm->RegisterFunction("SetRunningRotationSpeedMult", "TrueDirectionalMovement", SetRunningRotationSpeedMult);
		a_vm->RegisterFunction("GetSprintingRotationSpeedMult", "TrueDirectionalMovement", GetSprintingRotationSpeedMult);
		a_vm->RegisterFunction("SetSprintingRotationSpeedMult", "TrueDirectionalMovement", SetSprintingRotationSpeedMult);
		a_vm->RegisterFunction("GetAttackStartRotationSpeedMult", "TrueDirectionalMovement", GetAttackStartRotationSpeedMult);
		a_vm->RegisterFunction("SetAttackStartRotationSpeedMult", "TrueDirectionalMovement", SetAttackStartRotationSpeedMult);
		a_vm->RegisterFunction("GetAttackMidRotationSpeedMult", "TrueDirectionalMovement", GetAttackMidRotationSpeedMult);
		a_vm->RegisterFunction("SetAttackMidRotationSpeedMult", "TrueDirectionalMovement", SetAttackMidRotationSpeedMult);
		a_vm->RegisterFunction("GetAttackEndRotationSpeedMult", "TrueDirectionalMovement", GetAttackEndRotationSpeedMult);
		a_vm->RegisterFunction("SetAttackEndRotationSpeedMult", "TrueDirectionalMovement", SetAttackEndRotationSpeedMult);
		a_vm->RegisterFunction("GetAirRotationSpeedMult", "TrueDirectionalMovement", GetAirRotationSpeedMult);
		a_vm->RegisterFunction("SetAirRotationSpeedMult", "TrueDirectionalMovement", SetAirRotationSpeedMult);
		a_vm->RegisterFunction("GetFaceCrosshairInstantly", "TrueDirectionalMovement", GetFaceCrosshairInstantly);
		a_vm->RegisterFunction("SetFaceCrosshairInstantly", "TrueDirectionalMovement", SetFaceCrosshairInstantly);
		a_vm->RegisterFunction("GetDisableAttackRotationMultipliersForTransformations", "TrueDirectionalMovement", GetDisableAttackRotationMultipliersForTransformations);
		a_vm->RegisterFunction("SetDisableAttackRotationMultipliersForTransformations", "TrueDirectionalMovement", SetDisableAttackRotationMultipliersForTransformations);
		a_vm->RegisterFunction("GetStopOnDirectionChange", "TrueDirectionalMovement", GetStopOnDirectionChange);
		a_vm->RegisterFunction("SetStopOnDirectionChange", "TrueDirectionalMovement", SetStopOnDirectionChange);
		a_vm->RegisterFunction("GetTargetLockDistance", "TrueDirectionalMovement", GetTargetLockDistance);
		a_vm->RegisterFunction("SetTargetLockDistance", "TrueDirectionalMovement", SetTargetLockDistance);
		a_vm->RegisterFunction("GetTargetLockPitchAdjustSpeed", "TrueDirectionalMovement", GetTargetLockPitchAdjustSpeed);
		a_vm->RegisterFunction("SetTargetLockPitchAdjustSpeed", "TrueDirectionalMovement", SetTargetLockPitchAdjustSpeed);
		a_vm->RegisterFunction("GetTargetLockYawAdjustSpeed", "TrueDirectionalMovement", GetTargetLockYawAdjustSpeed);
		a_vm->RegisterFunction("SetTargetLockYawAdjustSpeed", "TrueDirectionalMovement", SetTargetLockYawAdjustSpeed);
		a_vm->RegisterFunction("GetTargetLockPitchOffsetStrength", "TrueDirectionalMovement", GetTargetLockPitchOffsetStrength);
		a_vm->RegisterFunction("SetTargetLockPitchOffsetStrength", "TrueDirectionalMovement", SetTargetLockPitchOffsetStrength);
		a_vm->RegisterFunction("GetTargetLockUseMouse", "TrueDirectionalMovement", GetTargetLockUseMouse);
		a_vm->RegisterFunction("SetTargetLockUseMouse", "TrueDirectionalMovement", SetTargetLockUseMouse);
		a_vm->RegisterFunction("GetTargetLockUseScrollWheel", "TrueDirectionalMovement", GetTargetLockUseScrollWheel);
		a_vm->RegisterFunction("SetTargetLockUseScrollWheel", "TrueDirectionalMovement", SetTargetLockUseScrollWheel);
		a_vm->RegisterFunction("GetTargetLockArrowAimType", "TrueDirectionalMovement", GetTargetLockArrowAimType);
		a_vm->RegisterFunction("SetTargetLockArrowAimType", "TrueDirectionalMovement", SetTargetLockArrowAimType);
		a_vm->RegisterFunction("GetTargetLockMissileAimType", "TrueDirectionalMovement", GetTargetLockMissileAimType);
		a_vm->RegisterFunction("SetTargetLockMissileAimType", "TrueDirectionalMovement", SetTargetLockMissileAimType);
		a_vm->RegisterFunction("GetAutoTargetNextOnDeath", "TrueDirectionalMovement", GetAutoTargetNextOnDeath);
		a_vm->RegisterFunction("SetAutoTargetNextOnDeath", "TrueDirectionalMovement", SetAutoTargetNextOnDeath);
		a_vm->RegisterFunction("GetTargetLockTestLOS", "TrueDirectionalMovement", GetTargetLockTestLOS);
		a_vm->RegisterFunction("SetTargetLockTestLOS", "TrueDirectionalMovement", SetTargetLockTestLOS);
		a_vm->RegisterFunction("GetTargetLockHostileActorsOnly", "TrueDirectionalMovement", GetTargetLockHostileActorsOnly);
		a_vm->RegisterFunction("SetTargetLockHostileActorsOnly", "TrueDirectionalMovement", SetTargetLockHostileActorsOnly);

		a_vm->RegisterFunction("GetWidgetShowReticle", "TrueDirectionalMovement", GetWidgetShowReticle);
		a_vm->RegisterFunction("SetWidgetShowReticle", "TrueDirectionalMovement", SetWidgetShowReticle);
		a_vm->RegisterFunction("GetWidgetReticleStyle", "TrueDirectionalMovement", GetWidgetReticleStyle);
		a_vm->RegisterFunction("SetWidgetReticleStyle", "TrueDirectionalMovement", SetWidgetReticleStyle);
		a_vm->RegisterFunction("GetWidgetShowTargetBar", "TrueDirectionalMovement", GetWidgetShowTargetBar);
		a_vm->RegisterFunction("SetWidgetShowTargetBar", "TrueDirectionalMovement", SetWidgetShowTargetBar);
		a_vm->RegisterFunction("GetWidgetShowSoftTargetBar", "TrueDirectionalMovement", GetWidgetShowSoftTargetBar);
		a_vm->RegisterFunction("SetWidgetShowSoftTargetBar", "TrueDirectionalMovement", SetWidgetShowSoftTargetBar);
		a_vm->RegisterFunction("GetWidgetShowTargetName", "TrueDirectionalMovement", GetWidgetShowTargetName);
		a_vm->RegisterFunction("SetWidgetShowTargetName", "TrueDirectionalMovement", SetWidgetShowTargetName);
		a_vm->RegisterFunction("GetWidgetTargetLevelMode", "TrueDirectionalMovement", GetWidgetTargetLevelMode);
		a_vm->RegisterFunction("SetWidgetTargetLevelMode", "TrueDirectionalMovement", SetWidgetTargetLevelMode);
		a_vm->RegisterFunction("GetWidgetTargetLevelThreshold", "TrueDirectionalMovement", GetWidgetTargetLevelThreshold);
		a_vm->RegisterFunction("SetWidgetTargetLevelThreshold", "TrueDirectionalMovement", SetWidgetTargetLevelThreshold);
		a_vm->RegisterFunction("GetWidgetShowDamage", "TrueDirectionalMovement", GetWidgetShowDamage);
		a_vm->RegisterFunction("SetWidgetShowDamage", "TrueDirectionalMovement", SetWidgetShowDamage);
		a_vm->RegisterFunction("GetWidgetShowHealthPhantom", "TrueDirectionalMovement", GetWidgetShowHealthPhantom);
		a_vm->RegisterFunction("SetWidgetShowHealthPhantom", "TrueDirectionalMovement", SetWidgetShowHealthPhantom);
		a_vm->RegisterFunction("GetWidgetHideVanillaTargetBar", "TrueDirectionalMovement", GetWidgetHideVanillaTargetBar);
		a_vm->RegisterFunction("SetWidgetHideVanillaTargetBar", "TrueDirectionalMovement", SetWidgetHideVanillaTargetBar);
		a_vm->RegisterFunction("GetWidgetHealthPhantomDuration", "TrueDirectionalMovement", GetWidgetHealthPhantomDuration);
		a_vm->RegisterFunction("SetWidgetHealthPhantomDuration", "TrueDirectionalMovement", SetWidgetHealthPhantomDuration);
		a_vm->RegisterFunction("GetWidgetDamageDuration", "TrueDirectionalMovement", GetWidgetDamageDuration);
		a_vm->RegisterFunction("SetWidgetDamageDuration", "TrueDirectionalMovement", SetWidgetDamageDuration);
		a_vm->RegisterFunction("GetWidgetReticleScale", "TrueDirectionalMovement", GetWidgetReticleScale);
		a_vm->RegisterFunction("SetWidgetReticleScale", "TrueDirectionalMovement", SetWidgetReticleScale);
		a_vm->RegisterFunction("GetWidgetTargetBarScale", "TrueDirectionalMovement", GetWidgetTargetBarScale);
		a_vm->RegisterFunction("SetWidgetTargetBarScale", "TrueDirectionalMovement", SetWidgetTargetBarScale);
		a_vm->RegisterFunction("GetWidgetUseHUDOpacityForReticle", "TrueDirectionalMovement", GetWidgetUseHUDOpacityForReticle);
		a_vm->RegisterFunction("SetWidgetUseHUDOpacityForReticle", "TrueDirectionalMovement", SetWidgetUseHUDOpacityForReticle);
		a_vm->RegisterFunction("GetWidgetReticleOpacity", "TrueDirectionalMovement", GetWidgetReticleOpacity);
		a_vm->RegisterFunction("SetWidgetReticleOpacity", "TrueDirectionalMovement", SetWidgetReticleOpacity);
		a_vm->RegisterFunction("GetWidgetUseHUDOpacityForTargetBar", "TrueDirectionalMovement", GetWidgetUseHUDOpacityForTargetBar);
		a_vm->RegisterFunction("SetWidgetUseHUDOpacityForTargetBar", "TrueDirectionalMovement", SetWidgetUseHUDOpacityForTargetBar);
		a_vm->RegisterFunction("GetWidgetTargetBarOpacity", "TrueDirectionalMovement", GetWidgetTargetBarOpacity);
		a_vm->RegisterFunction("SetWidgetTargetBarOpacity", "TrueDirectionalMovement", SetWidgetTargetBarOpacity);

		a_vm->RegisterFunction("GetShowBossBar", "TrueDirectionalMovement", GetShowBossBar);
		a_vm->RegisterFunction("SetShowBossBar", "TrueDirectionalMovement", SetShowBossBar);
		a_vm->RegisterFunction("GetBossNameAlignment", "TrueDirectionalMovement", GetBossNameAlignment);
		a_vm->RegisterFunction("SetBossNameAlignment", "TrueDirectionalMovement", SetBossNameAlignment);
		a_vm->RegisterFunction("GetBossLevelMode", "TrueDirectionalMovement", GetBossLevelMode);
		a_vm->RegisterFunction("SetBossLevelMode", "TrueDirectionalMovement", SetBossLevelMode);
		a_vm->RegisterFunction("GetBossLevelThreshold", "TrueDirectionalMovement", GetBossLevelThreshold);
		a_vm->RegisterFunction("SetBossLevelThreshold", "TrueDirectionalMovement", SetBossLevelThreshold);
		a_vm->RegisterFunction("GetShowBossDamage", "TrueDirectionalMovement", GetShowBossDamage);
		a_vm->RegisterFunction("SetShowBossDamage", "TrueDirectionalMovement", SetShowBossDamage);
		a_vm->RegisterFunction("GetShowBossHealthPhantom", "TrueDirectionalMovement", GetShowBossHealthPhantom);
		a_vm->RegisterFunction("SetShowBossHealthPhantom", "TrueDirectionalMovement", SetShowBossHealthPhantom);
		a_vm->RegisterFunction("GetBossHealthPhantomDuration", "TrueDirectionalMovement", GetBossHealthPhantomDuration);
		a_vm->RegisterFunction("SetBossHealthPhantomDuration", "TrueDirectionalMovement", SetBossHealthPhantomDuration);
		a_vm->RegisterFunction("GetBossDamageDuration", "TrueDirectionalMovement", GetBossDamageDuration);
		a_vm->RegisterFunction("SetBossDamageDuration", "TrueDirectionalMovement", SetBossDamageDuration);
		a_vm->RegisterFunction("GetBossBarScale", "TrueDirectionalMovement", GetBossBarScale);
		a_vm->RegisterFunction("SetBossBarScale", "TrueDirectionalMovement", SetBossBarScale);
		a_vm->RegisterFunction("GetUseHUDOpacityForBossBar", "TrueDirectionalMovement", GetUseHUDOpacityForBossBar);
		a_vm->RegisterFunction("SetUseHUDOpacityForBossBar", "TrueDirectionalMovement", SetUseHUDOpacityForBossBar);
		a_vm->RegisterFunction("GetBossBarOpacity", "TrueDirectionalMovement", GetBossBarOpacity);
		a_vm->RegisterFunction("SetBossBarOpacity", "TrueDirectionalMovement", SetBossBarOpacity);
		a_vm->RegisterFunction("GetBossBarX", "TrueDirectionalMovement", GetBossBarX);
		a_vm->RegisterFunction("SetBossBarX", "TrueDirectionalMovement", SetBossBarX);
		a_vm->RegisterFunction("GetBossBarY", "TrueDirectionalMovement", GetBossBarY);
		a_vm->RegisterFunction("SetBossBarY", "TrueDirectionalMovement", SetBossBarY);
		a_vm->RegisterFunction("GetBossBarOffsetSubtitles", "TrueDirectionalMovement", GetBossBarOffsetSubtitles);
		a_vm->RegisterFunction("SetBossBarOffsetSubtitles", "TrueDirectionalMovement", SetBossBarOffsetSubtitles);
		a_vm->RegisterFunction("GetMultipleBossBarsOffset", "TrueDirectionalMovement", GetMultipleBossBarsOffset);
		a_vm->RegisterFunction("SetMultipleBossBarsOffset", "TrueDirectionalMovement", SetMultipleBossBarsOffset);
		a_vm->RegisterFunction("GetMultipleBossBarsStackUpwards", "TrueDirectionalMovement", GetMultipleBossBarsStackUpwards);
		a_vm->RegisterFunction("SetMultipleBossBarsStackUpwards", "TrueDirectionalMovement", SetMultipleBossBarsStackUpwards);

		a_vm->RegisterFunction("GetTargetLockKey", "TrueDirectionalMovement", GetTargetLockKey);
		a_vm->RegisterFunction("SetTargetLockKey", "TrueDirectionalMovement", SetTargetLockKey);
		a_vm->RegisterFunction("GetSwitchTargetLeftKey", "TrueDirectionalMovement", GetSwitchTargetLeftKey);
		a_vm->RegisterFunction("SetSwitchTargetLeftKey", "TrueDirectionalMovement", SetSwitchTargetLeftKey);
		a_vm->RegisterFunction("GetSwitchTargetRightKey", "TrueDirectionalMovement", GetSwitchTargetRightKey);
		a_vm->RegisterFunction("SetSwitchTargetRightKey", "TrueDirectionalMovement", SetSwitchTargetRightKey);
		logger::info("Registered TrueDirectionalMovement class");
		return true;
	}

	void Register()
	{
		auto papyrus = SKSE::GetPapyrusInterface();
		papyrus->Register(TrueDirectionalMovement::Register);
		logger::info("Registered papyrus functions");
	}
}
