#include "DirectionalMovementHandler.h"
#include "Settings.h"
#include "Events.h"
#include "Offsets.h"

#include <Psapi.h>
#include <DbgHelp.h>
#pragma comment(lib, "dbghelp.lib")

constexpr auto werewolfFormID = 0xCDD84;
constexpr auto vampireLordFormID = 0x200283A;

DirectionalMovementHandler* DirectionalMovementHandler::GetSingleton()
{
	static DirectionalMovementHandler singleton;
	return std::addressof(singleton);
}

void DirectionalMovementHandler::Register()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	bool bSuccess = playerCharacter->AddAnimationGraphEventSink(DirectionalMovementHandler::GetSingleton());
	if (bSuccess) {
		logger::info("Registered {}"sv, typeid(RE::BSAnimationGraphEvent).name());
	} else {
		RE::BSAnimationGraphManagerPtr graphManager;
		playerCharacter->GetAnimationGraphManager(graphManager);
		bool bSinked = false;
		if (graphManager) {			
			for (auto& animationGraph : graphManager->graphs) {
				if (bSinked) {
					break;
				}
				auto eventSource = animationGraph->GetEventSource<RE::BSAnimationGraphEvent>();
				for (auto& sink : eventSource->sinks) {
					if (sink == DirectionalMovementHandler::GetSingleton()) {
						bSinked = true;
						break;
					}
				}
			}
		}
		
		if (!bSinked) {
			logger::info("Failed to register {}"sv, typeid(RE::BSAnimationGraphEvent).name());
		}		
	}
}

constexpr uint32_t hash(const char* data, size_t const size) noexcept
{
	uint32_t hash = 5381;

	for (const char* c = data; c < data + size; ++c) {
		hash = ((hash << 5) + hash) + (unsigned char)*c;
	}

	return hash;
}

constexpr uint32_t operator"" _h(const char* str, size_t size) noexcept
{
	return hash(str, size);
}

DirectionalMovementHandler::EventResult DirectionalMovementHandler::ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>*)
{
	if (a_event) {
		std::string_view eventTag = a_event->tag.data();

		switch (hash(eventTag.data(), eventTag.size())) {
		// Start phase
		case "CastOKStart"_h:
		case "preHitFrame"_h:
		case "MCO_AttackInitiate"_h:
		case "MCO_PowerAttackInitiate"_h:
		case "MCO_InputBuffer"_h:
		case "TDM_AttackStart"_h:
		case "Collision_AttackStart"_h:
		case "Collision_Start"_h:
			if (_attackState != AttackState::kMid) {
				SetAttackState(AttackState::kStart);
			}
			break;

		case "weaponSwing"_h:
		case "weaponLeftSwing"_h:
		case "SoundPlay.WPNSwingUnarmed"_h:
		case "TDM_AttackMid"_h:
		case "Collision_Add"_h:
			if (_attackState != AttackState::kEnd) {
				SetAttackState(AttackState::kMid);
			}
			break;

		case "HitFrame"_h:
		case "attackWinStart"_h:
		case "SkySA_AttackWinStart"_h:
		case "MCO_WinOpen"_h:
		case "MCO_PowerWinOpen"_h:
		case "MCO_TransitionOpen"_h:
		case "MCO_Recovery"_h:
		case "BFCO_NextWinStart"_h:
		case "BFCO_NextPowerWinStart"_h:
		case "BFCO_DIY_recovery"_h:
		case "TDM_AttackEnd"_h:
		case "Collision_AttackEnd"_h:
			SetAttackState(AttackState::kEnd);
			break;

		// Back to none
		case "attackStop"_h:
		case "TDM_AttackStop"_h:
		case "SkySA_AttackWinEnd"_h:
		case "MCO_WinClose"_h:
		case "MCO_PowerWinClose"_h:
		case "BFCO_DIY_EndLoop"_h:
		case "MCO_TransitionClose"_h:
			SetAttackState(AttackState::kNone);
			break;

		case "DF_DodgeStart"_h:
			OnDodge();
			break;
		}
	}

	return EventResult::kContinue;
}

void DirectionalMovementHandler::ResetControls()
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	directionalMovementHandler->_bHasMovementInput = false;
	directionalMovementHandler->_pressedDirections = Direction::kInvalid;
}

void DirectionalMovementHandler::Update()
{
	if (RE::UI::GetSingleton()->GameIsPaused()) {
		return;
	}

	Settings::UpdateGlobals();

	ProgressTimers();

	UpdateTargetLock();

	UpdateTweeningState();

	UpdateFacingState();

	UpdateFacingCrosshair();

	UpdateDirectionalMovement();

	UpdateDodgingState();

	UpdateJumpingState();

	UpdateMountedArchery();

	if (_bReticleRemoved || IsAiming()) {
		_bReticleRemoved = false;
		ShowCrosshair();
	} else if (HasTargetLocked()) {
		HideCrosshair();
	}

	if (IsFreeCamera()) {
		if (_target) {
			SetDesiredAngleToTarget(RE::PlayerCharacter::GetSingleton(), _target);
			LookAtTarget(_target);
			if (Settings::glob_directionalMovement) {
				Settings::glob_directionalMovement->value = 0;
			}
		} else if (Settings::fMeleeMagnetismAngle > 0.f) {
			SetDesiredAngleToMagnetismTarget();
		}		

		if (Settings::uDialogueMode == DialogueMode::kFaceSpeaker) {
			auto newDialogueSpeaker = RE::MenuTopicManager::GetSingleton()->speaker;

			if (newDialogueSpeaker != _dialogueSpeaker) {
				_dialogueSpeaker = newDialogueSpeaker;
			}
			
			if (_dialogueSpeaker) {
				auto dialogueSpeaker = _dialogueSpeaker.get();
				if (dialogueSpeaker) {
					auto actorSpeaker = dialogueSpeaker->As<RE::Actor>();
					if (actorSpeaker) {
						RE::ActorHandle actorHandle = actorSpeaker->GetHandle();
						SetDesiredAngleToTarget(RE::PlayerCharacter::GetSingleton(), actorHandle);
						if (Settings::bHeadtracking && !GetForceDisableHeadtracking()) {
							auto playerCharacter = RE::PlayerCharacter::GetSingleton();
							auto currentProcess = playerCharacter->GetActorRuntimeData().currentProcess;
							if (currentProcess && currentProcess->high) {
								currentProcess->high->SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE::kCombat, actorSpeaker);
								RefreshDialogueHeadtrackTimer();
							}
						}
					}
				}
			}
		}

		UpdateSwimmingPitchOffset();

		// last safety check before utilizing desired angle
		if (IsPlayerAIDriven() || IsPlayerAnimationDriven()) {
			ResetDesiredAngle();
		}

		UpdateRotation();
	} else {
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (playerCharacter && playerCharacter->IsOnMount())
		{
			LookAtTarget(_target);
		}
		else
		{
			UpdateRotationLockedCam();
		}

		if (Settings::bHeadtracking && !GetForceDisableHeadtracking()) {
			auto playerCamera = RE::PlayerCamera::GetSingleton();
			if (playerCharacter && playerCamera && playerCamera->currentState && (playerCamera->currentState->id != RE::CameraState::kThirdPerson || IFPV_IsFirstPerson() || ImprovedCamera_IsFirstPerson())){
				// disable headtracking while not in third person
				playerCharacter->AsActorState()->actorState2.headTracking = false;
				if (!IsBehaviorPatchInstalled(playerCharacter) && !RE::UI::GetSingleton()->IsMenuOpen(RE::RaceSexMenu::MENU_NAME)) {
					playerCharacter->SetGraphVariableBool("IsNPC", false);
				}
			}
		}
	}

	if (Settings::uAdjustCameraYawDuringMovement > CameraAdjustMode::kDisable) {
		UpdateCameraAutoRotation();
	}

	if (_bResetCamera) {
		auto playerCamera = RE::PlayerCamera::GetSingleton();
		if (playerCamera->currentState && playerCamera->currentState->id == RE::CameraState::kThirdPerson || playerCamera->currentState->id == RE::CameraState::kMount) {
			RE::TESObjectREFR* cameraTarget = nullptr;
			auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
			bool bIsMounted = thirdPersonState->id == RE::CameraState::kMount;

			if (bIsMounted) {
				auto horseCameraState = static_cast<RE::HorseCameraState*>(thirdPersonState);
				cameraTarget = horseCameraState->horseRefHandle.get().get();
			} else {
				cameraTarget = RE::PlayerCharacter::GetSingleton();
			}

			float desiredRotationX = NormalRelativeAngle(_desiredCameraAngleX - cameraTarget->data.angle.z);
			float desiredRotationY = Settings::bResetCameraPitch ? 0.f : thirdPersonState->freeRotation.y;
			float desiredTargetPitch = Settings::bResetCameraPitch ? 0.f : cameraTarget->data.angle.x;
			const float realTimeDeltaTime = GetRealTimeDeltaTime();
			thirdPersonState->freeRotation.x = InterpAngleTo(thirdPersonState->freeRotation.x, desiredRotationX, realTimeDeltaTime, 10.f);
			thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, desiredRotationY, realTimeDeltaTime, 10.f);
			cameraTarget->data.angle.x = InterpAngleTo(cameraTarget->data.angle.x, desiredTargetPitch, realTimeDeltaTime, 10.f);
			if (GetAngleDiff(thirdPersonState->freeRotation.x, desiredRotationX) < 0.05f &&
				GetAngleDiff(thirdPersonState->freeRotation.y, desiredRotationY) < 0.05f &&
				GetAngleDiff(cameraTarget->data.angle.x, desiredTargetPitch) < 0.05f) {
				_bResetCamera = false;
			} else {
				return;
			}
		} else {
			_bResetCamera = false;
		}
	}

	UpdateProjectileTargetMap();

	if (Settings::bOverrideAcrobatics) {
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerController = playerCharacter->GetCharController();
		if (playerController) {
			if (_defaultAcrobatics == -1.f) {
				_defaultAcrobatics = playerController->acrobatics;
			}
			bool bGliding = false;
			playerCharacter->GetGraphVariableBool("bParaGliding", bGliding);
			playerController->acrobatics = bGliding ? Settings::fAcrobaticsGliding : Settings::fAcrobatics;
		}
	}

#ifndef NDEBUG
	if (APIs::TrueHUD) {
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();

		auto playerAngle = playerCharacter->data.angle.z;
		auto playerPos = playerCharacter->GetPosition() + RE::NiPoint3{ 0.f, 0.f, 50.f };

		RE::NiPoint3 forwardVector{ 0.f, 1.f, 0.f };

		RE::NiPoint3 dir = RotateAngleAxis(forwardVector, -playerAngle, { 0.f, 0.f, 1.f });
		APIs::TrueHUD->DrawArrow(playerPos, playerPos + dir * 20.f);

		if (_desiredAngle != -1.f) {
			playerPos += RE::NiPoint3{ 0.f, 0.f, 10.f };
			dir = RotateAngleAxis(forwardVector, -_desiredAngle, { 0.f, 0.f, 1.f });
			APIs::TrueHUD->DrawArrow(playerPos, playerPos + dir * 25.f, 10.f, 0.f, 0xFFFF00FF, 1.5f);
		}
	}
#endif
}

void DirectionalMovementHandler::UpdateDirectionalMovement()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	bool bIsAIDriven = IsPlayerAIDriven();
	bool bFreeCamera = GetFreeCameraEnabled();

	RE::TESCameraState* currentCameraState = RE::PlayerCamera::GetSingleton()->currentState.get();
	if (bFreeCamera && !GetForceDisableDirectionalMovement() && currentCameraState && !bIsAIDriven &&
		(!_bShouldFaceCrosshair || _bCurrentlyTurningToCrosshair)  &&
		((currentCameraState->id == RE::CameraStates::kThirdPerson && !IFPV_IsFirstPerson() && !ImprovedCamera_IsFirstPerson()) ||
			(currentCameraState->id == RE::CameraStates::kTween && _cameraStateBeforeTween != RE::CameraStates::kFirstPerson) ||
			currentCameraState->id == RE::CameraState::kBleedout) &&
		(Settings::uDialogueMode != DialogueMode::kDisable || !RE::MenuTopicManager::GetSingleton()->speaker)) {
		_bDirectionalMovement = true;
		if (Settings::glob_directionalMovement) {
			Settings::glob_directionalMovement->value = Is360Movement();
		}
	} else {
		_bDirectionalMovement = false;
		if (Settings::glob_directionalMovement) {
			Settings::glob_directionalMovement->value = 0;
		}
		//if (bIsAIDriven && currentCameraState->id == RE::CameraStates::kThirdPerson) {
		//	// reset the free rotation while ai driven to avoid issues
		//	auto thirdPersonState = static_cast<RE::ThirdPersonState*>(currentCameraState);
		//	thirdPersonState->freeRotation.x = 0;
		//}
		ResetDesiredAngle();
	}

	OverrideControllerBufferDepth(_bDirectionalMovement && !playerCharacter->AsActorState()->IsSprinting());
}

void DirectionalMovementHandler::UpdateFacingState()
{
	using Delivery = RE::MagicSystem::Delivery;

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (_faceCrosshairTimer > 0.f) {
		_bShouldFaceCrosshair = true;
		_bShouldFaceTarget = true;
	}

	if (_aimingTimer <= 0.f) {
		_bIsAiming = false;
	}

	if (_faceCrosshairTimer <= 0.f) {
		_bShouldFaceCrosshair = false;
		_bShouldFaceTarget = false;
	}

	if (!playerCharacter) {
		return;
	}

	if (Settings::bFaceCrosshairDuringAutoMove && RE::PlayerControls::GetSingleton()->data.autoMove) {
		_bShouldFaceCrosshair = true;
		_bShouldFaceTarget = true;
		return;
	}

	auto playerActorState = playerCharacter->AsActorState();

	// workaround for an issue with AGO, I don't like this but it works
	static RE::ATTACK_STATE_ENUM currentAttackState;
	auto previousState = currentAttackState;
	RE::ATTACK_STATE_ENUM playerAttackState = playerActorState->GetAttackState();
	if (playerAttackState != RE::ATTACK_STATE_ENUM::kBowAttached) {
		currentAttackState = playerAttackState;
	}

	bool bShouldFaceCrosshairWhileMoving = (playerActorState->GetWeaponState() == RE::WEAPON_STATE::kSheathed ? Settings::uDirectionalMovementSheathed : Settings::uDirectionalMovementDrawn) == DirectionalMovementMode::kVanilla;

	if (bShouldFaceCrosshairWhileMoving && HasMovementInput() && !HasTargetLocked()) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = 0.f;
		_bShouldFaceTarget = true;
		return;
	}

	bool bIsAttacking = playerAttackState > RE::ATTACK_STATE_ENUM::kNone && playerAttackState < RE::ATTACK_STATE_ENUM::kBowDraw;

	if (Settings::bFaceCrosshairWhileAttacking && bIsAttacking && !HasTargetLocked() && !IsMagnetismActive()) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = 0.1f;
		_bShouldFaceTarget = true;
		return;
	}

	if (Settings::bFaceCrosshairWhileShouting)
	{
		if (auto currentProcess = playerCharacter->GetActorRuntimeData().currentProcess) {
			if (currentProcess && currentProcess->high && currentProcess->high->currentShout) {
				_bShouldFaceCrosshair = true;
				_faceCrosshairTimer = _faceCrosshairDuration;
				_bShouldFaceTarget = true;
				return;
			}
		}
	}

	if (Settings::bFaceCrosshairWhileBlocking && !HasTargetLocked() &&
		(playerCharacter->IsBlocking() || playerAttackState == RE::ATTACK_STATE_ENUM::kBash) ) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = _faceCrosshairDuration;
		_bShouldFaceTarget = true;
		return;
	}

	// face crosshair if we're using a staff. Thanks NickNak for the iState writeup!
	int iState = 0;
	playerCharacter->GetGraphVariableInt("iState", iState);

	auto rightHand = playerCharacter->GetEquippedObject(false);
	if (rightHand) {
		auto rightWeapon = rightHand->As<RE::TESObjectWEAP>();
		if (rightWeapon && rightWeapon->IsBow()) {
			bool bAGOWorkaround = playerAttackState != RE::ATTACK_STATE_ENUM::kBowAttached || (previousState != RE::ATTACK_STATE_ENUM::kNone && previousState != RE::ATTACK_STATE_ENUM::kBowReleased);
			if ((playerAttackState >= RE::ATTACK_STATE_ENUM::kBowDraw && bAGOWorkaround && playerAttackState <= RE::ATTACK_STATE_ENUM::kBowReleased)) {
				SetIsAiming(!HasTargetLocked() || Settings::uTargetLockArrowAimType == kFreeAim);
				_bShouldFaceCrosshair = IsAiming();
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = _faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		} else if (rightWeapon && rightWeapon->IsCrossbow()) {
			if ((playerAttackState >= RE::ATTACK_STATE_ENUM::kBowDrawn && playerAttackState <= RE::ATTACK_STATE_ENUM::kBowReleased)) {
				SetIsAiming(!HasTargetLocked() || Settings::uTargetLockArrowAimType == kFreeAim);
				_bShouldFaceCrosshair = IsAiming();
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = _faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		} else if (rightWeapon && rightWeapon->IsStaff()) {
			if (iState == 10) {
				SetIsAiming(!HasTargetLocked() || Settings::uTargetLockMissileAimType == kFreeAim);
				_bShouldFaceCrosshair = IsAiming();
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = _faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		}

		auto rightSpell = rightHand->As<RE::SpellItem>();
		if (rightSpell && playerCharacter->IsCasting(rightSpell)) {
			if (rightSpell->GetDelivery() != Delivery::kSelf) {
				SetIsAiming(!HasTargetLocked() || rightSpell->GetDelivery() == Delivery::kTargetLocation || Settings::uTargetLockMissileAimType == kFreeAim);
				_bShouldFaceCrosshair = IsAiming();
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = _faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			} else if (Settings::bFaceCrosshairWhileBlocking && rightSpell->avEffectSetting && rightSpell->avEffectSetting->HasKeyword(Settings::kywd_magicWard)) {
				_bShouldFaceCrosshair = true;
				_faceCrosshairTimer = _faceCrosshairDuration;
				_bShouldFaceTarget = true;
				return;
			}
		}
	}

	auto leftHand = playerCharacter->GetEquippedObject(true);
	if (leftHand) {
		auto leftWeapon = leftHand->As<RE::TESObjectWEAP>();
		if (leftWeapon && leftWeapon->IsStaff()) {
			if (iState == 10) {
				SetIsAiming(!HasTargetLocked() || Settings::uTargetLockMissileAimType == kFreeAim);
				_bShouldFaceCrosshair = IsAiming();
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = _faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		}

		auto leftSpell = leftHand->As<RE::SpellItem>();
		if (leftSpell && playerCharacter->IsCasting(leftSpell)) {
			if (leftSpell->GetDelivery() != Delivery::kSelf) {
				SetIsAiming(!HasTargetLocked() || leftSpell->GetDelivery() == Delivery::kTargetLocation || Settings::uTargetLockMissileAimType == kFreeAim);
				_bShouldFaceCrosshair = IsAiming();
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = _faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
			else if (Settings::bFaceCrosshairWhileBlocking && leftSpell->avEffectSetting && leftSpell->avEffectSetting->HasKeyword(Settings::kywd_magicWard)) {
				_bShouldFaceCrosshair = true;
				_faceCrosshairTimer = _faceCrosshairDuration;
				_bShouldFaceTarget = true;
				return;
			}
		}
	}

	SetIsAiming(false);

	if (_aimingTimer <= 0.f) {
		_bIsAiming = false;
	}

	if (_faceCrosshairTimer <= 0.f) {
		_bShouldFaceCrosshair = false;
		_bShouldFaceTarget = false;
	}
}

void DirectionalMovementHandler::UpdateFacingCrosshair()
{
	if (_bShouldFaceCrosshair && !_bYawControlledByPlugin)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerCamera = RE::PlayerCamera::GetSingleton();
		if (playerCharacter && playerCamera && playerCamera->currentState && playerCamera->currentState->id == RE::CameraStates::kThirdPerson)
		{
			auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
			if (playerCharacter->AsActorState()->IsSprinting() && (!Settings::bFaceCrosshairDuringAutoMove || !RE::PlayerControls::GetSingleton()->data.autoMove)) {
				_bCurrentlyTurningToCrosshair = true;
				return;
			}
			else if (!_bIsAiming || fabs(thirdPersonState->freeRotation.x) > FLT_EPSILON) {
				_bCurrentlyTurningToCrosshair = true;

				float currentCharacterRot = playerCharacter->data.angle.z;
				float currentCameraRotOffset = thirdPersonState->freeRotation.x;

				_desiredAngle = NormalAbsoluteAngle(currentCharacterRot + currentCameraRotOffset);

				return;
			}
		}
	}

	_bCurrentlyTurningToCrosshair = false;
}

void DirectionalMovementHandler::UpdateDodgingState()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (APIs::DodgeFramework) {
		playerCharacter->GetGraphVariableBool("DF_IsDodging", _DF_bIsDodging);
		playerCharacter->GetGraphVariableBool("DF_UnlockRotation", _DF_bUnlockRotation);
		playerCharacter->GetGraphVariableBool("DF_UnlockRotationFull", _DF_bUnlockRotationFull);
	} else {  // no dodge framework, old code
		bool bWasDodging = _bIsDodging_Legacy;
		playerCharacter->GetGraphVariableBool("TDM_Dodge", _bIsDodging_Legacy);
		_bJustDodged_Legacy = !bWasDodging && _bIsDodging_Legacy;
		if (_bJustDodged_Legacy && !playerCharacter->IsAnimationDriven()) {
			_faceCrosshairTimer = 0.f;
		}
	}
}

void DirectionalMovementHandler::UpdateJumpingState()
{
	auto playerController = RE::PlayerCharacter::GetSingleton()->GetCharController();
	if (playerController && Is360Movement()) {
		if (playerController->wantState == RE::hkpCharacterStateType::kJumping) {
			UpdateRotation(true);
		}
	}
}

void DirectionalMovementHandler::UpdateSwimmingPitchOffset()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter && playerCharacter->AsActorState()->IsSwimming()) {
		_currentSwimmingPitchOffset = InterpTo(_currentSwimmingPitchOffset, _desiredSwimmingPitchOffset, GetPlayerDeltaTime(), Settings::fSwimmingPitchSpeed);
	}
}

void DirectionalMovementHandler::UpdateMountedArchery()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	bool bEnable = IsMountedArcheryPatchInstalled(playerCharacter);

	if (APIs::SmoothCam) {
		APIs::SmoothCam->EnableUnlockedHorseAim(bEnable);

		bEnable = bEnable && GetCurrentlyMountedAiming();
		if (bEnable) {
			if (!_mountedArcheryRequestedSmoothCamCrosshair) {
				auto result = APIs::SmoothCam->RequestCrosshairControl(SKSE::GetPluginHandle(), true);
				if (result == SmoothCamAPI::APIResult::OK || result == SmoothCamAPI::APIResult::AlreadyGiven) {
					_mountedArcheryRequestedSmoothCamCrosshair = true;
				}
			}			
		}
		else {
			if (_mountedArcheryRequestedSmoothCamCrosshair) {
				_mountedArcheryRequestedSmoothCamCrosshair = false;
				if (!_targetLockRequestedSmoothCamCrosshair) {
					APIs::SmoothCam->ReleaseCrosshairControl(SKSE::GetPluginHandle());
				}
			}
		}
	}
}

void DirectionalMovementHandler::ProgressTimers()
{
	const float playerDeltaTime = GetPlayerDeltaTime();
	const float realTimeDeltaTime = GetRealTimeDeltaTime();
	if (_dialogueHeadtrackTimer > 0.f) {
		_dialogueHeadtrackTimer -= playerDeltaTime;
	}
	if (_lastTargetSwitchTimer > 0.f) {
		_lastTargetSwitchTimer -= realTimeDeltaTime;
	}
	if (_lastLOSTimer > 0.f) {
		_lastLOSTimer -= playerDeltaTime;
	}
	if (_faceCrosshairTimer > 0.f) {
		_faceCrosshairTimer -= playerDeltaTime;
	}
	if (_aimingTimer > 0.f) {
		_aimingTimer -= playerDeltaTime;
	}
	if (_cameraHeadtrackTimer > 0.f) {
		_cameraHeadtrackTimer -= playerDeltaTime;
	}
	if (_cameraRotationDelayTimer > 0.f) {
		_cameraRotationDelayTimer -= realTimeDeltaTime;
	}
	if (_tutorialHintTimer > 0.f) {
		_tutorialHintTimer -= realTimeDeltaTime;
	}
}

void DirectionalMovementHandler::OnDodge()
{
	if (APIs::DodgeFramework) {
		_faceCrosshairTimer = 0.f;
		if (Is360Movement()) {
			UpdateRotation(true);  // immediately rotate to desired direction on dodge, for responsiveness
		}
	}
}

void DirectionalMovementHandler::UpdateProjectileTargetMap()
{
	if (!_projectileTargets.empty()) {
		for (auto it = _projectileTargets.begin(), next_it = it; it != _projectileTargets.end(); it = next_it) {
			++next_it;

			if (!it->first) {
				_projectileTargets.erase(it);
			}
		}
	}
}

void DirectionalMovementHandler::UpdateLeaning(RE::Actor* a_actor, [[maybe_unused]] float a_deltaTime)
{
	if (!Settings::bEnableLeaning) {
		return;
	}

	if (!a_actor) {
		return;
	}

	auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
	if (!currentProcess || !currentProcess->InHighProcess()) {
		return;
	}

	if (!Settings::bEnableLeaningNPC && !a_actor->IsPlayerRef()) {
		return;
	}

	auto characterController = a_actor->GetCharController();
	if (!characterController) {
		return;
	}

	RE::NiPoint3 previousVelocity;
	bool bFound = a_actor->GetGraphVariableFloat("TDM_VelocityX", previousVelocity.x);
	if (!bFound) {
		return;
	}
	a_actor->GetGraphVariableFloat("TDM_VelocityY", previousVelocity.y);

	float desiredPitch = 0.f;
	float desiredRoll = 0.f;

	const float playerDeltaTime = GetPlayerDeltaTime();

	if (a_actor->AsActorState()->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kNone) {
		float quad[4];
		_mm_store_ps(quad, characterController->forwardVec.quad);
		RE::NiPoint3 worldVelocity{ -quad[0], -quad[1], 0.f };
		RE::NiPoint3 upVector{ 0.f, 0.f, 1.f };

		worldVelocity *= characterController->speedPct * Settings::fLeaningMult;

		a_actor->SetGraphVariableFloat("TDM_VelocityX", worldVelocity.x);
		a_actor->SetGraphVariableFloat("TDM_VelocityY", worldVelocity.y);

		// calculate acceleration
		RE::NiPoint3 worldAcceleration = RE::NiPoint3::Zero();
		
		if (playerDeltaTime > 0.f) {
			worldAcceleration = (worldVelocity - previousVelocity) / playerDeltaTime;
		}

		worldAcceleration *= worldAcceleration.Dot(worldVelocity) > 0 ? 1.f : 0.5f;
		worldAcceleration = ClampSizeMax(worldAcceleration, Settings::fMaxLeaningStrength);  // clamp to sane values
		auto acceleration = RotateAngleAxis(worldAcceleration, a_actor->data.angle.z, upVector);

		// get desired lean
		desiredPitch = acceleration.y;
		desiredRoll = acceleration.x;

//#ifndef NDEBUG
//		if (g_trueHUD) {
//			RE::NiPoint3 rootPos = a_actor->GetPosition();
//			RE::NiPoint3 pos = rootPos + RE::NiPoint3{ 0.f, 0.f, 50.f };
//			g_trueHUD->DrawArrow(rootPos, rootPos + worldVelocity);
//			g_trueHUD->DrawArrow(pos, pos + worldAcceleration, 10.f, 0.f, 0xFFFF00FF);
//		}
//#endif
	}

	float pitch, roll;
	a_actor->GetGraphVariableFloat("TDM_Pitch", pitch);
	a_actor->GetGraphVariableFloat("TDM_Roll", roll);

	// interpolate
	roll = InterpTo(roll, desiredRoll, playerDeltaTime, Settings::fLeaningSpeed);
	pitch = InterpTo(pitch, desiredPitch, playerDeltaTime, Settings::fLeaningSpeed);

	// update angles
	a_actor->SetGraphVariableFloat("TDM_Pitch", pitch);
	a_actor->SetGraphVariableFloat("TDM_Roll", roll);
	a_actor->SetGraphVariableFloat("TDM_SpineTurn", roll);
}

void DirectionalMovementHandler::UpdateCameraAutoRotation()
{
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	if (playerCamera && playerCamera->currentState && (playerCamera->currentState->id == RE::CameraState::kThirdPerson || playerCamera->currentState->id == RE::CameraState::kMount)) {
		RE::Actor* cameraTarget = nullptr;
		auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
		bool bIsMounted = thirdPersonState->id == RE::CameraState::kMount;

		if (bIsMounted) {
			auto horseCameraState = static_cast<RE::HorseCameraState*>(thirdPersonState);
			cameraTarget = horseCameraState->horseRefHandle.get()->As<RE::Actor>();
		} else {
			cameraTarget = RE::PlayerCharacter::GetSingleton();
		}
		
		if (!GetFreeCameraEnabled() || (!IsFreeCamera() && !bIsMounted) || _bShouldFaceCrosshair || IsCameraResetting() || HasTargetLocked() || _cameraRotationDelayTimer > 0.f) {
			_currentAutoCameraRotationSpeed = 0.f;
			return;
		}

		float desiredSpeed = 0.f;

		auto characterController = cameraTarget->GetCharController();
		if (characterController) {
			float speedPct = characterController->speedPct;
			if (speedPct > 0.f) {
				bool bOnlyDuringSprint = Settings::uAdjustCameraYawDuringMovement == CameraAdjustMode::kDuringSprint;
				bool bIsSprinting = cameraTarget->AsActorState()->IsSprinting();
				if (bOnlyDuringSprint && !bIsSprinting) {
					desiredSpeed = 0.f;
				} else {
					desiredSpeed = -sin(thirdPersonState->freeRotation.x) * speedPct * Settings::fCameraAutoAdjustSpeedMult;
				}
			}
		}

		const float realTimeDeltaTime = GetRealTimeDeltaTime();
		_currentAutoCameraRotationSpeed = InterpTo(_currentAutoCameraRotationSpeed, desiredSpeed, realTimeDeltaTime, 5.f);
		thirdPersonState->freeRotation.x += _currentAutoCameraRotationSpeed * realTimeDeltaTime;
	}
}

bool DirectionalMovementHandler::IsCrosshairVisible() const
{
	auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
	RE::GFxValue crosshairState;
	hud->uiMovie->GetVariable(&crosshairState, "HUDMovieBaseInstance.bCrosshairEnabled");
	return crosshairState.GetBool();
}

void DirectionalMovementHandler::HideCrosshair()
{
	// Hide crosshair if the option is on.
	if (Settings::bTargetLockHideCrosshair) {
		// Request control over crosshair from SmoothCam.
		bool bCanControlCrosshair = false;
		if (APIs::SmoothCam && !_targetLockRequestedSmoothCamCrosshair) {
			auto result = APIs::SmoothCam->RequestCrosshairControl(SKSE::GetPluginHandle(), true);
			if (result == SmoothCamAPI::APIResult::OK || result == SmoothCamAPI::APIResult::AlreadyGiven) {
				_targetLockRequestedSmoothCamCrosshair = true;
				bCanControlCrosshair = true;
			}
		} else {
			bCanControlCrosshair = true;
		}

		if (bCanControlCrosshair) {
			auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
			if (hud && hud->uiMovie) {
				bool bIsCrosshairVisible = IsCrosshairVisible();
				if (bIsCrosshairVisible) {
					const RE::GFxValue bFalse{ false };
					//hud->uiMovie->SetVariable("HUDMovieBaseInstance.Crosshair._visible", bFalse);
					hud->uiMovie->Invoke("HUDMovieBaseInstance.SetCrosshairEnabled", nullptr, &bFalse, 1);
					_bCrosshairIsHidden = true;
				}
			}
		}
	}
}

void DirectionalMovementHandler::ShowCrosshair()
{
	// Show crosshair if it has been hidden.
	if (_bCrosshairIsHidden) {
		bool bCanControlCrosshair = false;
		// Check if we have control over crosshair from SmoothCam
		if (APIs::SmoothCam && _targetLockRequestedSmoothCamCrosshair) {
			auto pluginHandle = APIs::SmoothCam->GetCrosshairOwner();
			if (pluginHandle == SKSE::GetPluginHandle()) {
				bCanControlCrosshair = true;
			}
		} else {
			bCanControlCrosshair = true;
		}

		if (bCanControlCrosshair) {
			auto hud = RE::UI::GetSingleton()->GetMenu(RE::HUDMenu::MENU_NAME);
			if (hud && hud->uiMovie) {
				const RE::GFxValue bTrue{ true };
				//hud->uiMovie->SetVariable("HUDMovieBaseInstance.Crosshair._visible", bTrue);
				hud->uiMovie->Invoke("HUDMovieBaseInstance.SetCrosshairEnabled", nullptr, &bTrue, 1);
			}

			// Release control over crosshair to SmoothCam.
			if (APIs::SmoothCam && _targetLockRequestedSmoothCamCrosshair) {
				_targetLockRequestedSmoothCamCrosshair = false;
				if (!_mountedArcheryRequestedSmoothCamCrosshair) {
					APIs::SmoothCam->ReleaseCrosshairControl(SKSE::GetPluginHandle());
				}
			}
		}

		_bCrosshairIsHidden = false;
	}
}

void DirectionalMovementHandler::SetIsAiming(bool a_bIsAiming)
{
	if (a_bIsAiming) {
		_bIsAiming = true;
		_aimingTimer = _aimingDuration;
	}
}

bool DirectionalMovementHandler::ProcessInput(RE::NiPoint2& a_inputDirection, RE::PlayerControlsData* a_playerControlsData)
{
	_actualInputDirection = a_inputDirection;

	if (a_playerControlsData->fovSlideMode) {
		return false;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
	if (!playerCharacter || !thirdPersonState) {
		return false;
	}

	// Skip if player's yaw is controlled by another plugin
	if (_bYawControlledByPlugin) {
		return false;
	}

	// Skip if player is AI driven
	if (IsPlayerAIDriven()) {
		ResetDesiredAngle();
		return false;
	}

	// Skip if player is animation driven
	if (IsPlayerAnimationDriven()) {
		ResetDesiredAngle();
		return false;
	}

	_bHasMovementInput = true;

	if (_bShouldFaceCrosshair && !playerCharacter->AsActorState()->IsSprinting()) {
		if (!_bCurrentlyTurningToCrosshair) {
			ResetDesiredAngle();
		}
		return false;  // let the hook do the rotation
	}

	float currentCharacterRot = playerCharacter->data.angle.z;
	float currentCameraRotOffset = thirdPersonState->freeRotation.x;

	RE::NiPoint2 normalizedInputDirection = a_inputDirection;
	float inputLength = normalizedInputDirection.Unitize();

	if (Settings::bThumbstickBounceFix && inputLength < 0.25f && DetectInputAnalogStickBounce()) {
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec.x = 0.f;
		a_playerControlsData->moveInputVec.y = 0.f;
		SetLastInputDirection(a_playerControlsData->moveInputVec);
		ResetDesiredAngle();
		return true;
	}

	if (Settings::bThumbstickBounceFix) {
		SetLastInputDirection(normalizedInputDirection);
	}
	
	RE::NiPoint2 forwardVector(0.f, 1.f);
	RE::NiPoint2 characterDirection = Vec2Rotate(forwardVector, currentCharacterRot);
	RE::NiPoint2 cameraRelativeInputDirection = Vec2Rotate(a_inputDirection, -currentCameraRotOffset);

	RE::NiPoint2 worldRelativeInputDirection = Vec2Rotate(cameraRelativeInputDirection, currentCharacterRot);

	RE::NiPoint2 normalizedWorldRelativeInputDirection = worldRelativeInputDirection;
	normalizedWorldRelativeInputDirection.Unitize();

	if (normalizedInputDirection.x == 0.f && normalizedInputDirection.y == 0.f) {
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec = a_inputDirection;
		return true;
	}

	if (HasTargetLocked() && (HasDodgeRotationLock())) {
		// don't rotate when dodging in target lock
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec = a_inputDirection;
		return true;
	}

	bool bWantsToSprint = playerCharacter->GetPlayerRuntimeData().playerFlags.isSprinting;

	if ((HasTargetLocked() && !bWantsToSprint) || IsMagnetismActive() || (Settings::uDialogueMode == DialogueMode::kFaceSpeaker && _dialogueSpeaker)) {
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec = cameraRelativeInputDirection;

		return true;
	}

	_desiredAngle = NormalAbsoluteAngle(-GetAngle(characterDirection, cameraRelativeInputDirection));

	bool bPivoting = false;

	if (!playerCharacter->IsInMidair() || !Settings::bOverrideAcrobatics) {
		float dot = characterDirection.Dot(normalizedWorldRelativeInputDirection);
		bPivoting = dot < 0.f;
		if (dot < -0.8f) {
			playerCharacter->NotifyAnimationGraph("TDM_Turn_180");
		}
	}

	bool bShouldStop = Settings::bStopOnDirectionChange && RE::BSTimer::QGlobalTimeMultiplier() == 1;

	a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
	a_playerControlsData->moveInputVec.x = 0.f;
	a_playerControlsData->moveInputVec.y = bShouldStop && bPivoting ? 0.01f : inputLength;  // reduce input to almost 0 when trying to move in opposite direction

	return true;
}

void DirectionalMovementHandler::SetDesiredAngleToTarget(RE::PlayerCharacter* a_playerCharacter, RE::ActorHandle a_target)
{
	if (_bYawControlledByPlugin) {
		return;
	}

	auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
	if (!a_playerCharacter || !thirdPersonState) {
		return;
	}

	if (a_playerCharacter->IsOnMount()) {
		return;
	}

	if (_bShouldFaceCrosshair) {
		return;
	}

	if ((!Settings::bHeadtracking || GetForceDisableHeadtracking()) && _bACCInstalled && Settings::uDialogueMode == DialogueMode::kFaceSpeaker && _dialogueSpeaker) {
		return;
	}

	bool bIsDodging = false;
	a_playerCharacter->GetGraphVariableBool("TDM_Dodge", bIsDodging);
	if (a_playerCharacter->GetPlayerRuntimeData().playerFlags.isSprinting || bIsDodging) {
		return;
	}

	auto target = a_target.get();

	if (!target) {
		return;
	}

	RE::NiPoint2 playerPos;
	playerPos.x = a_playerCharacter->GetPositionX();
	playerPos.y = a_playerCharacter->GetPositionY();
	RE::NiPoint3 targetPos = _currentTargetPoint ? _currentTargetPoint->world.translate : target->GetLookingAtLocation();

	RE::NiPoint2 directionToTarget = RE::NiPoint2(-(targetPos.x - playerPos.x), targetPos.y - playerPos.y);
	directionToTarget.Unitize();

	RE::NiPoint2 forwardVector(0.f, 1.f);
	RE::NiPoint2 currentCharacterDirection = Vec2Rotate(forwardVector, a_playerCharacter->data.angle.z);

	float angleDelta = GetAngle(currentCharacterDirection, directionToTarget);

	if (Settings::bHeadtracking && !GetForceDisableHeadtracking() &&
		(HasTargetLocked() || (Settings::uDialogueMode == DialogueMode::kFaceSpeaker && _dialogueSpeaker)) &&
		!_bShouldFaceTarget &&
		!_bHasMovementInput &&
		_attackState == AttackState::kNone &&
		!RE::PlayerCharacter::GetSingleton()->IsBlocking() &&
		abs(angleDelta) < PI4) 
	{
		return;
	}

	_desiredAngle = NormalAbsoluteAngle(GetAngle(forwardVector, directionToTarget));
}

void DirectionalMovementHandler::UpdateRotation(bool bForceInstant /*= false */)
{
	if (_desiredAngle < 0.f) {
		return;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
	if (!playerCharacter || !thirdPersonState) {
		return;
	}

	if (playerCharacter->IsOnMount()) {
		return;
	}

	float angleDelta = NormalRelativeAngle(_desiredAngle - playerCharacter->data.angle.z);

	bool bInstantRotation = bForceInstant || (_bShouldFaceCrosshair && Settings::bFaceCrosshairInstantly) || (_bShouldFaceCrosshair && !_bCurrentlyTurningToCrosshair) || (_bJustDodged_Legacy && !playerCharacter->IsAnimationDriven()) || (_bYawControlledByPlugin && _controlledYawRotationSpeedMultiplier <= 0.f);

	const float playerDeltaTime = GetPlayerDeltaTime();

	if (!bInstantRotation) {
		if (IsPlayerAnimationDriven() || HasDodgeRotationLock() || IsTDMRotationLocked()) {
			ResetDesiredAngle();
			return;
		}

		float rotationSpeedMult = PI;
		bool bRelativeSpeed = true;

		auto playerActorState = playerCharacter->AsActorState();

		if (playerActorState->IsSwimming()) {
			rotationSpeedMult *= Settings::fSwimmingRotationSpeedMult;
		} else if (_bYawControlledByPlugin) {
			rotationSpeedMult *= _controlledYawRotationSpeedMultiplier;
		} else {
			// Get the current movement type
			RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
			playerCharacter->GetAnimationGraphManager(animationGraphManagerPtr);

			RE::BSFixedString string;
			RE::BGSMovementType* movementType = nullptr;

			if (animationGraphManagerPtr) {
				auto& graphManagerRuntimeData = animationGraphManagerPtr->GetRuntimeData();
				auto activeGraph = graphManagerRuntimeData.activeGraph;				
				
				RE::BShkbAnimationGraph* animationGraph = animationGraphManagerPtr->graphs[activeGraph].get();
				BShkbAnimationGraph_GetCurrentMovementTypeName(animationGraph, &string);

				const char* stringCstr = string.c_str();
				movementType = GetMovementTypeFromString(&stringCstr);

				if (movementType) {
					rotationSpeedMult = movementType->movementTypeData.defaultData.rotateWhileMovingRun;
				}
			}

			bool bSkipAttackRotationMultipliers = false;

			if (Settings::bDisableAttackRotationMultipliersForTransformations) {
				auto raceFormID = playerCharacter->GetRace()->GetFormID();
				if (raceFormID == werewolfFormID || raceFormID == vampireLordFormID) {
					bSkipAttackRotationMultipliers = true;
				}
			}

			RE::ATTACK_STATE_ENUM playerAttackState = playerActorState->GetAttackState();
			bool bIsAttacking = playerAttackState > RE::ATTACK_STATE_ENUM::kNone && playerAttackState < RE::ATTACK_STATE_ENUM::kBowDraw;
			if (playerCharacter->IsInMidair()) {
				bool bGliding = false;
				playerCharacter->GetGraphVariableBool("bParaGliding", bGliding);
				rotationSpeedMult *= bGliding ? Settings::fGlidingRotationSpeedMult : Settings::fAirRotationSpeedMult;
				bRelativeSpeed = false;
			} else if (!bSkipAttackRotationMultipliers && bIsAttacking) {
				if (_attackState == AttackState::kStart) {
					rotationSpeedMult *= Settings::fAttackStartRotationSpeedMult;
					bRelativeSpeed = false;
				} else if (_attackState == AttackState::kMid) {
					rotationSpeedMult *= Settings::fAttackMidRotationSpeedMult;
					bRelativeSpeed = false;
				} else if (_attackState == AttackState::kEnd) {
					rotationSpeedMult *= Settings::fAttackEndRotationSpeedMult;
					bRelativeSpeed = false;
				}
			} else if (playerActorState->IsSprinting()) {
				rotationSpeedMult *= Settings::fSprintingRotationSpeedMult;
			} else if (_bCurrentlyTurningToCrosshair) {
				rotationSpeedMult *= Settings::fFaceCrosshairRotationSpeedMultiplier;
			} else {
				rotationSpeedMult *= Settings::fRunningRotationSpeedMult;
			}

			if (_DF_bIsDodging && _DF_bUnlockRotation && !_DF_bUnlockRotationFull) {
				rotationSpeedMult *= Settings::fDodgeUnlockedRotationSpeedMult;
			}

			// multiply it by water speed mult
			float submergeLevel = TESObjectREFR_GetSubmergeLevel(playerCharacter, playerCharacter->data.location.z, playerCharacter->parentCell);
			if (submergeLevel > 0.18f) {
				rotationSpeedMult *= 0.69f - submergeLevel + ((0.31f + submergeLevel) * Settings::fWaterRotationSpeedMult);
			}
		}
		
		if (rotationSpeedMult <= 0.f) {
			return;
		}

		// multiply rotation speed by the inverse of slow time multiplier to effectively ignore it
		if (Settings::bIgnoreSlowTime) {
			float gtm = RE::BSTimer::QGlobalTimeMultiplier();
			rotationSpeedMult /= gtm;
		}

		float maxAngleDelta = rotationSpeedMult * playerDeltaTime;
		if (bRelativeSpeed) {
			maxAngleDelta *= (1.f + abs(angleDelta));
		}

		angleDelta = ClipAngle(angleDelta, -maxAngleDelta, maxAngleDelta);
	}

	float aiProcessRotationSpeed = angleDelta * (1 / playerDeltaTime);
	SetDesiredAIProcessRotationSpeed(aiProcessRotationSpeed);
	
	playerCharacter->SetHeading(playerCharacter->data.angle.z + angleDelta);

	thirdPersonState->freeRotation.x = NormalRelativeAngle(thirdPersonState->freeRotation.x - angleDelta);

	if (_bIsTweening) {
		_yawDelta += angleDelta;
	}

	if (angleDelta * angleDelta < FLT_EPSILON) {
		ResetDesiredAngle();
	}
}

void DirectionalMovementHandler::UpdateRotationLockedCam()
{
	if (_bIsAiming) {
		return;
	}

	if (!_target) {
		return;
	}

	RE::NiPoint3 targetPos = _currentTargetPoint ? _currentTargetPoint->world.translate : _target.get()->GetLookingAtLocation();

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter) {
		return;
	}

	RE::NiPoint3 playerPos;
	if (!GetTorsoPos(playerCharacter, playerPos)) {
		return;
	}

	float currentCharacterYaw = playerCharacter->data.angle.z;
	float currentCharacterPitch = playerCharacter->data.angle.x;

	RE::NiPoint3 cameraPos = GetCameraPos();

	RE::NiPoint3 playerToTarget = RE::NiPoint3(-(targetPos.x - playerPos.x), targetPos.y - playerPos.y, targetPos.z - playerPos.z);
	RE::NiPoint3 playerDirectionToTarget = playerToTarget;
	playerDirectionToTarget.Unitize();

	RE::NiPoint2 playerDirectionToTargetXY(playerToTarget.x, playerToTarget.y);
	playerDirectionToTargetXY.Unitize();

	// yaw
	RE::NiPoint2 forwardVector(0.f, 1.f);
	RE::NiPoint2 currentPlayerDirection = Vec2Rotate(forwardVector, currentCharacterYaw);

	float angleDelta = GetAngle(currentPlayerDirection, playerDirectionToTargetXY);
	angleDelta = NormalRelativeAngle(angleDelta);

	const float realTimeDeltaTime = GetRealTimeDeltaTime();

	float desiredCharacterYaw = currentCharacterYaw + angleDelta;
	playerCharacter->SetHeading(InterpAngleTo(currentCharacterYaw, desiredCharacterYaw, realTimeDeltaTime, Settings::fTargetLockYawAdjustSpeed));

	// pitch
	RE::NiPoint3 playerAngle = ToOrientationRotation(playerDirectionToTarget);
	float desiredPlayerPitch = -playerAngle.x;

	playerCharacter->SetLooking(InterpAngleTo(currentCharacterPitch, desiredPlayerPitch, realTimeDeltaTime, Settings::fTargetLockPitchAdjustSpeed));
}

void DirectionalMovementHandler::UpdateTweeningState()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter) {
		auto movementController = playerCharacter->GetActorRuntimeData().movementController;
		if (movementController && movementController->unk0F8) {
			uintptr_t tweenerArbiterPtr = movementController->unk0F8 - 0x20;

			bool bWasTweening = _bIsTweening;
			_bIsTweening = *(bool*)tweenerArbiterPtr + 0x170;

			if (!_bIsTweening && bWasTweening) {  // was tweening but is not anymore - reset yaw delta
				ResetYawDelta();
			}
		}
	}
}

bool DirectionalMovementHandler::IFPV_IsFirstPerson() const
{
	if (_IFPV_IsFirstPerson) {
		return _IFPV_IsFirstPerson->value;
	}

	return false;
}

bool DirectionalMovementHandler::ImprovedCamera_IsFirstPerson() const
{
	if (_ImprovedCamera_IsFirstPerson) {
		return *_ImprovedCamera_IsFirstPerson;
	}

	return false;
}

bool DirectionalMovementHandler::IsImprovedCameraInstalled() const
{
	if (_ImprovedCamera_IsFirstPerson) {
		return true;
	}

	return false;
}

TDM_API::DirectionalMovementMode DirectionalMovementHandler::GetDirectionalMovementMode() const
{
	if (IsFreeCamera()) {
		if (Is360Movement()) {
			if (HasTargetLocked()) {
				return TDM_API::DirectionalMovementMode::kTargetLock;
			}
			return TDM_API::DirectionalMovementMode::kDirectional;
		}
		return TDM_API::DirectionalMovementMode::kVanillaStyle;
	}
	return TDM_API::DirectionalMovementMode::kDisabled;
}

bool DirectionalMovementHandler::GetFreeCameraEnabled() const
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter)
	{
		return (playerCharacter->AsActorState()->GetWeaponState() == RE::WEAPON_STATE::kSheathed ? Settings::uDirectionalMovementSheathed : Settings::uDirectionalMovementDrawn) != DirectionalMovementMode::kDisabled;
	}

	return false;
}

bool DirectionalMovementHandler::IsPlayerAIDriven() const
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	auto& runtimeData = playerCharacter->GetPlayerRuntimeData();
	if (/*runtimeData.playerFlags.aiControlledToPos || runtimeData.playerFlags.aiControlledFromPos || */runtimeData.playerFlags.aiControlledPackage) {
		return true;
	}

	auto& movementController = playerCharacter->GetActorRuntimeData().movementController;
	if (movementController && !movementController->playerControls) {
		return true;
	}

	return false;
}

bool DirectionalMovementHandler::IsPlayerAnimationDriven() const
{
	// workaround for 'IsNPC' issue
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	return playerCharacter->IsAnimationDriven() && !_bCurrentlyTurningToCrosshair && !HasTargetLocked();
}

bool DirectionalMovementHandler::IsTDMRotationLocked() const
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter) {
		bool result = false;
		playerCharacter->GetGraphVariableBool("TDM_LockRotation", result);
		return result;
	}
	
	return false;
}

void DirectionalMovementHandler::ResetCamera()
{
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	if (playerCamera->currentState && playerCamera->currentState->id == RE::CameraState::kThirdPerson || playerCamera->currentState->id == RE::CameraState::kMount) {
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
		_desiredCameraAngleX = playerCharacter->data.angle.z;
		if (Settings::bResetCameraPitch) {
			_desiredCameraAngleY = playerCharacter->data.angle.x + thirdPersonState->freeRotation.y;
		}

		_bResetCamera = true;
	}
}

bool DirectionalMovementHandler::ToggleTargetLock(bool bEnable, bool bPressedManually /*= false */)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (bEnable)
	{
		if (RE::MenuTopicManager::GetSingleton()->speaker) // don't enter lockon during dialogue
		{
			return false;
		}

		RE::ActorHandle actor = FindTarget(bPressedManually ? TargetLockSelectionMode::kCombined : TargetLockSelectionMode::kClosest);
		if (actor) 
		{
			SetTarget(actor);

			// Set graph variable
			playerCharacter->SetGraphVariableBool("TDM_TargetLock", true);

			// Add spell so DAR can detect target lock
			if (Settings::spel_targetLockSpell) {
				playerCharacter->AddSpell(Settings::spel_targetLockSpell);
			}

			return true;
		}

		// if we're here, this means toggle target lock was called and there was no valid target to be found, so reset camera if we should and fall through to disable a target lock if it's enabled
		if (bPressedManually && Settings::bResetCameraWithTargetLock) {
			ResetCamera();
		}
	}

	if (!bEnable || HasTargetLocked())
	{
		SetTarget(RE::ActorHandle());

		// Set graph variable
		playerCharacter->SetGraphVariableBool("TDM_TargetLock", false);

		// Remove spell so DAR can detect target lock
		if (Settings::spel_targetLockSpell) {
			playerCharacter->RemoveSpell(Settings::spel_targetLockSpell);
		}

		//ShowCrosshair();

		_lastLOSTimer = _lostSightAllowedDuration;

		auto playerCamera = RE::PlayerCamera::GetSingleton();
		// If on a mount, set player and horse pitch to avoid camera snap
		if (playerCharacter->IsOnMount() && playerCamera->currentState && playerCamera->currentState->id == RE::CameraState::kMount) {
			auto horseCameraState = static_cast<RE::HorseCameraState*>(playerCamera->currentState.get());
			playerCharacter->data.angle.x = -horseCameraState->freeRotation.y;
			//horseCameraState->freeRotation.y = 0;

			if (auto horseRefPtr = horseCameraState->horseRefHandle.get()) {
				auto horse = horseRefPtr->As<RE::Actor>();
				if (horse) {
					horse->data.angle.x = -horseCameraState->freeRotation.y;
				}
			}
		}

		return true;
	}

	return false;
}

RE::ActorHandle DirectionalMovementHandler::GetTarget() const
{
	//return HasTargetLocked() ? _target : _softTarget;
	return _target;
}

RE::NiPointer<RE::NiAVObject> DirectionalMovementHandler::GetTargetPoint() const
{
	return _currentTargetPoint;
}

RE::NiPoint3 DirectionalMovementHandler::GetTargetPosition() const
{
	if (_currentTargetPoint) {
		return _currentTargetPoint->world.translate;
	} else if (_target) {
		return _target.get()->GetLookingAtLocation();
	}

	return RE::NiPoint3();
}

void DirectionalMovementHandler::ClearTargets()
{
	if (_target)
	{
		ToggleTargetLock(false);
	}
	if (_softTarget)
	{
		SetSoftTarget(RE::ActorHandle());
	}
}

void DirectionalMovementHandler::OverrideControllerBufferDepth(bool a_override)
{
	if (a_override)	{
		if (_defaultControllerBufferDepth == -1.f) {
			_defaultControllerBufferDepth = *g_fControllerBufferDepth;
		}
		*g_fControllerBufferDepth = Settings::fControllerBufferDepth;
	}
	else if (_defaultControllerBufferDepth > 0.f) {
		*g_fControllerBufferDepth = _defaultControllerBufferDepth;
	}
}

float DirectionalMovementHandler::GetTargetLockDistanceRaceSizeMultiplier(RE::TESRace* a_race) const
{
	if (a_race) {
		switch (a_race->data.raceSize.get())
		{
		case RE::RACE_SIZE::kMedium:
		default:
			return 1.f;
		case RE::RACE_SIZE::kSmall:
			return Settings::fTargetLockDistanceMultiplierSmall;
		case RE::RACE_SIZE::kLarge:
			return Settings::fTargetLockDistanceMultiplierLarge;
		case RE::RACE_SIZE::kExtraLarge:
			return Settings::fTargetLockDistanceMultiplierExtraLarge;
		}
	}

	return 1.f;
}

bool DirectionalMovementHandler::CheckCurrentTarget(RE::ActorHandle a_target, bool bInstantLOS /*= false*/)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (!a_target)
	{
		return false;
	}
	
	auto target = a_target.get();
	if (!target || target->IsDisabled() || !target->parentCell || target->parentCell->cellDetached) {
		return false;
	}

	auto actorState = target->AsActorState();
	auto currentProcess = target->GetActorRuntimeData().currentProcess;
	if (!currentProcess || !currentProcess->InHighProcess() ||
		target->IsDead() ||
		(actorState->IsBleedingOut() && target->IsEssential()) ||
		target->GetPosition().GetDistance(playerCharacter->GetPosition()) > (Settings::fTargetLockDistance * GetTargetLockDistanceRaceSizeMultiplier(target->GetRace()) * _targetLockDistanceHysteresis) ||
		target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kInvisibility) > 0 ||
		//RE::UI::GetSingleton()->IsMenuOpen("Dialogue Menu"))
		RE::MenuTopicManager::GetSingleton()->speaker)
	{
		return false;
	}

	RE::ActorPtr playerMount = nullptr;
	if (playerCharacter->GetMount(playerMount) && playerMount.get() == a_target.get().get())
		return false;

	if (Settings::bTargetLockTestLOS)
	{
		if (bInstantLOS)
		{
			bool r8 = false;
			bool bHasLOS = playerCharacter->HasLineOfSight(a_target.get().get(), r8);
			if (!bHasLOS) {
				return false;
			}
		}
		else 
		{
			auto timeNow = std::chrono::system_clock::now();

			bool r8 = false;
			bool bHasLOS = playerCharacter->HasLineOfSight(a_target.get().get(), r8);
			if (bHasLOS) {
				_lastLOSTimer = _lostSightAllowedDuration;
			}

			if (_lastLOSTimer <= 0.f) {
				return false;
			}
		}
	}

	return true;
}

void DirectionalMovementHandler::UpdateTargetLock()
{
	if (HasTargetLocked())
	{		
		auto playerCamera = RE::PlayerCamera::GetSingleton();
		if (playerCamera && playerCamera->currentState->id == RE::CameraStates::kVATS)
		{
			ToggleTargetLock(false);
		}

		if (!CheckCurrentTarget(_target))
		{
			ToggleTargetLock(false);
		}
	}
}

bool DirectionalMovementHandler::IsActorValidTarget(RE::ActorPtr a_actor, bool a_bCheckDistance /*= false*/) const
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (!a_actor || !a_actor.get() || !playerCharacter || a_actor.get() == playerCharacter)
		return false;

	RE::ActorPtr playerMount = nullptr;
	if (playerCharacter->GetMount(playerMount) && playerMount == a_actor)
		return false;
	
	if (a_actor->formType != RE::FormType::ActorCharacter)
		return false;

	if (a_actor->IsDead())
		return false;

	if (a_actor->AsActorState()->IsBleedingOut() && a_actor->IsEssential())
		return false;

	if (a_bCheckDistance && a_actor->GetPosition().GetDistance(playerCharacter->GetPosition()) > Settings::fTargetLockDistance * GetTargetLockDistanceRaceSizeMultiplier(a_actor->GetRace()))
		return false;

	if (a_actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kInvisibility) > 0)
		return false;

	/*if (a_actor->IsPlayerTeammate())
		return false;*/

	if (Settings::bTargetLockHostileActorsOnly && !a_actor->IsHostileToActor(playerCharacter))
		return false;

	bool r8 = false;
	bool bHasLOS = playerCharacter->HasLineOfSight(a_actor.get(), r8);

	if (!bHasLOS)
		return false;

	return true;
}

RE::ActorHandle DirectionalMovementHandler::FindTarget(TargetLockSelectionMode a_mode, bool a_bSkipCurrent /*= true*/)
{
	if (auto crosshairRef = Events::CrosshairRefManager::GetSingleton()->GetCachedRef()) {
		if (auto crosshairRefPtr = crosshairRef.get()) {
			auto crosshairActor = RE::ActorPtr(crosshairRef.get()->As<RE::Actor>());
			if (crosshairActor && crosshairActor != _target.get() && IsActorValidTarget(crosshairActor)) {
				return crosshairActor->GetHandle();
			}
		}
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	RE::NiPoint3 playerPosition;
	if (!GetTorsoPos(playerCharacter, playerPosition)) {
		return RE::ActorHandle();
	}
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	auto& currentCameraState = playerCamera->currentState;
	if (!currentCameraState) {
		return RE::ActorHandle();
	}

	auto& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
	if (actorHandles.size() == 0) {
		return RE::ActorHandle();
	}

	RE::NiPoint3 forwardVector(0.f, 1.f, 0.f);
	//RE::NiPoint3 cameraPosition = playerCamera->pos;
	//RE::NiPoint3 cameraAngle = GetCameraRotation();
	
	RE::ActorHandle bestTarget;
	float bestDistance = FLT_MAX;
	float bestDot = -1.f;
	float bestCombined = FLT_MAX;

	auto getDot = [&](RE::NiPoint3 a_directionVector) {
		RE::NiQuaternion cameraRotation;
		currentCameraState->GetRotation(cameraRotation);
		auto cameraForwardVector = RotateVector(forwardVector, cameraRotation);
		cameraForwardVector.z = 0.f;
		cameraForwardVector.Unitize();

		return cameraForwardVector.Dot(a_directionVector);
	};

	for (auto& actorHandle : actorHandles) {
		auto actor = actorHandle.get();
		if (a_bSkipCurrent && actorHandle == _target) {
		    continue;
		}
		if (IsActorValidTarget(actor)) {
			float targetLockMaxDistance = Settings::fTargetLockDistance * GetTargetLockDistanceRaceSizeMultiplier(actor->GetRace());
			
			auto targetPoint = GetBestTargetPoint(actorHandle);

			RE::NiPoint3 actorPosition = targetPoint ? targetPoint->world.translate : actor->GetLookingAtLocation();
			RE::NiPoint3 directionVector = actorPosition - playerPosition;

			float distance = directionVector.Unitize();
			
			if (distance <= targetLockMaxDistance) {
				switch (a_mode) {
				case TargetLockSelectionMode::kClosest:
					if (distance < bestDistance) {
						bestDistance = distance;
						bestTarget = actorHandle;
					}
					break;
				case TargetLockSelectionMode::kCenter:
					{
						float dot = getDot(directionVector);
						if (dot > bestDot) {
							bestDot = dot;
							bestTarget = actorHandle;
						}
						break;
					}					
				case TargetLockSelectionMode::kCombined:
					{
						float dot = getDot(directionVector);
						float combined = distance * (1.f - dot);
						if (combined < bestCombined) {
							bestCombined = combined;
							bestTarget = actorHandle;
						}
						break;
					}
				}
			}
		}
	}

	return bestTarget;
}

void DirectionalMovementHandler::SwitchTarget(Direction a_direction)
{
	if (a_direction == _lastTargetSwitchDirection && _lastTargetSwitchTimer > 0.f) {
		return;  // too soon
	}

	//RE::ActorHandle actor = GetClosestTarget(FindTargetsByDirection(a_direction), _target);

	if (SwitchTargetPoint(a_direction)) {
		if (APIs::TrueHUD) {
			APIs::TrueHUD->SetTarget(SKSE::GetPluginHandle(), _target);

			if (Settings::bEnableTargetLockReticle) {
				if (_target) {
					if (auto widget = _targetLockReticle.lock()) {
						widget->ChangeTarget(_target, _currentTargetPoint);
					} else {
						AddTargetLockReticle(_target, _currentTargetPoint);
					}
				} else {
					RemoveTargetLockReticle();
				}
			}
		}

		_lastTargetSwitchDirection = a_direction;
		_lastTargetSwitchTimer = 0.25f;

	} else {
		RE::ActorHandle actor = SwitchScreenTarget(a_direction);

		if (actor) {
			SetTarget(actor);
			_lastTargetSwitchDirection = a_direction;
			_lastTargetSwitchTimer = 0.25f;
		}
	}
}

bool DirectionalMovementHandler::SwitchTargetPoint(Direction a_direction)
{
	if (!_target) {
		return false;
	}

	auto targetPoints = GetTargetPoints(_target);
	if (targetPoints.empty()) {
		return false;
	}

	if (!_currentTargetPoint) {
		return false;
	}

	RE::NiPoint3 currentTargetPos = _currentTargetPoint->world.translate;

	float bestScreenDistance = FLT_MAX;
	bool bChangedTargetPoint = false;

	constexpr RE::NiPoint2 upVector{ 0.f, 1.f };
	constexpr RE::NiPoint2 downVector{ 0.f, -1.f };
	constexpr RE::NiPoint2 leftVector{ -1.f, 0.f };
	constexpr RE::NiPoint2 rightVector{ 1.f, 0.f };

	for (auto& targetPoint : targetPoints) {
		if (!targetPoint || targetPoint == _currentTargetPoint) {
			continue;
		}

		RE::NiPoint3 newTargetPos = targetPoint->world.translate;

		RE::NiPoint2 currentTargetScreenPosition;
		float currentTargetDepth;
		RE::NiPoint2 newTargetScreenPosition;
		float newTargetDepth;
		RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, currentTargetPos, currentTargetScreenPosition.x, currentTargetScreenPosition.y, currentTargetDepth, 1e-5f);
		RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, newTargetPos, newTargetScreenPosition.x, newTargetScreenPosition.y, newTargetDepth, 1e-5f);

		if (newTargetDepth < 0.f) {  // offscreen
			continue;
		}

		bool bIsCorrectDirection = false;
		RE::NiPoint2 directionVector;

		switch (a_direction) {
		case Direction::kLeft:
			bIsCorrectDirection = newTargetScreenPosition.x < currentTargetScreenPosition.x;
			directionVector = leftVector;
			break;
		case Direction::kRight:
			bIsCorrectDirection = newTargetScreenPosition.x > currentTargetScreenPosition.x;
			directionVector = rightVector;
			break;
		case Direction::kForward:
		case Direction::kUp:
			bIsCorrectDirection = newTargetScreenPosition.y > currentTargetScreenPosition.y;
			directionVector = upVector;
			break;
		case Direction::kBack:
		case Direction::kDown:
			bIsCorrectDirection = newTargetScreenPosition.y < currentTargetScreenPosition.y;
			directionVector = downVector;
			break;
		}

		if (bIsCorrectDirection) {
			RE::NiPoint2 distanceVector = newTargetScreenPosition - currentTargetScreenPosition;
			float screenDistance = distanceVector.Unitize();
			float directionMult = 2.f - directionVector.Dot(distanceVector);  // so targets that are closer to the desired direction are given better score than closer targets that aren't really in that direction

			if (screenDistance * directionMult < bestScreenDistance) {
				bestScreenDistance = screenDistance;
				_currentTargetPoint = targetPoint;
				bChangedTargetPoint = true;
			}
		}

	}

	return bChangedTargetPoint;
}

RE::ActorHandle DirectionalMovementHandler::SwitchScreenTarget(Direction a_direction)
{
	RE::ActorHandle newTarget;

	auto& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
	if (actorHandles.size() == 0) {
		return newTarget;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	RE::NiPoint3 playerPosition;
	if (!GetTorsoPos(playerCharacter, playerPosition)) {
		return newTarget;
	}

	const RE::NiPoint3 currentTargetPosition = GetTargetPosition();

	float bestScreenDistance = FLT_MAX;

	constexpr RE::NiPoint2 upVector{ 0.f, 1.f };
	constexpr RE::NiPoint2 downVector{ 0.f, -1.f };
	constexpr RE::NiPoint2 leftVector{ -1.f, 0.f };
	constexpr RE::NiPoint2 rightVector{ 1.f, 0.f };

	for (auto& actorHandle : actorHandles) {
		if (actorHandle == _target) {
			continue;
		}

		auto actor = actorHandle.get();
		if (IsActorValidTarget(actor)) {
			auto actorTargetPoint = GetBestTargetPoint(actorHandle);

			float targetLockMaxDistance = Settings::fTargetLockDistance * GetTargetLockDistanceRaceSizeMultiplier(actor->GetRace());
			RE::NiPoint3 newTargetPosition = actorTargetPoint ? actorTargetPoint->world.translate : actor->GetLookingAtLocation();
			float distance = playerPosition.GetDistance(newTargetPosition);

			if (distance <= targetLockMaxDistance) {
				RE::NiPoint2 currentTargetScreenPosition;
				float currentTargetDepth;
				RE::NiPoint2 newTargetScreenPosition;
				float newTargetDepth;
				RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, currentTargetPosition, currentTargetScreenPosition.x, currentTargetScreenPosition.y, currentTargetDepth, 1e-5f);
				RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, newTargetPosition, newTargetScreenPosition.x, newTargetScreenPosition.y, newTargetDepth, 1e-5f);

				if (newTargetDepth < 0.f) {  // offscreen
					continue;
				}

				bool bIsCorrectDirection = false;
				RE::NiPoint2 directionVector;

				switch (a_direction) {
				case Direction::kLeft:
					bIsCorrectDirection = newTargetScreenPosition.x < currentTargetScreenPosition.x;
					directionVector = leftVector;
					break;
				case Direction::kRight:
					bIsCorrectDirection = newTargetScreenPosition.x > currentTargetScreenPosition.x;
					directionVector = rightVector;
					break;
				case Direction::kForward:
				case Direction::kUp:
					bIsCorrectDirection = newTargetScreenPosition.y > currentTargetScreenPosition.y;
					directionVector = upVector;
					break;
				case Direction::kBack:
				case Direction::kDown:
					bIsCorrectDirection = newTargetScreenPosition.y < currentTargetScreenPosition.y;
					directionVector = downVector;
					break;
				}

				if (bIsCorrectDirection) {
					RE::NiPoint2 distanceVector = newTargetScreenPosition - currentTargetScreenPosition;
					float screenDistance = distanceVector.Unitize();
					float directionMult = 2.f - directionVector.Dot(distanceVector); // so targets that are closer to the desired direction are given better score than closer targets that aren't really in that direction

					if (screenDistance * directionMult < bestScreenDistance) {
						bestScreenDistance = screenDistance;
						newTarget = actorHandle;
					}
				}
			}
		}
	}

	return newTarget;
}

std::vector<RE::NiPointer<RE::NiAVObject>> DirectionalMovementHandler::GetTargetPoints(RE::ActorHandle a_actorHandle) const
{
	std::vector<RE::NiPointer<RE::NiAVObject>> ret;

	if (!a_actorHandle) {
		return ret;
	}

	auto actor = a_actorHandle.get().get();
	if (!actor) {
		return ret;
	}

	auto race = actor->GetRace();
	if (!race) {
		return ret;
	}

	RE::BGSBodyPartData* bodyPartData = race->bodyPartData;
	if (!bodyPartData) {
		return ret;
	}

	auto actor3D = actor->Get3D2();
	if (!actor3D) {
		return ret;
	}

	auto it = Settings::targetPoints.find(bodyPartData);
	if (it != Settings::targetPoints.end()) {
		auto& targetPoints = it->second;
		for (auto& targetPoint : targetPoints) {
			auto node = NiAVObject_LookupBoneNodeByName(actor3D, targetPoint, true);
			if (node) {
				ret.push_back(RE::NiPointer<RE::NiAVObject>(node));
			}
		}
		return ret;
	}

	// no custom target points found, fallback to the default target point
	RE::BGSBodyPart* bodyPart = Settings::uReticleAnchor == WidgetAnchor::kBody ? bodyPartData->parts[RE::BGSBodyPartDefs::LIMB_ENUM::kTorso] : bodyPartData->parts[RE::BGSBodyPartDefs::LIMB_ENUM::kHead];
	if (!bodyPart) {
		return ret;
	}

	auto node = NiAVObject_LookupBoneNodeByName(actor3D, bodyPart->targetName, true);
	ret.push_back(RE::NiPointer<RE::NiAVObject>(node));
	return ret;
}

RE::NiPointer<RE::NiAVObject> DirectionalMovementHandler::GetBestTargetPoint(RE::ActorHandle a_actorHandle) const
{
	if (!a_actorHandle) {
		return nullptr;
	}

	auto actor = a_actorHandle.get().get();
	if (!actor) {
		return nullptr;
	}

	auto targetPoints = GetTargetPoints(a_actorHandle);
	if (targetPoints.empty()) {
		return nullptr;
	}

	// just return the first target point on the list
	return targetPoints[0];
}

bool DirectionalMovementHandler::SetDesiredAngleToMagnetismTarget()
{
	if (_bYawControlledByPlugin) {
		_bMagnetismActive = false;
		return false;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter || _bCurrentlyTurningToCrosshair) {
		_bMagnetismActive = false;
		return false;
	}

	RE::ATTACK_STATE_ENUM playerAttackState = playerCharacter->AsActorState()->GetAttackState();
	bool bIsAttacking = playerAttackState > RE::ATTACK_STATE_ENUM::kNone && playerAttackState < RE::ATTACK_STATE_ENUM::kBowDraw;
	bool bIsBlocking = Settings::bMagnetismWhileBlocking ? playerCharacter->IsBlocking() : false;

	if (!bIsBlocking && (!bIsAttacking || _attackState > AttackState::kStart)) {
		_bMagnetismActive = false;
		return false;
	}

	auto& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
	if (actorHandles.size() == 0) {
		_bMagnetismActive = false;
		return false;
	}

	if (!bIsBlocking && IsMagnetismActive()) {
		return true;
	}

	bool bFoundMagnetismTarget = false;
	float magnetismAngleDelta = (Settings::fMeleeMagnetismAngle * PI) / 180.f;
	float smallestDistance = _meleeMagnetismRange;

	auto playerPosition = playerCharacter->GetPosition();

	float desiredAngle = _desiredAngle;
	if (desiredAngle < 0) {
		desiredAngle = playerCharacter->data.angle.z;
	}

	for (auto& actorHandle : actorHandles) {
		auto actor = actorHandle.get();
		if (IsActorValidTarget(actor)) {
			auto actorPosition = actor->GetPosition();
			auto distance = actorPosition.GetDistance(playerPosition);
			if (distance < _meleeMagnetismRange)
			{
				RE::NiPoint2 directionToTarget = RE::NiPoint2(-(actorPosition.x - playerPosition.x), actorPosition.y - playerPosition.y);
				directionToTarget.Unitize();

				RE::NiPoint2 forwardVector(0.f, 1.f);
				RE::NiPoint2 desiredDirection = Vec2Rotate(forwardVector, desiredAngle);

				float angleDelta = NormalRelativeAngle(GetAngle(desiredDirection, directionToTarget));
				//if (GetAngleDiff(angleDelta, magnetismAngleDelta) < fabs(magnetismAngleDelta) || GetAngleDiff(angleDelta, -magnetismAngleDelta) < fabs(magnetismAngleDelta))
				if (fabs(angleDelta) < fabs(magnetismAngleDelta) && distance < smallestDistance)
				{
					smallestDistance = distance;
					magnetismAngleDelta = angleDelta;
					bFoundMagnetismTarget = true;
					SetSoftTarget(actorHandle);
				}
			}
		}
	}

	if (bFoundMagnetismTarget) {
		_desiredAngle = NormalAbsoluteAngle(desiredAngle + magnetismAngleDelta);
		_bMagnetismActive = true;
		return true;
	}

	_bMagnetismActive = false;
	return false;
}

void DirectionalMovementHandler::SetTarget(RE::ActorHandle a_target)
{
	if (_target == a_target) {
		return;
	}

	_target = a_target;

	SetTargetPoint(GetBestTargetPoint(a_target));

	SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE::kDialogue, nullptr);

	if (APIs::TrueHUD) {
		APIs::TrueHUD->SetTarget(SKSE::GetPluginHandle(), a_target);

		if (Settings::bEnableTargetLockReticle) {
			if (a_target) {
				if (auto widget = _targetLockReticle.lock()) {
					widget->ChangeTarget(a_target, _currentTargetPoint);
				} else {
					AddTargetLockReticle(a_target, _currentTargetPoint);
				}
			} else {
				RemoveTargetLockReticle();
			}
		}
	}
	
	if (Settings::bHeadtracking && !GetForceDisableHeadtracking() && _target) {
		SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE::kDialogue, a_target.get().get());
	}
}

void DirectionalMovementHandler::SetSoftTarget(RE::ActorHandle a_softTarget)
{
	if (a_softTarget == _softTarget) {
		return;
	}

	_softTarget = a_softTarget;

	SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE::kCombat, nullptr);

	if (APIs::TrueHUD) {
		APIs::TrueHUD->SetSoftTarget(SKSE::GetPluginHandle(), _softTarget);
	}

	if (Settings::bHeadtracking && !GetForceDisableHeadtracking() && Settings::bHeadtrackSoftTarget && _softTarget) {
		SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE::kCombat, a_softTarget.get().get());
	}
}

RE::NiAVObject* DirectionalMovementHandler::GetProjectileTargetPoint(RE::ObjectRefHandle a_projectileHandle) const
{
	auto it = _projectileTargets.find(a_projectileHandle);
	if (it != _projectileTargets.end() && it->second) {
		return it->second.get();
	}

	return nullptr;
}

void DirectionalMovementHandler::AddProjectileTarget(RE::ObjectRefHandle a_projectileHandle, RE::NiPointer<RE::NiAVObject> a_targetPoint)
{
	_projectileTargets.emplace(a_projectileHandle, a_targetPoint);
}

void DirectionalMovementHandler::RemoveProjectileTarget(RE::ObjectRefHandle a_projectileHandle)
{
	_projectileTargets.erase(a_projectileHandle);
}

void DirectionalMovementHandler::AddTargetLockReticle(RE::ActorHandle a_target, RE::NiPointer<RE::NiAVObject> a_targetPoint)
{
	auto reticleStyle = Settings::uReticleStyle;
	if (reticleStyle == ReticleStyle::kCrosshair && !IsCrosshairVisible()) {
		reticleStyle = ReticleStyle::kCrosshairNoTransform;
	}
	auto widget = std::make_shared<Scaleform::TargetLockReticle>(a_target.native_handle(), a_target, a_targetPoint, reticleStyle);
	_targetLockReticle = widget;

	APIs::TrueHUD->AddWidget(SKSE::GetPluginHandle(), 'LOCK', 0, "TDM_TargetLockReticle", widget);
}

void DirectionalMovementHandler::ReticleRemoved()
{
	DirectionalMovementHandler::GetSingleton()->_bReticleRemoved = true;
}

void DirectionalMovementHandler::RemoveTargetLockReticle()
{
	APIs::TrueHUD->RemoveWidget(SKSE::GetPluginHandle(), 'LOCK', 0, TRUEHUD_API::WidgetRemovalMode::Normal);
}

void DirectionalMovementHandler::SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE a_headtrackType, RE::TESObjectREFR* a_target)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto currentProcess = playerCharacter->GetActorRuntimeData().currentProcess;	
	if (a_target) {
		currentProcess->high->SetHeadtrackTarget(a_headtrackType, a_target);
	} else {
		currentProcess->high->SetHeadtrackTarget(a_headtrackType, nullptr);
		currentProcess->high->SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE::kDefault, nullptr);
	}
}

void DirectionalMovementHandler::UpdateCameraHeadtracking()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto playerCamera = RE::PlayerCamera::GetSingleton();

	if (!playerCharacter || !playerCamera || !playerCamera->currentState || !playerCamera->cameraRoot) {
		return;
	}

	float cameraPitchOffset = 0.f;
	float cameraYawOffset = 0.f;

	if (playerCamera->currentState->id == RE::CameraState::kThirdPerson || playerCamera->currentState->id == RE::CameraState::kMount)
	{
		auto currentState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());

		cameraYawOffset = NormalRelativeAngle(currentState->freeRotation.x);
		cameraPitchOffset = currentState->freeRotation.y;
	} else if (playerCamera->currentState->id != RE::CameraState::kFirstPerson) {
	    return;
	}
	
	RE::NiPoint3 cameraPos = GetCameraPos();

	if (Settings::uCameraHeadtrackingMode == CameraHeadtrackingMode::kDisable && !(cameraYawOffset < TWOTHIRDS_PI && cameraYawOffset > -TWOTHIRDS_PI)) {
		return;
	} else if (Settings::uCameraHeadtrackingMode == CameraHeadtrackingMode::kFaceCamera && !(cameraYawOffset < PI2 && cameraYawOffset > -PI2)) {
		playerCharacter->GetActorRuntimeData().currentProcess->SetHeadtrackTarget(playerCharacter, cameraPos);
		return;
	}
	
	float offsetMult = Settings::fCameraHeadtrackingStrength;
	cameraYawOffset *= offsetMult;
	float yaw = NormalRelativeAngle(playerCharacter->data.angle.z + cameraYawOffset - PI2);
	float pitch = NormalRelativeAngle(playerCharacter->data.angle.x - cameraPitchOffset);
	pitch *= offsetMult;
	RE::NiPoint3 direction = -RotationToDirection(yaw, pitch);
	direction.x *= -1.f;

	RE::NiPoint3 playerHeadPos = playerCharacter->GetLookingAtLocation();

	auto targetPos = playerHeadPos + direction * 500.f;
	playerCharacter->GetActorRuntimeData().currentProcess->SetHeadtrackTarget(playerCharacter, targetPos);
}

void DirectionalMovementHandler::SetCurrentHorseAimAngle(float a_angle)
{
	_horseAimAngle = NormalRelativeAngle(a_angle);
	float absoluteAimAngle = NormalAbsoluteAngle(a_angle);
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter) {
		playerCharacter->SetGraphVariableFloat("TDM_HorseAimTurn_Angle", _horseAimAngle);
		playerCharacter->SetGraphVariableFloat("TDM_HorseAimTurn_Angle_Absolute", absoluteAimAngle);
	}
}

void DirectionalMovementHandler::UpdateHorseAimDirection()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (_currentHorseAimDirection == Direction::kForward) {
		if (_horseAimAngle > 3 * PI8 || _horseAimAngle < 3 * -PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Direction::kRight:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_ForwardToRight");
				logger::debug("HorseAimTurn - Forward To Right");
				return;
			case Direction::kBack:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_ForwardToBack");
				logger::debug("HorseAimTurn - Forward To Back");
				return;
			case Direction::kLeft:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_ForwardToLeft");
				logger::debug("HorseAimTurn - Forward To Left");
				return;
			}
		}
	} else if (_currentHorseAimDirection == Direction::kRight) {
		if (_horseAimAngle > 7 * PI8 || _horseAimAngle < PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Direction::kForward:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_RightToForward");
				logger::debug("HorseAimTurn - Right To Forward");
				return;
			case Direction::kBack:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_RightToBack");
				logger::debug("HorseAimTurn - Right To Back");
				return;
			case Direction::kLeft:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_RightToLeft");
				logger::debug("HorseAimTurn - Right To Left");
				return;
			}
		}
	} else if (_currentHorseAimDirection == Direction::kBack) {
		if (_horseAimAngle > 5 * -PI8 || _horseAimAngle < 5 * PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Direction::kForward:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_BackToForward");
				logger::debug("HorseAimTurn - Back To Forward");
				return;
			case Direction::kRight:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_BackToRight");
				logger::debug("HorseAimTurn - Back To Right");
				return;
			case Direction::kLeft:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_BackToLeft");
				logger::debug("HorseAimTurn - Back To Left");
				return;
			}
		}
	} else if (_currentHorseAimDirection == Direction::kLeft) {
		if (_horseAimAngle > -PI8 || _horseAimAngle < 7 * -PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Direction::kForward:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_LeftToForward");
				logger::debug("HorseAimTurn - Left To Forward");
				return;
			case Direction::kRight:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_LeftToRight");
				logger::debug("HorseAimTurn - Left To Right");
				return;
			case Direction::kBack:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_LeftToBack");
				logger::debug("HorseAimTurn - Left To Back");
				return;
			}
		}
	}
}

void DirectionalMovementHandler::SetNewHorseAimDirection(float a_angle)
{
	if (a_angle >= -PI4 && a_angle < PI4) {
		_currentHorseAimDirection = Direction::kForward;
	} else if (a_angle >= PI4 && a_angle < 3 * PI4) {
		_currentHorseAimDirection = Direction::kRight;
	} else if (a_angle >= 3 * PI4 || a_angle < 3 * -PI4) {
		_currentHorseAimDirection = Direction::kBack;
	} else if (a_angle >= 3 * -PI4 && a_angle < -PI4) {
		_currentHorseAimDirection = Direction::kLeft;
	}
}

void DirectionalMovementHandler::SetLastInputDirection(RE::NiPoint2& a_inputDirection)
{
	static constexpr RE::NiPoint2 zeroVector{ 0.f, 0.f };

	if (a_inputDirection == zeroVector && _lastInputs.size() > 0) {
		_lastInputs.pop_back();
	} else {
		_lastInputs.push_front(a_inputDirection);
		if (_lastInputs.size() > _inputBufferSize) {
			_lastInputs.pop_back();
		}
	}
}

bool DirectionalMovementHandler::DetectInputAnalogStickBounce() const
{
	if (_lastInputs.size() < 2) {
		return false;
	}

	auto it = _lastInputs.begin();

	do {
		auto& input = *it;
		it++;
		auto& nextInput = *it;
		float dot = input.Dot(nextInput);
		if (CheckInputDot(dot)) {
			logger::debug("{} < {}", dot, _analogBounceDotThreshold);
			return true;
		}
	} while (it != _lastInputs.end() - 1);

	return false;
}

RE::NiPoint3 DirectionalMovementHandler::GetCameraRotation()
{
	RE::PlayerCamera* playerCamera = RE::PlayerCamera::GetSingleton();
	RE::PlayerCharacter* player = RE::PlayerCharacter::GetSingleton();
	RE::NiPoint3 ret;

	if (playerCamera->currentState->id == RE::CameraStates::kFirstPerson)
	{
		const auto firstPersonState = static_cast<RE::FirstPersonState*>(playerCamera->cameraStates[RE::CameraState::kFirstPerson].get());
		RE::NiPoint3 angle;
		firstPersonState->firstPersonCameraObj->world.rotate.ToEulerAnglesXYZ(angle);
		ret.x = player->data.angle.x - angle.x;
		ret.y = angle.y;
		ret.z = player->data.angle.z; //NormalAbsoluteAngle(-angle.z);
	} else if (playerCamera->currentState->id == RE::CameraStates::kThirdPerson || playerCamera->currentState->id == RE::CameraStates::kMount) {
		const auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
		ret.x = player->data.angle.x + thirdPersonState->freeRotation.y;
		ret.y = 0.f;
		ret.z = player->data.angle.z + thirdPersonState->freeRotation.x;
	} else {
		ret = player->data.angle;
	}

	return ret;
}

// probably bad math ahead
void DirectionalMovementHandler::LookAtTarget(RE::ActorHandle a_target)
{
	if (_bIsAiming) {
		return;
	}

	if (!_currentTargetPoint && !a_target) {
		return;
	}

	RE::NiPoint3 targetPos = _currentTargetPoint ? _currentTargetPoint->world.translate : _target.get()->GetLookingAtLocation();

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	RE::ThirdPersonState* thirdPersonState = nullptr;

	bool bIsHorseCamera = playerCamera->currentState->id == RE::CameraState::kMount;

	if (playerCamera && playerCamera->currentState && (playerCamera->currentState->id == RE::CameraState::kThirdPerson || bIsHorseCamera)) {
		thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
	}

	if (!playerCharacter || !thirdPersonState) {
		return;
	}

	RE::NiPoint3 playerPos;
	if (!GetTorsoPos(playerCharacter, playerPos)) {
		return;
	}

	float currentCharacterYaw = playerCharacter->data.angle.z;
	float currentCharacterPitch = playerCharacter->data.angle.x;
	float currentCameraYawOffset = NormalAbsoluteAngle(thirdPersonState->freeRotation.x);

	RE::NiPoint3 cameraPos = GetCameraPos();

	//RE::NiPoint3 midPoint = (playerPos + targetPos) / 2;

	float distanceToTarget = playerPos.GetDistance(targetPos);
	float zOffset = distanceToTarget * Settings::fTargetLockPitchOffsetStrength;

	if (bIsHorseCamera) {
		zOffset *= -1.f;
	}

	RE::NiPoint3 offsetTargetPos = targetPos;
	offsetTargetPos.z -= zOffset;
	//offsetTargetPos = midPoint;

	RE::NiPoint3 playerToTarget = RE::NiPoint3(targetPos.x - playerPos.x, targetPos.y - playerPos.y, targetPos.z - playerPos.z);
	RE::NiPoint3 playerDirectionToTarget = playerToTarget;
	playerDirectionToTarget.Unitize();
	RE::NiPoint3 cameraToTarget = RE::NiPoint3(offsetTargetPos.x - cameraPos.x, offsetTargetPos.y - cameraPos.y, offsetTargetPos.z - cameraPos.z);
	RE::NiPoint3 cameraDirectionToTarget = cameraToTarget;
	cameraDirectionToTarget.Unitize();
	RE::NiPoint3 cameraToPlayer = RE::NiPoint3(playerPos.x - cameraPos.x, playerPos.y - cameraPos.y, playerPos.z - cameraPos.z);

	RE::NiPoint3 projected = Project(cameraToPlayer, cameraToTarget);
	RE::NiPoint3 projectedPos = RE::NiPoint3(projected.x + cameraPos.x, projected.y + cameraPos.y, projected.z + cameraPos.z);
	RE::NiPoint3 projectedDirectionToTarget = RE::NiPoint3(targetPos.x - projectedPos.x, targetPos.y - projectedPos.y, targetPos.z - projectedPos.z);
	projectedDirectionToTarget.Unitize();

	// yaw
	RE::NiPoint2 forwardVector(0.f, 1.f);
	RE::NiPoint2 currentCameraDirection = Vec2Rotate(forwardVector, currentCharacterYaw + currentCameraYawOffset);

	RE::NiPoint2 projectedDirectionToTargetXY(-projectedDirectionToTarget.x, projectedDirectionToTarget.y);

	bool bIsBehind = projectedDirectionToTargetXY.Dot(currentCameraDirection) < 0;

	auto reversedCameraDirection = currentCameraDirection * -1.f;
	float angleDelta = bIsBehind ? GetAngle(reversedCameraDirection, projectedDirectionToTargetXY) : GetAngle(currentCameraDirection, projectedDirectionToTargetXY);
	angleDelta = NormalRelativeAngle(angleDelta);

	const float realTimeDeltaTime = GetRealTimeDeltaTime();

	float desiredFreeCameraRotation = currentCameraYawOffset + angleDelta;
	thirdPersonState->freeRotation.x = InterpAngleTo(currentCameraYawOffset, desiredFreeCameraRotation, realTimeDeltaTime, Settings::fTargetLockYawAdjustSpeed);

	if (bIsBehind)
	{
		return; // don't adjust pitch
	}

	// pitch
	RE::NiPoint3 playerAngle = ToOrientationRotation(playerDirectionToTarget);
	RE::NiPoint3 cameraAngle = ToOrientationRotation(cameraDirectionToTarget);
	_desiredPlayerPitch = -playerAngle.x;
	cameraAngle.x *= ((PI - fabs(cameraAngle.x)) / PI);
	float desiredCameraAngle = _desiredPlayerPitch + cameraAngle.x;

	playerCharacter->data.angle.x = _desiredPlayerPitch;															// player pitch
	
	if (bIsHorseCamera) {
		auto horseCameraState = static_cast<RE::HorseCameraState*>(thirdPersonState);
		if (auto horseRefPtr = horseCameraState->horseRefHandle.get()) {
			auto horse = horseRefPtr->As<RE::Actor>();
			if (horse) {
				horse->data.angle.x = _desiredPlayerPitch;
			}
		}
	}

	float cameraPitchOffset = _desiredPlayerPitch - currentCharacterPitch;

	if (!bIsHorseCamera) {
		thirdPersonState->freeRotation.y += cameraPitchOffset;
		thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, desiredCameraAngle, realTimeDeltaTime, Settings::fTargetLockPitchAdjustSpeed);
	} else {
		thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, -desiredCameraAngle, realTimeDeltaTime, Settings::fTargetLockPitchAdjustSpeed);
	}
}

void DirectionalMovementHandler::UpdateAIProcessRotationSpeed(RE::Actor* a_actor)
{
	if (a_actor) {
		auto currentProcess = a_actor->GetActorRuntimeData().currentProcess;
		if (currentProcess && currentProcess->middleHigh) {
			currentProcess->middleHigh->rotationSpeed.z = _desiredAIProcessRotationSpeed;
		}
	}
	SetDesiredAIProcessRotationSpeed(0.f);
}

void DirectionalMovementHandler::Initialize()
{
	if (APIs::TrueHUD) {
		if (APIs::TrueHUD->RequestTargetControl(SKSE::GetPluginHandle()) != TRUEHUD_API::APIResult::AlreadyTaken) {
			_bControlsTrueHUDTarget = true;
		}
		APIs::TrueHUD->LoadCustomWidgets(SKSE::GetPluginHandle(), "TrueDirectionalMovement/TDM_Widgets.swf"sv, [](TRUEHUD_API::APIResult a_apiResult) {
			if (a_apiResult == TRUEHUD_API::APIResult::OK) {
				APIs::TrueHUD->RegisterNewWidgetType(SKSE::GetPluginHandle(), 'LOCK');
			}
		});
	}
}

void DirectionalMovementHandler::OnPreLoadGame()
{
	ResetControls();
	ResetDesiredAngle();
	ToggleTargetLock(false);
	_DF_bIsDodging = false;
	_DF_bUnlockRotation = false;
	_DF_bUnlockRotationFull = false;
	_bIsDodging_Legacy = false;
	_bJustDodged_Legacy = false;
	_attackState = AttackState::kNone;
	_target = RE::ActorHandle();
	_softTarget = RE::ActorHandle();
	_dialogueSpeaker = RE::ObjectRefHandle();
	_playerIsNPC = false;
	_papyrusDisableDirectionalMovement.clear();
	_papyrusDisableHeadtracking.clear();
}

void DirectionalMovementHandler::OnSettingsUpdated()
{
	if (!Settings::bHeadtracking) {
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (playerCharacter) {
			playerCharacter->AsActorState()->actorState2.headTracking = false;
			playerCharacter->SetGraphVariableBool("IsNPC", false);
		}
	}
	if (auto widget = _targetLockReticle.lock()) {
		if (widget->_object.IsDisplayObject()) {
			widget->Initialize();
		}
	}
	if (!Settings::bOverrideAcrobatics && _defaultAcrobatics != -1.f) {
		auto playerController = RE::PlayerCharacter::GetSingleton()->GetCharController();
		if (playerController) {
			playerController->acrobatics = _defaultAcrobatics;
			_defaultAcrobatics = -1.f;
		}
	}
}

// From SmoothCam
namespace ICSignatures
{
	constexpr const DWORD SizeOfImage = 0x00054000;
	constexpr const DWORD Signature = 0x00004550;
	constexpr const DWORD AddressOfEntryPoint = 0x0001b0a4;
	constexpr const DWORD TimeDateStamp = 0x5d3e15f0;
	constexpr const DWORD FileVersion[4] = { 1, 0, 0, 4 };
};

void DirectionalMovementHandler::InitCameraModsCompatibility()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		_IFPV_IsFirstPerson = dataHandler->LookupForm<RE::TESGlobal>(0x801, "IFPVDetector.esl");
	}

	if (GetModuleHandle("AlternateConversationCamera.dll")) {
		_bACCInstalled = true;
	}

	if (GetModuleHandle("ImprovedCameraSE.dll")) {
		_bICInstalled = true;
	}

	// From SmoothCam - Improved Camera compatibility
	auto hMod = GetModuleHandle("ImprovedCamera.dll");
	if (hMod) {
		MODULEINFO mi;
		GetModuleInformation(GetCurrentProcess(), hMod, &mi, sizeof(mi));
		if (mi.SizeOfImage != ICSignatures::SizeOfImage)
		{
			return;
		}

		const auto ntHeader = ImageNtHeader(hMod);
		if (ntHeader->Signature != ICSignatures::Signature || 
			ntHeader->OptionalHeader.AddressOfEntryPoint != ICSignatures::AddressOfEntryPoint || 
			ntHeader->FileHeader.TimeDateStamp != ICSignatures::TimeDateStamp)
		{
			return;
		}

		DWORD dwHandle;
		auto sz = GetFileVersionInfoSize("ImprovedCamera.dll", &dwHandle);
		if (sz != 0) {
			LPSTR verData = (LPSTR)malloc(sizeof(char) * sz);

			if (GetFileVersionInfo("ImprovedCamera.dll", dwHandle, sz, verData)) {
				LPBYTE lpBuffer = NULL;
				UINT size = 0;
				if (VerQueryValue(verData, "\\", reinterpret_cast<void**>(&lpBuffer), &size) && size) {
					VS_FIXEDFILEINFO* verInfo = reinterpret_cast<VS_FIXEDFILEINFO*>(lpBuffer);
					if (verInfo->dwSignature == 0xfeef04bd) {
						auto v0 = (verInfo->dwFileVersionMS >> 16) & 0xffff;
						auto v1 = (verInfo->dwFileVersionMS >> 0) & 0xffff;
						auto v2 = (verInfo->dwFileVersionLS >> 16) & 0xffff;
						auto v3 = (verInfo->dwFileVersionLS >> 0) & 0xffff;

						// Now check for our match
						if (v0 == ICSignatures::FileVersion[0] && v1 == ICSignatures::FileVersion[1] &&
							v2 == ICSignatures::FileVersion[2] && v3 == ICSignatures::FileVersion[3])
						{
							_ImprovedCamera_IsFirstPerson = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(hMod) + 0x4d510);
						}		
					}
				}
			}

			free(verData);
		}
	}
}

bool DirectionalMovementHandler::IsBehaviorPatchInstalled(RE::TESObjectREFR* a_ref)
{
	if (!a_ref)
	{
		return false;
	}

	bool bOut;
	return a_ref->GetGraphVariableBool("tdmHeadtrackingSKSE", bOut);
}

bool DirectionalMovementHandler::IsMountedArcheryPatchInstalled(RE::TESObjectREFR* a_ref)
{
	if (!a_ref) {
		return false;
	}

	bool bOut;
	return a_ref->GetGraphVariableBool("360HorseGen", bOut);
}

void DirectionalMovementHandler::UpdatePlayerPitch()
{
	if (_bUpdatePlayerPitch)
	{
		_bUpdatePlayerPitch = false;
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
		if (playerCharacter && thirdPersonState)
		{
			float pitchDelta = playerCharacter->data.angle.x - _desiredPlayerPitch;
			playerCharacter->data.angle.x -= pitchDelta;
			thirdPersonState->freeRotation.y -= pitchDelta;
		}
	}
}

void DirectionalMovementHandler::SetYawControl(bool a_enable, float a_yawRotationSpeedMultiplier /*= 0*/)
{
	_bYawControlledByPlugin = a_enable;
	_controlledYawRotationSpeedMultiplier = _controlledYawRotationSpeedMultiplier < 0.f ? 0.f : a_yawRotationSpeedMultiplier;
	if (a_enable) {
		ResetDesiredAngle();
	}
}

void DirectionalMovementHandler::PapyrusDisableDirectionalMovement(std::string_view a_modName, bool a_bDisable)
{
	if (a_bDisable) {
		_papyrusDisableDirectionalMovement.emplace(a_modName.data());
	} else {
		_papyrusDisableDirectionalMovement.erase(a_modName.data());
	}
}

void DirectionalMovementHandler::PapyrusDisableHeadtracking(std::string_view a_modName, bool a_bDisable)
{
	if (a_bDisable) {
		_papyrusDisableHeadtracking.emplace(a_modName.data());
	} else {
		_papyrusDisableHeadtracking.erase(a_modName.data());
	}
}

DirectionalMovementHandler::DirectionalMovementHandler() :
	_lock()
{}
