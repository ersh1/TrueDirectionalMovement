#include "DirectionalMovementHandler.h"
#include "Settings.h"
#include "Events.h"
#include "Offsets.h"
#include "Utils.h"

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

void DirectionalMovementHandler::ResetControls()
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	directionalMovementHandler->_bHasMovementInput = false;
	directionalMovementHandler->_pressedDirections = Directions::kInvalid;
}

void DirectionalMovementHandler::Update()
{
	if (RE::UI::GetSingleton()->GameIsPaused())
	{
		return;
	}

	ProgressTimers();

	UpdateTargetLock();

	UpdateTweeningState();

	UpdateFacingState();

	if (!Settings::bFaceCrosshairInstantly)
	{
		UpdateFacingCrosshair();
	}

	UpdateDirectionalMovement();

	UpdateDodgingState();

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
							if (playerCharacter->currentProcess && playerCharacter->currentProcess->high) {
								playerCharacter->currentProcess->high->SetHeadtrackTarget(3, actorSpeaker);
								RefreshDialogueHeadtrackTimer();
							}
						}
					}
				}
			}
		}

		UpdateSwimmingPitchOffset();

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
				playerCharacter->actorState2.headTracking = false;
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
			thirdPersonState->freeRotation.x = InterpAngleTo(thirdPersonState->freeRotation.x, desiredRotationX, *g_deltaTimeRealTime, 10.f);
			thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, desiredRotationY, *g_deltaTimeRealTime, 10.f);
			cameraTarget->data.angle.x = InterpAngleTo(cameraTarget->data.angle.x, desiredTargetPitch, *g_deltaTimeRealTime, 10.f);
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
}

void DirectionalMovementHandler::UpdateDirectionalMovement()
{
	bool bIsAIDriven = false;
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter) {
		bIsAIDriven = playerCharacter->movementController && !playerCharacter->movementController->unk1C5;
	}

	bool bFreeCamera = GetFreeCameraEnabled();

	RE::TESCameraState* currentCameraState = RE::PlayerCamera::GetSingleton()->currentState.get();
	if (bFreeCamera && !_bForceDisableDirectionalMovement && currentCameraState && !bIsAIDriven &&
		(!_bShouldFaceCrosshair || _bCurrentlyTurningToCrosshair)  &&
		((currentCameraState->id == RE::CameraStates::kThirdPerson && !IFPV_IsFirstPerson() && !ImprovedCamera_IsFirstPerson()) ||
			(currentCameraState->id == RE::CameraStates::kTween && _cameraStateBeforeTween != RE::CameraStates::kFirstPerson) ||
			currentCameraState->id == RE::CameraState::kBleedout) &&
		(Settings::uDialogueMode != DialogueMode::kDisable || !RE::MenuTopicManager::GetSingleton()->speaker)) {
		_bDirectionalMovement = true;
		if (Settings::glob_directionalMovement) {
			Settings::glob_directionalMovement->value = 1;
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

	OverrideControllerBufferDepth(_bDirectionalMovement);
}

void DirectionalMovementHandler::UpdateFacingState()
{
	using Delivery = RE::MagicSystem::Delivery;

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (_faceCrosshairTimer > 0.f) {
		_bShouldFaceCrosshair = true;
		_bShouldFaceTarget = true;
	}

	if (!playerCharacter) {
		return;
	}

	if (Settings::bFaceCrosshairDuringAutoMove && RE::PlayerControls::GetSingleton()->data.autoMove) {
		_bShouldFaceCrosshair = true;
		_bShouldFaceTarget = true;
		return;
	}

	// workaround for an issue with AGO, I don't like this but it works
	static RE::ATTACK_STATE_ENUM currentAttackState;
	auto previousState = currentAttackState;
	RE::ATTACK_STATE_ENUM playerAttackState = playerCharacter->GetAttackState();
	if (playerAttackState != RE::ATTACK_STATE_ENUM::kBowAttached) {
		currentAttackState = playerAttackState;
	}

	if (Settings::bFaceCrosshairWhileAttacking && playerAttackState > RE::ATTACK_STATE_ENUM::kNone && !HasTargetLocked()) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = _faceCrosshairDuration;
		_bShouldFaceTarget = true;
		return;
	}

	if (Settings::bFaceCrosshairWhileShouting && playerCharacter->currentProcess && playerCharacter->currentProcess->high && playerCharacter->currentProcess->high->currentShout) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = _faceCrosshairDuration;
		_bShouldFaceTarget = true;
		return;
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
			} else if (Settings::bFaceCrosshairWhileBlocking && leftSpell->avEffectSetting && leftSpell->avEffectSetting->HasKeyword(Settings::kywd_magicWard)) {
				_bShouldFaceCrosshair = true;
				_faceCrosshairTimer = _faceCrosshairDuration;
				_bShouldFaceTarget = true;
				return;
			}
		}
	}

	SetIsAiming(false);

	if (_faceCrosshairTimer <= 0.f) {
		_bShouldFaceCrosshair = false;
		_bShouldFaceTarget = false;
	}
}

void DirectionalMovementHandler::UpdateFacingCrosshair()
{
	if (_bShouldFaceCrosshair)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto playerCamera = RE::PlayerCamera::GetSingleton();
		if (playerCharacter && playerCamera && playerCamera->currentState && playerCamera->currentState->id == RE::CameraStates::kThirdPerson)
		{
			auto thirdPersonState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
			if (fabs(thirdPersonState->freeRotation.x) > FLT_EPSILON) {
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

	bool bWasDodging = _bIsDodging;
	playerCharacter->GetGraphVariableBool("TDM_Dodge", _bIsDodging);

	_bJustDodged = !bWasDodging && _bIsDodging;

	if (_bJustDodged && !playerCharacter->IsAnimationDriven())
	{
		_faceCrosshairTimer = 0.f;
	}
}

void DirectionalMovementHandler::UpdateSwimmingPitchOffset()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter && playerCharacter->IsSwimming()) {
		_currentSwimmingPitchOffset = InterpTo(_currentSwimmingPitchOffset, _desiredSwimmingPitchOffset, *g_deltaTime, Settings::fSwimmingPitchSpeed);
	}
}

void DirectionalMovementHandler::ProgressTimers()
{
	if (_dialogueHeadtrackTimer > 0.f) {
		_dialogueHeadtrackTimer -= *g_deltaTime;
	}
	if (_lastTargetSwitchTimer > 0.f) {
		_lastTargetSwitchTimer -= *g_deltaTimeRealTime;
	}
	if (_lastLOSTimer > 0.f) {
		_lastLOSTimer -= *g_deltaTime;
	}
	if (_faceCrosshairTimer > 0.f) {
		_faceCrosshairTimer -= *g_deltaTime;
	}
	if (_cameraHeadtrackTimer > 0.f) {
		_cameraHeadtrackTimer -= *g_deltaTime;
	}
	if (_cameraRotationDelayTimer > 0.f) {
		_cameraRotationDelayTimer -= *g_deltaTimeRealTime;
	}
	if (_tutorialHintTimer > 0.f) {
		_tutorialHintTimer -= *g_deltaTimeRealTime;
	}
}

void DirectionalMovementHandler::UpdateLeaning()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter)
	{
		return;
	}

	auto characterController = playerCharacter->GetCharController();
	if (!characterController) {
		return;
	}

	float quad[4];
	_mm_store_ps(quad, characterController->outVelocity.quad);
	RE::NiPoint3 worldVelocity{ quad[0], quad[1], quad[2] };
	RE::NiPoint3 upVector{ 0, 0, 1 };

	auto velocity = worldVelocity;

	/*_velocityBuffer.push_front(worldVelocity);
	if (_velocityBuffer.size() > _velocityBufferSize)
	{
		_velocityBuffer.pop_back();
	}
	
	RE::NiPoint3 avgVelocity;
	for (auto& vel : _velocityBuffer)
	{
		avgVelocity += vel;
	}
	avgVelocity /= (float)_velocityBuffer.size();*/

	// calculate acceleration
	auto worldAcceleration = (velocity - _previousVelocity) / *g_deltaTime;
	_previousVelocity = velocity;

	worldAcceleration = ClampSizeMax(worldAcceleration, worldAcceleration.Dot(velocity) > 0 ? 1500.f : 2048.f);
	auto acceleration = RotateAngleAxis(worldAcceleration, playerCharacter->data.angle.z, upVector);

	// get desired lean
	LeanAmount desiredLean;
	desiredLean.FB = acceleration.y;
	desiredLean.LR = -acceleration.x;

	// interpolate
	_leanAmount.LR = InterpTo(_leanAmount.LR, desiredLean.LR, *g_deltaTime, _leanInterpSpeed);
	_leanAmount.FB = InterpTo(_leanAmount.FB, desiredLean.FB, *g_deltaTime, _leanInterpSpeed);

	// update angles
	characterController->pitchAngle = _leanAmount.FB / 100;
	characterController->rollAngle = _leanAmount.LR / 100;
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
		
		if (!GetFreeCameraEnabled() || (!IsFreeCamera() && !bIsMounted) || IsCameraResetting() || HasTargetLocked() || _cameraRotationDelayTimer > 0.f) {
			_currentAutoCameraRotationSpeed = 0.f;
			return;
		}

		float desiredSpeed = 0.f;

		float speedPct = cameraTarget->GetCharController()->speedPct;
		if (speedPct > 0.f && abs(thirdPersonState->freeRotation.x) > _cameraAutoRotationAngleDeadzone) {
			bool bOnlyDuringSprint = Settings::uAdjustCameraYawDuringMovement == CameraAdjustMode::kDuringSprint;
			bool bIsSprinting = cameraTarget->IsSprinting();
			if (bOnlyDuringSprint && !bIsSprinting) {
				desiredSpeed = 0.f;
			} else {
				desiredSpeed = -sin(thirdPersonState->freeRotation.x) * speedPct * Settings::fCameraAutoAdjustSpeedMult;
			}
		}

		_currentAutoCameraRotationSpeed = InterpTo(_currentAutoCameraRotationSpeed, desiredSpeed, *g_deltaTimeRealTime, 5.f);
		thirdPersonState->freeRotation.x += _currentAutoCameraRotationSpeed * *g_deltaTimeRealTime;
	}
}

void DirectionalMovementHandler::ResetCameraRotationDelay()
{
	_cameraRotationDelayTimer = Settings::fCameraAutoAdjustDelay;
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
		if (g_SmoothCam) {
			auto result = g_SmoothCam->RequestCrosshairControl(SKSE::GetPluginHandle(), true);
			if (result == SmoothCamAPI::APIResult::OK || result == SmoothCamAPI::APIResult::AlreadyGiven) {
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
		if (g_SmoothCam) {
			auto pluginHandle = g_SmoothCam->GetCrosshairOwner();
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
			if (g_SmoothCam) {
				g_SmoothCam->ReleaseCrosshairControl(SKSE::GetPluginHandle());
			}
		}

		_bCrosshairIsHidden = false;
	}
}

bool DirectionalMovementHandler::IsAiming() const
{
	return _bIsAiming;
}

void DirectionalMovementHandler::SetIsAiming(bool a_bIsAiming)
{
	if (_bIsAiming != a_bIsAiming) {
		_bIsAiming = a_bIsAiming;
	}
}

bool DirectionalMovementHandler::ProcessInput(RE::NiPoint2& a_inputDirection, RE::PlayerControlsData* a_playerControlsData)
{
	if (a_playerControlsData->fovSlideMode) {
		return false;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
	if (!playerCharacter || !thirdPersonState) {
		return false;
	}

	// Skip if player is AI driven
	if (playerCharacter->movementController && !playerCharacter->movementController->unk1C5) {
		ResetDesiredAngle();
		return false;
	}

	// Skip if player is animation driven
	if (IsPlayerAnimationDriven()) {
		ResetDesiredAngle();
		return false;
	}

	if (_bShouldFaceCrosshair) {
		if (!_bCurrentlyTurningToCrosshair) {
			ResetDesiredAngle();
		}
		return false;  // let the hook do the rotation
	}

	_bHasMovementInput = true;

	float currentCharacterRot = playerCharacter->data.angle.z;
	float currentCameraRotOffset = thirdPersonState->freeRotation.x;

	RE::NiPoint2 normalizedInputDirection = a_inputDirection;
	float inputLength = normalizedInputDirection.Unitize();

	if (Settings::bThumbstickBounceFix && inputLength < 0.25f && DetectInputAnalogStickBounce()) {
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec.x = 0;
		a_playerControlsData->moveInputVec.y = 0;
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

	if (HasTargetLocked() && _bIsDodging) {
		// don't rotate when dodging in target lock
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec = a_inputDirection;
		return true;
	}

	bool bWantsToSprint = (playerCharacter->unkBDD & RE::PlayerCharacter::FlagBDD::kSprinting) != RE::PlayerCharacter::FlagBDD::kNone;

	if ((HasTargetLocked() && !bWantsToSprint) || _bMagnetismActive || (Settings::uDialogueMode == DialogueMode::kFaceSpeaker && _dialogueSpeaker)) {
		a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
		a_playerControlsData->moveInputVec = cameraRelativeInputDirection;

		return true;
	}

	_desiredAngle = NormalAbsoluteAngle(-GetAngle(characterDirection, cameraRelativeInputDirection));

	float dot = characterDirection.Dot(normalizedWorldRelativeInputDirection);	
	
	bool bPivoting = dot < 0.f;

	if (dot < -0.8f) {
		playerCharacter->NotifyAnimationGraph("TDM_Turn_180");
	}

	bool bShouldStop = Settings::bStopOnDirectionChange && RE::BSTimer::GetCurrentGlobalTimeMult() == 1;

	a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
	a_playerControlsData->moveInputVec.x = 0.f;
	a_playerControlsData->moveInputVec.y = bShouldStop && bPivoting ? 0.01f : inputLength;  // reduce input to almost 0 when trying to move in opposite direction

	return true;
}

void DirectionalMovementHandler::SetDesiredAngleToTarget(RE::PlayerCharacter* a_playerCharacter, RE::ActorHandle a_target)
{
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
	if ((a_playerCharacter->unkBDD & RE::PlayerCharacter::FlagBDD::kSprinting) != RE::PlayerCharacter::FlagBDD::kNone || bIsDodging) {
		return;
	}

	auto target = a_target.get();

	if (!target) {
		return;
	}

	RE::NiPoint2 playerPos;
	playerPos.x = a_playerCharacter->GetPositionX();
	playerPos.y = a_playerCharacter->GetPositionY();
	RE::NiPoint2 targetPos;
	targetPos.x = target->GetPositionX();
	targetPos.y = target->GetPositionY();

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

void DirectionalMovementHandler::UpdateRotation()
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

	bool bInstantRotation = (_bShouldFaceCrosshair && !_bCurrentlyTurningToCrosshair) || (_bJustDodged && !playerCharacter->IsAnimationDriven());

	if (!bInstantRotation) {
		if (IsPlayerAnimationDriven() || _bIsDodging) {
			ResetDesiredAngle();
			return;
		}

		float rotationSpeedMult = PI;
		bool bRelativeSpeed = true;

		if (_bCurrentlyTurningToCrosshair)
		{
			rotationSpeedMult *= Settings::fFaceCrosshairRotationSpeedMultiplier;
		} else if (playerCharacter->IsSwimming()) {
			rotationSpeedMult *= Settings::fSwimmingRotationSpeedMult;
		} else {
			// Get the current movement type
			RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
			playerCharacter->GetAnimationGraphManager(animationGraphManagerPtr);

			RE::BSFixedString string;
			RE::BGSMovementType* movementType = nullptr;

			if (animationGraphManagerPtr) {
				RE::BShkbAnimationGraph* animationGraph = animationGraphManagerPtr->graphs[animationGraphManagerPtr->activeGraph].get();
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

			RE::ATTACK_STATE_ENUM playerAttackState = playerCharacter->GetAttackState();
			bool bIsAttacking = playerAttackState > RE::ATTACK_STATE_ENUM::kNone && playerAttackState < RE::ATTACK_STATE_ENUM::kBowDraw;
			if (playerCharacter->IsInMidair()) {
				rotationSpeedMult *= Settings::fAirRotationSpeedMult;
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
			} else if (playerCharacter->IsSprinting()) {
				rotationSpeedMult *= Settings::fSprintingRotationSpeedMult;
			} else {
				rotationSpeedMult *= Settings::fRunningRotationSpeedMult;
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
			float gtm = RE::BSTimer::GetCurrentGlobalTimeMult();
			rotationSpeedMult /= gtm;
		}

		float maxAngleDelta = rotationSpeedMult * *g_deltaTime;
		if (bRelativeSpeed) {
			maxAngleDelta *= (1.f + abs(angleDelta));
		}

		angleDelta = ClipAngle(angleDelta, -maxAngleDelta, maxAngleDelta);
	}

	float aiProcessRotationSpeed = angleDelta * (1 / *g_deltaTime);
	SetDesiredAIProcessRotationSpeed(aiProcessRotationSpeed);
	
	playerCharacter->SetRotationZ(playerCharacter->data.angle.z + angleDelta);

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

	RE::NiPoint3 targetPos;
	if (!GetTargetPos(_target, targetPos)) {
		return;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter) {
		return;
	}

	RE::NiPoint3 playerPos;
	if (!GetTargetPos(playerCharacter->GetHandle(), playerPos)) {
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

	float desiredCharacterYaw = currentCharacterYaw + angleDelta;

	playerCharacter->SetRotationZ(InterpAngleTo(currentCharacterYaw, desiredCharacterYaw, *g_deltaTimeRealTime, Settings::fTargetLockYawAdjustSpeed));

	// pitch
	RE::NiPoint3 playerAngle = ToOrientationRotation(playerDirectionToTarget);
	float desiredPlayerPitch = -playerAngle.x;

	playerCharacter->SetRotationX(InterpAngleTo(currentCharacterPitch, desiredPlayerPitch, *g_deltaTimeRealTime, Settings::fTargetLockPitchAdjustSpeed));
}

void DirectionalMovementHandler::UpdateTweeningState()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter) {
		auto movementController = playerCharacter->movementController;
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

bool DirectionalMovementHandler::IsFreeCamera() const
{
	return _bDirectionalMovement;
}

bool DirectionalMovementHandler::GetFreeCameraEnabled() const
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter)
	{
		return playerCharacter->GetWeaponState() == RE::WEAPON_STATE::kSheathed ? Settings::bDirectionalMovementSheathed : Settings::bDirectionalMovementDrawn;
	}

	return false;
}

bool DirectionalMovementHandler::HasMovementInput() const
{
	return _bHasMovementInput;
}

bool DirectionalMovementHandler::IsDodging() const
{
	return _bIsDodging;
}

bool DirectionalMovementHandler::IsMagnetismActive() const
{
	return _bMagnetismActive;
}

bool DirectionalMovementHandler::IsPlayerAnimationDriven() const
{
	// workaround for 'IsNPC' issue
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter)
	{
		return playerCharacter->IsAnimationDriven() && !_bCurrentlyTurningToCrosshair && !HasTargetLocked();
	}
	return false;
}

DirectionalMovementHandler::AttackState DirectionalMovementHandler::GetAttackState() const
{
	return _attackState;
}

void DirectionalMovementHandler::SetAttackState(DirectionalMovementHandler::AttackState a_state)
{
	_attackState = a_state;
}

bool DirectionalMovementHandler::IsCameraResetting() const
{
	return _bResetCamera;
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

void DirectionalMovementHandler::ResetDesiredAngle()
{
	_desiredAngle = -1.f;
}

float DirectionalMovementHandler::GetYawDelta() const
{
	return _yawDelta;
}

void DirectionalMovementHandler::ResetYawDelta()
{
	_yawDelta = 0.f;
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

		//RE::ActorHandle actor = FindTarget(Settings::fTargetLockDistance, HasTargetLocked() ? kSort_CharacterDistanceAndCrosshair : kSort_Crosshair);
		RE::ActorHandle actor = FindTarget(Settings::fTargetLockDistance, kSort_Combined);
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
		playerCharacter->SetGraphVariableBool("TDM_TargetLock", true);

		// Remove spell so DAR can detect target lock
		if (Settings::spel_targetLockSpell) {
			playerCharacter->RemoveSpell(Settings::spel_targetLockSpell);
		}

		//ShowCrosshair();

		_lastLOSTimer = -1.f;

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

RE::ActorHandle DirectionalMovementHandler::GetTarget()
{
	//return HasTargetLocked() ? _target : _softTarget;
	return _target;
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
	Locker locker(_lock);
	if (a_override)	{
		if (_defaultControllerBufferDepth == -1.f) {
			_defaultControllerBufferDepth = *g_fControllerBufferDepth;
		}
		*g_fControllerBufferDepth = _freecamControllerBufferDepth;
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
	if (!target || target->IsDisabled() ||
		!target->parentCell || target->parentCell->cellDetached ||
		!target->currentProcess || !target->currentProcess->InHighProcess() ||
		target->IsDead() ||
		(target->IsBleedingOut() && target->IsEssential()) ||
		target->GetPosition().GetDistance(playerCharacter->GetPosition()) > (Settings::fTargetLockDistance * GetTargetLockDistanceRaceSizeMultiplier(target->GetRace()) * _targetLockDistanceHysteresis) ||
		target->GetActorValue(RE::ActorValue::kInvisibility) > 0 ||
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

			bool bFirstLOSCheck = _lastLOSTimer == -1.f;

			bool r8 = false;
			bool bHasLOS = playerCharacter->HasLineOfSight(a_target.get().get(), r8);
			if (bHasLOS || bFirstLOSCheck) {
				_lastLOSTimer = 1.f;
			}

			if (!bFirstLOSCheck) {
				if (_lastLOSTimer <= 0.f) {
					return false;
				}
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

	if (a_actor->IsBleedingOut() && a_actor->IsEssential())
		return false;

	if (a_bCheckDistance && a_actor->GetPosition().GetDistance(playerCharacter->GetPosition()) > Settings::fTargetLockDistance * GetTargetLockDistanceRaceSizeMultiplier(a_actor->GetRace()))
		return false;

	if (a_actor->GetActorValue(RE::ActorValue::kInvisibility) > 0)
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

std::vector<RE::ActorHandle> DirectionalMovementHandler::FindCloseActor(float a_distance, TargetSortOrder a_sortOrder)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	float fovThreshold = playerCamera->worldFOV / 180.f * PI / 2;

	// Increase the FOV threshold a bit when switching targets
	if (a_sortOrder == kSort_ZAxisClock || a_sortOrder == kSort_ZAxisRClock) {
		fovThreshold *= 1.5f;	
	}

	std::vector<RE::ActorHandle> result;

	auto& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
	if (actorHandles.size() == 0)
	{
		return result;
	}

	std::vector<std::pair<float, RE::ActorHandle>> vec;

	RE::NiPoint3 playerPosition = playerCharacter->data.location;
	RE::NiPoint2 forwardVector(0.f, 1.f);	
	RE::NiPoint2 playerDirection = Vec2Rotate(forwardVector, playerCharacter->data.angle.z);
	RE::NiPoint3 cameraPosition = playerCamera->pos;
	RE::NiPoint3 cameraAngle = GetCameraRotation();

	for (auto& actorHandle : actorHandles)
	{
		auto actor = actorHandle.get();
		if (IsActorValidTarget(actor))
		{
			RE::NiPoint3 actorPosition = actor->GetPosition();
			const float dx = actorPosition.x - cameraPosition.x;
			const float dy = actorPosition.y - cameraPosition.y;
			const float dz = actorPosition.z - cameraPosition.z;
			const float dd = sqrtf(dx * dx + dy * dy + dz * dz);

			float distance = a_distance * GetTargetLockDistanceRaceSizeMultiplier(actor->GetRace());

			if (distance <= 0 || dd <= distance)
			{
				float point;
				const float angleZ = NormalRelativeAngle(atan2(dx, dy) - cameraAngle.z);
				const float angleX = NormalRelativeAngle(atan2(-dz, sqrtf(dx * dx + dy * dy)) -cameraAngle.x);

				if (fabs(angleZ) < fovThreshold)
				{
					switch (a_sortOrder)
					{
					case kSort_CameraDistance:
						point = dd;
						break;
					case kSort_CharacterDistanceAndCrosshair:
						point = actorPosition.GetDistance(playerPosition) + sqrtf(angleZ * angleZ + angleX * angleX);
						break;
					case kSort_Crosshair:
						point = sqrtf(angleZ * angleZ + angleX * angleX);
						break;
					case kSort_Combined:
					{
						RE::NiPoint2 directionToTarget = RE::NiPoint2(-(actorPosition.x - playerPosition.x), actorPosition.y - playerPosition.y);
						directionToTarget.Unitize();
						float dot = playerDirection.Dot(directionToTarget);
						point = actorPosition.GetDistance(playerPosition) * (1 - dot) + sqrtf(angleZ * angleZ + angleX * angleX);
						break;
					}						
					case kSort_ZAxisClock:
						point = NormalAbsoluteAngle(atan2(dx, dy) - cameraAngle.z);
						break;
					case kSort_ZAxisRClock:
						point = 2 * PI - NormalAbsoluteAngle(atan2(dx, dy) - cameraAngle.z);
						break;
					default:
						point = 0;
						break;
					}

					if (point >= 0)
					{
						vec.emplace_back(point, actorHandle);
					}
				}
			}
		}
	}

	if (vec.empty())
	{
		return result;
	}

	if (a_sortOrder < kSort_Invalid)
	{
		struct sort_pred
		{
			bool operator()(const std::pair<float, RE::ActorHandle>& l, const std::pair<float, RE::ActorHandle>& r)
			{
				return l.first < r.first;
			}
		};

		std::sort(vec.begin(), vec.end(), sort_pred());
	}

	for (int i = 0; i < vec.size(); i++)
	{
		result.push_back(vec[i].second);
	}

	return result;
}

RE::ActorHandle DirectionalMovementHandler::FindTarget(float a_distance, TargetSortOrder a_sortOrder /*= kSort_Crosshair */)
{
	if (auto crosshairRef = Events::CrosshairRefManager::GetSingleton()->GetCachedRef())
	{
		if (auto crosshairRefPtr = crosshairRef.get()) {
			auto crosshairActor = RE::ActorPtr(crosshairRef.get()->As<RE::Actor>());
			if (crosshairActor && crosshairActor != _target.get() && IsActorValidTarget(crosshairActor)) {
				return crosshairActor->GetHandle();
			}
		}
	}

	auto actors = FindCloseActor(a_distance, a_sortOrder);
	for (auto actor : actors)
	{
		if (actor && actor != _target)
		{
			return actor;
		}
	}

	return RE::ActorHandle();
}

RE::ActorHandle DirectionalMovementHandler::FindNextTarget(float a_distance, bool bRight)
{
	auto actors = FindCloseActor(a_distance, bRight ? kSort_ZAxisClock : kSort_ZAxisRClock);
	for (auto actor : actors) 
	{
		if (actor && actor != _target) 
		{
			return actor;
		}
	}

	return RE::ActorHandle();
}

RE::ActorHandle DirectionalMovementHandler::FindClosestTarget(float a_distance)
{
	auto actors = FindCloseActor(a_distance, kSort_CameraDistance);
	for (auto actor : actors) 
	{
		if (actor && actor != _target) 
		{
			return actor;
		}
	}

	return RE::ActorHandle();
}

bool DirectionalMovementHandler::SetDesiredAngleToMagnetismTarget()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (!playerCharacter || _bCurrentlyTurningToCrosshair) {
		_bMagnetismActive = false;
		return false;
	}

	RE::ATTACK_STATE_ENUM playerAttackState = playerCharacter->GetAttackState();
	bool bIsAttacking = playerAttackState > RE::ATTACK_STATE_ENUM::kNone && playerAttackState < RE::ATTACK_STATE_ENUM::kBowDraw;

	if (!bIsAttacking || _attackState > AttackState::kStart) {
		_bMagnetismActive = false;
		return false;
	}

	auto& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
	if (actorHandles.size() == 0) {
		_bMagnetismActive = false;
		return false;
	}

	if (_bMagnetismActive) {
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

float DirectionalMovementHandler::GetCurrentSwimmingPitchOffset() const
{
	return _currentSwimmingPitchOffset;
}

void DirectionalMovementHandler::SetDesiredSwimmingPitchOffset(float a_value)
{
	_desiredSwimmingPitchOffset = a_value;
}

void DirectionalMovementHandler::SetTarget(RE::ActorHandle a_target)
{
	_target = a_target;

	SetHeadtrackTarget(4, nullptr);

	if (g_trueHUD) {
		g_trueHUD->SetTarget(SKSE::GetPluginHandle(), _target);

		if (Settings::bEnableTargetLockReticle) {
			if (a_target) {
				if (auto widget = _targetLockReticle.lock()) {
					widget->ChangeTarget(a_target);
				} else {
					AddTargetLockReticle(_target);
				}
			} else {
				RemoveTargetLockReticle();
			}
		}
	}
	
	if (Settings::bHeadtracking && !GetForceDisableHeadtracking() && _target) {
		SetHeadtrackTarget(4, a_target.get().get());
	}
}

void DirectionalMovementHandler::SetSoftTarget(RE::ActorHandle a_softTarget)
{
	if (a_softTarget == _softTarget) {
		return;
	}

	_softTarget = a_softTarget;

	SetHeadtrackTarget(3, nullptr);

	if (g_trueHUD) {
		g_trueHUD->SetSoftTarget(SKSE::GetPluginHandle(), _softTarget);
	}

	if (Settings::bHeadtracking && !GetForceDisableHeadtracking() && Settings::bHeadtrackSoftTarget && _softTarget) {
		SetHeadtrackTarget(3, a_softTarget.get().get());
	}
}

void DirectionalMovementHandler::AddTargetLockReticle(RE::ActorHandle a_target)
{
	auto reticleStyle = Settings::uReticleStyle;
	if (reticleStyle == ReticleStyle::kCrosshair && !IsCrosshairVisible()) {
		reticleStyle = ReticleStyle::kCrosshairNoTransform;
	}
	auto widget = std::make_shared<Scaleform::TargetLockReticle>(a_target.native_handle(), a_target, reticleStyle);
	_targetLockReticle = widget;

	g_trueHUD->AddWidget(SKSE::GetPluginHandle(), 'LOCK', 0, "TDM_TargetLockReticle", widget);
}

void DirectionalMovementHandler::ReticleRemoved()
{
	DirectionalMovementHandler::GetSingleton()->_bReticleRemoved = true;
}

void DirectionalMovementHandler::RemoveTargetLockReticle()
{
	g_trueHUD->RemoveWidget(SKSE::GetPluginHandle(), 'LOCK', 0, TRUEHUD_API::WidgetRemovalMode::Normal);
}

void DirectionalMovementHandler::SwitchTarget(Directions a_direction)
{
	if (a_direction == _lastTargetSwitchDirection && _lastTargetSwitchTimer > 0.f)
	{
		return; // too soon
	}

	RE::ActorHandle actor;

	switch (a_direction)
	{
	case Directions::kLeft:
		actor = FindNextTarget(Settings::fTargetLockDistance, false);
		break;
	case Directions::kRight:
		actor = FindNextTarget(Settings::fTargetLockDistance, true);
		break;
	case Directions::kBack:
		actor = FindClosestTarget(Settings::fTargetLockDistance);
		break;
	default:
		break;
	}

	if (actor)
	{
		SetTarget(actor);

		_lastTargetSwitchDirection = a_direction;
		_lastTargetSwitchTimer = 0.25f;
	}
}

void DirectionalMovementHandler::SetHeadtrackTarget(int32_t a_headtrackPriority, RE::TESObjectREFR* a_target)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (a_target) {
		playerCharacter->currentProcess->high->SetHeadtrackTarget(a_headtrackPriority, a_target);
	} else {
		playerCharacter->currentProcess->high->SetHeadtrackTarget(a_headtrackPriority, nullptr);
		playerCharacter->currentProcess->high->SetHeadtrackTarget(0, nullptr);
	}
}

void DirectionalMovementHandler::UpdateCameraHeadtracking()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto playerCamera = RE::PlayerCamera::GetSingleton();

	if (!playerCharacter || !playerCamera || !playerCamera->currentState || !playerCamera->cameraRoot) {
		return;
	}

	RE::ThirdPersonState* currentState = nullptr;

	if (playerCamera->currentState->id == RE::CameraState::kThirdPerson || playerCamera->currentState->id == RE::CameraState::kMount)
	{
		currentState = static_cast<RE::ThirdPersonState*>(playerCamera->currentState.get());
	}

	if (!currentState) {
		return;
	}
	
	RE::NiPoint3 cameraPos = GetCameraPos();

	float cameraYawOffset = NormalRelativeAngle(currentState->freeRotation.x);
	if (Settings::uCameraHeadtrackingMode == CameraHeadtrackingMode::kDisable && !(cameraYawOffset < TWOTHIRDS_PI && cameraYawOffset > -TWOTHIRDS_PI)) {
		return;
	} else if (Settings::uCameraHeadtrackingMode == CameraHeadtrackingMode::kFaceCamera && !(cameraYawOffset < PI2 && cameraYawOffset > -PI2)) {
		playerCharacter->currentProcess->SetHeadtrackTarget(playerCharacter, cameraPos);
		return;
	}

	float cameraPitchOffset = currentState->freeRotation.y;
	float offsetMult = Settings::fCameraHeadtrackingStrength;
	cameraYawOffset *= offsetMult;
	float yaw = NormalRelativeAngle(playerCharacter->data.angle.z + cameraYawOffset - PI2);
	float pitch = NormalRelativeAngle(playerCharacter->data.angle.x - cameraPitchOffset);
	pitch *= offsetMult;
	RE::NiPoint3 direction = -RotationToDirection(yaw, pitch);
	direction.x *= -1.f;

	RE::NiPoint3 playerHeadPos = playerCharacter->GetLookingAtLocation();

	auto targetPos = playerHeadPos + direction * 500.f;
	playerCharacter->currentProcess->SetHeadtrackTarget(playerCharacter, targetPos);
}


void DirectionalMovementHandler::SetPreviousHorseAimAngle(float a_angle)
{
	_previousHorseAimAngle = a_angle;
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

	if (_currentHorseAimDirection == Directions::kForward) {
		if (_horseAimAngle > 3 * PI8 || _horseAimAngle < 3 * -PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Directions::kRight:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_ForwardToRight");
				logger::debug("HorseAimTurn - Forward To Right");
				return;
			case Directions::kBack:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_ForwardToBack");
				logger::debug("HorseAimTurn - Forward To Back");
				return;
			case Directions::kLeft:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_ForwardToLeft");
				logger::debug("HorseAimTurn - Forward To Left");
				return;
			}
		}
	} else if (_currentHorseAimDirection == Directions::kRight) {
		if (_horseAimAngle > 7 * PI8 || _horseAimAngle < PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Directions::kForward:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_RightToForward");
				logger::debug("HorseAimTurn - Right To Forward");
				return;
			case Directions::kBack:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_RightToBack");
				logger::debug("HorseAimTurn - Right To Back");
				return;
			case Directions::kLeft:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_RightToLeft");
				logger::debug("HorseAimTurn - Right To Left");
				return;
			}
		}
	} else if (_currentHorseAimDirection == Directions::kBack) {
		if (_horseAimAngle > 5 * -PI8 || _horseAimAngle < 5 * PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Directions::kForward:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_BackToForward");
				logger::debug("HorseAimTurn - Back To Forward");
				return;
			case Directions::kRight:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_BackToRight");
				logger::debug("HorseAimTurn - Back To Right");
				return;
			case Directions::kLeft:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_BackToLeft");
				logger::debug("HorseAimTurn - Back To Left");
				return;
			}
		}
	} else if (_currentHorseAimDirection == Directions::kLeft) {
		if (_horseAimAngle > -PI8 || _horseAimAngle < 7 * -PI8) {
			SetNewHorseAimDirection(_horseAimAngle);
			switch (_currentHorseAimDirection) {
			case Directions::kForward:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_LeftToForward");
				logger::debug("HorseAimTurn - Left To Forward");
				return;
			case Directions::kRight:
				playerCharacter->NotifyAnimationGraph("TDM_HorseAimTurn_LeftToRight");
				logger::debug("HorseAimTurn - Left To Right");
				return;
			case Directions::kBack:
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
		_currentHorseAimDirection = Directions::kForward;
	} else if (a_angle >= PI4 && a_angle < 3 * PI4) {
		_currentHorseAimDirection = Directions::kRight;
	} else if (a_angle >= 3 * PI4 || a_angle < 3 * -PI4) {
		_currentHorseAimDirection = Directions::kBack;
	} else if (a_angle >= 3 * -PI4 && a_angle < -PI4) {
		_currentHorseAimDirection = Directions::kLeft;
	}
}

float DirectionalMovementHandler::GetCurrentHorseAimAngle() const
{
	return _horseAimAngle;
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

bool DirectionalMovementHandler::CheckInputDot(float a_dot) const
{
	return a_dot < _analogBounceDotThreshold;
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

void DirectionalMovementHandler::SetCameraStateBeforeTween(RE::CameraStates::CameraState a_cameraState)
{
	_cameraStateBeforeTween = a_cameraState;
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

	RE::NiPoint3 targetPos;
	if (!GetTargetPos(a_target, targetPos)) {
		return;
	}

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
	if (!GetTargetPos(playerCharacter->GetHandle(), playerPos)) {
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

	float desiredFreeCameraRotation = currentCameraYawOffset + angleDelta;
	thirdPersonState->freeRotation.x = InterpAngleTo(currentCameraYawOffset, desiredFreeCameraRotation, *g_deltaTimeRealTime, Settings::fTargetLockYawAdjustSpeed);

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
		thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, desiredCameraAngle, *g_deltaTimeRealTime, Settings::fTargetLockPitchAdjustSpeed);
	} else {
		thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, -desiredCameraAngle, *g_deltaTimeRealTime, Settings::fTargetLockPitchAdjustSpeed);
	}
}

bool DirectionalMovementHandler::ShouldFaceTarget() const
{
	return _bShouldFaceTarget;
}

bool DirectionalMovementHandler::ShouldFaceCrosshair() const
{
	return _bShouldFaceCrosshair;
}

bool DirectionalMovementHandler::HasTargetLocked() const
{
	return static_cast<bool>(_target);
}

float DirectionalMovementHandler::GetDialogueHeadtrackTimer() const
{
	return _dialogueHeadtrackTimer;
}

void DirectionalMovementHandler::RefreshDialogueHeadtrackTimer()
{
	_dialogueHeadtrackTimer = Settings::fDialogueHeadtrackingDuration;
}

float DirectionalMovementHandler::GetCameraHeadtrackTimer() const
{
	return _cameraHeadtrackTimer;
}

void DirectionalMovementHandler::RefreshCameraHeadtrackTimer()
{
	_cameraHeadtrackTimer = Settings::fCameraHeadtrackingDuration;
}

void DirectionalMovementHandler::UpdateAIProcessRotationSpeed(RE::Actor* a_actor)
{
	if (a_actor && a_actor->currentProcess && a_actor->currentProcess->middleHigh) {
		a_actor->currentProcess->middleHigh->rotationSpeed.z = _desiredAIProcessRotationSpeed;
	}
	SetDesiredAIProcessRotationSpeed(0.f);
}

void DirectionalMovementHandler::SetDesiredAIProcessRotationSpeed(float a_rotationSpeed)
{
	_desiredAIProcessRotationSpeed = a_rotationSpeed;
}

// for testing purposes
//static float fCurrentSpecial = 100.f;
//float GetCurrentSpecial([[maybe_unused]] RE::Actor* a_actor)
//{
//	while (fCurrentSpecial < -50.f) {
//		fCurrentSpecial += 200.f;
//	}
//	fCurrentSpecial -= *g_deltaTime * 25.f;
//	
//	return fCurrentSpecial;
//}
//
//float GetMaxSpecial([[maybe_unused]] RE::Actor* a_actor)
//{
//	return 100.f;
//}

void DirectionalMovementHandler::Initialize()
{
	if (g_trueHUD) {
		if (g_trueHUD->RequestTargetControl(SKSE::GetPluginHandle()) != TRUEHUD_API::APIResult::AlreadyTaken) {
			_bControlsTrueHUDTarget = true;
		}
		/*if (g_trueHUD->RequestSpecialResourceBarsControl(SKSE::GetPluginHandle()) == TRUEHUD_API::APIResult::OK) {
			g_trueHUD->RegisterSpecialResourceFunctions(SKSE::GetPluginHandle(), GetCurrentSpecial, GetMaxSpecial, true);
		}*/
		g_trueHUD->LoadCustomWidgets(SKSE::GetPluginHandle(), "TrueDirectionalMovement/TDM_Widgets.swf"sv, [](TRUEHUD_API::APIResult a_apiResult) {
			if (a_apiResult == TRUEHUD_API::APIResult::OK) {
				DirectionalMovementHandler::GetSingleton()->g_trueHUD->RegisterNewWidgetType(SKSE::GetPluginHandle(), 'LOCK');
			}
		});
	}
}

void DirectionalMovementHandler::OnPreLoadGame()
{
	ResetControls();
	ResetDesiredAngle();
	ToggleTargetLock(false);
	_bIsDodging = false;
	_attackState = AttackState::kNone;
	_target = RE::ActorHandle();
	_softTarget = RE::ActorHandle();
	_dialogueSpeaker = RE::ObjectRefHandle();
	_playerIsNPC = false;
}

void DirectionalMovementHandler::OnSettingsUpdated()
{
	if (!Settings::bHeadtracking) {
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (playerCharacter) {
			playerCharacter->actorState2.headTracking = false;
			playerCharacter->SetGraphVariableBool("IsNPC", false);
		}
	}
	if (auto widget = _targetLockReticle.lock()) {
		if (widget->_object.IsDisplayObject()) {
			widget->Initialize();
		}
	}
}

// From SmoothCam
#ifdef IS_SKYRIM_AE
namespace ICSignatures
{
	constexpr const DWORD SizeOfImage = 0xFFFFFFFF;
	constexpr const DWORD Signature = 0xFFFFFFFF;
	constexpr const DWORD AddressOfEntryPoint = 0xFFFFFFFF;
	constexpr const DWORD TimeDateStamp = 0xFFFFFFFF;
	constexpr const DWORD FileVersion[4] = { 0, 0, 0, 0 };
};
#else
namespace ICSignatures
{
	constexpr const DWORD SizeOfImage = 0x00054000;
	constexpr const DWORD Signature = 0x00004550;
	constexpr const DWORD AddressOfEntryPoint = 0x0001b0a4;
	constexpr const DWORD TimeDateStamp = 0x5d3e15f0;
	constexpr const DWORD FileVersion[4] = { 1, 0, 0, 4 };
};
#endif

void DirectionalMovementHandler::InitCameraModsCompatibility()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		_IFPV_IsFirstPerson = dataHandler->LookupForm<RE::TESGlobal>(0x801, "IFPVDetector.esl");
	}

	if (GetModuleHandle("AlternateConversationCamera.dll")) {
		_bACCInstalled = true;
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
	return a_ref->GetGraphVariableBool("tdmDummy", bOut);
}

bool DirectionalMovementHandler::GetPlayerIsNPC() const
{
	return _playerIsNPC;
}

void DirectionalMovementHandler::SetPlayerIsNPC(bool a_enable)
{
	_playerIsNPC = a_enable;
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

bool DirectionalMovementHandler::GetForceDisableDirectionalMovement() const
{
	return _bForceDisableDirectionalMovement;
}

bool DirectionalMovementHandler::GetForceDisableHeadtracking() const
{
	return _bForceDisableHeadtracking;
}

void DirectionalMovementHandler::SetForceDisableDirectionalMovement(bool a_disable)
{
	_bForceDisableDirectionalMovement = a_disable;
}

void DirectionalMovementHandler::SetForceDisableHeadtracking(bool a_disable)
{
	_bForceDisableHeadtracking = a_disable;
}

DirectionalMovementHandler::DirectionalMovementHandler() :
	_lock()
{}
