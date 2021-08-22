#include "DirectionalMovementHandler.h"
#include "Events.h"
#include "Offsets.h"
#include "Utils.h"
#include "WidgetHandler.h"
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
	ProgressTimers();

	CheckBosses();
	UpdateTargetLock();

	UpdateTweeningState();

	UpdateFacingState();
	UpdateDirectionalMovement();

	UpdateDodgingState();

	WidgetHandler::GetSingleton()->UpdateVanillaTargetBarState();

	if (_bAiming) {
		ShowCrosshair();
	} else if (_bTargetLock) {
		HideCrosshair();
	}

	if (IsFreeCamera()) {
		if (_target) {
			SetDesiredAngleToTarget(RE::PlayerCharacter::GetSingleton(), _target);
			LookAtTarget(_target);
			if (_directionalMovementGlobal) {
				_directionalMovementGlobal->value = 0;
			}
		}

		if (_dialogueMode == kFaceSpeaker) {
			auto newDialogueSpeaker = RE::MenuTopicManager::GetSingleton()->speaker;

			if (newDialogueSpeaker != _dialogueSpeaker) {
				_dialogueSpeaker = newDialogueSpeaker;
			}
			
			if (_dialogueSpeaker) {
				auto actorSpeaker = _dialogueSpeaker.get()->As<RE::Actor>();
				if (actorSpeaker) {
					RE::ActorHandle actorHandle = actorSpeaker->GetHandle();
					SetDesiredAngleToTarget(RE::PlayerCharacter::GetSingleton(), actorHandle);
					if (_bHeadtracking) {
						auto playerCharacter = RE::PlayerCharacter::GetSingleton();
						if (playerCharacter->currentProcess && playerCharacter->currentProcess->high)
						{
							playerCharacter->currentProcess->high->SetHeadtrackTarget(3, actorSpeaker);
							RefreshDialogueHeadtrackTimer();
						}
					}
				}
			}
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

		if (_bHeadtracking) {
			auto playerCamera = RE::PlayerCamera::GetSingleton();
			if (playerCharacter && playerCamera && playerCamera->currentState && (playerCamera->currentState->id != RE::CameraState::kThirdPerson || IsIFPV() || IsImprovedCamera())){
				// disable headtracking while not in third person
				playerCharacter->actorState2.headTracking = false;
				if (!IsBehaviorPatchInstalled(playerCharacter)){
					playerCharacter->SetGraphVariableBool("IsNPC", false);
				}
			}
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
	if (bFreeCamera && currentCameraState && !bIsAIDriven && !_bShouldFaceCrosshair &&
		((currentCameraState->id == RE::CameraStates::kThirdPerson && !IsIFPV() && !IsImprovedCamera()) ||
			currentCameraState->id == RE::CameraStates::kTween ||
			currentCameraState->id == RE::CameraState::kBleedout) &&
		(_dialogueMode != kDisable || !RE::MenuTopicManager::GetSingleton()->speaker)) {
		_bDirectionalMovement = true;
		if (_directionalMovementGlobal) {
			_directionalMovementGlobal->value = 1;
		}
	} else {
		_bDirectionalMovement = false;
		if (_directionalMovementGlobal) {
			_directionalMovementGlobal->value = 0;
		}
		ResetDesiredAngle();
	}

	OverrideControllerBufferDepth(_bDirectionalMovement);
}

void DirectionalMovementHandler::UpdateFacingState()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (_faceCrosshairTimer > 0.f) {
		_bShouldFaceCrosshair = true;
		_bShouldFaceTarget = true;
	}

	_bAiming = false;

	if (!playerCharacter) {
		return;
	}

	if (_bFaceCrosshairDuringAutoMove && RE::PlayerControls::GetSingleton()->data.autoMove) {
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

	if (_bFaceCrosshairWhileAttacking && playerAttackState > RE::ATTACK_STATE_ENUM::kNone && !HasTargetLocked()) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = faceCrosshairDuration;
		_bShouldFaceTarget = true;
		return;
	}

	if (_bFaceCrosshairWhileBlocking && (playerCharacter->IsBlocking() || playerAttackState == RE::ATTACK_STATE_ENUM::kBash) && !HasTargetLocked()) {
		_bShouldFaceCrosshair = true;
		_faceCrosshairTimer = faceCrosshairDuration;
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
				_bAiming = !HasTargetLocked() || _targetLockArrowAimType == kFreeAim;
				_bShouldFaceCrosshair = _bAiming;
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		} else if (rightWeapon && rightWeapon->IsCrossbow()) {
			if ((playerAttackState >= RE::ATTACK_STATE_ENUM::kBowDrawn && playerAttackState <= RE::ATTACK_STATE_ENUM::kBowReleased)) {
				_bAiming = !HasTargetLocked() || _targetLockArrowAimType == kFreeAim;
				_bShouldFaceCrosshair = _bAiming;
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		} else if (rightWeapon && rightWeapon->IsStaff()) {
			if (iState == 10) {
				_bAiming = !HasTargetLocked() || _targetLockMissileAimType == kFreeAim;
				_bShouldFaceCrosshair = _bAiming;
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		}

		auto rightSpell = rightHand->As<RE::SpellItem>();
		if (rightSpell && (playerCharacter->IsCasting(rightSpell) && rightSpell->GetDelivery() != RE::MagicSystem::Delivery::kSelf)) {
			_bAiming = !HasTargetLocked() || _targetLockMissileAimType == kFreeAim;
			_bShouldFaceCrosshair = _bAiming;
			if (_bShouldFaceCrosshair) {
				_faceCrosshairTimer = faceCrosshairDuration;
			}
			_bShouldFaceTarget = true;
			return;
		}
	}

	auto leftHand = playerCharacter->GetEquippedObject(true);
	if (leftHand) {
		auto leftWeapon = leftHand->As<RE::TESObjectWEAP>();
		if (leftWeapon && leftWeapon->IsStaff()) {
			if (iState == 10) {
				_bAiming = !HasTargetLocked() || _targetLockMissileAimType == kFreeAim;
				_bShouldFaceCrosshair = _bAiming;
				if (_bShouldFaceCrosshair) {
					_faceCrosshairTimer = faceCrosshairDuration;
				}
				_bShouldFaceTarget = true;
				return;
			}
		}

		auto leftSpell = leftHand->As<RE::SpellItem>();
		if (leftSpell && (playerCharacter->IsCasting(leftSpell) && leftSpell->GetDelivery() != RE::MagicSystem::Delivery::kSelf)) {
			_bAiming = !HasTargetLocked() || _targetLockMissileAimType == kFreeAim;
			_bShouldFaceCrosshair = _bAiming;
			if (_bShouldFaceCrosshair) {
				_faceCrosshairTimer = faceCrosshairDuration;
			}
			_bShouldFaceTarget = true;
			return;
		}
	}

	if (_faceCrosshairTimer <= 0.f) {
		_bShouldFaceCrosshair = false;
		_bShouldFaceTarget = false;
	}
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

void DirectionalMovementHandler::ProgressTimers()
{
	if (_dialogueHeadtrackTimer > 0.f) {
		_dialogueHeadtrackTimer -= *g_deltaTime;
	}
	if (_lastTargetSwitchTimer > 0.f) {
		_lastTargetSwitchTimer -= *g_deltaTime;
	}
	if (_lastLOSTimer > 0.f) {	
		_lastLOSTimer -= *g_deltaTime;
	}
	if (_faceCrosshairTimer > 0.f) {
		_faceCrosshairTimer -= *g_deltaTime;
	}
}

void DirectionalMovementHandler::HideCrosshair()
{
	// Hide crosshair if the option is on.
	if (_bTargetLockHideCrosshair) {
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
			auto hud = RE::UI::GetSingleton()->GetMenu("HUD Menu");
			if (hud && hud->uiMovie) {
				const RE::GFxValue bFalse{ false };
				hud->uiMovie->SetVariable("HUDMovieBaseInstance.Crosshair._visible", bFalse);
				_bCrosshairIsHidden = true;
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
			auto hud = RE::UI::GetSingleton()->GetMenu("HUD Menu");
			if (hud && hud->uiMovie) {
				const RE::GFxValue bTrue{ true };
				hud->uiMovie->SetVariable("HUDMovieBaseInstance.Crosshair._visible", bTrue);
			}

			// Release control over crosshair to SmoothCam.
			if (g_SmoothCam) {
				g_SmoothCam->ReleaseCrosshairControl(SKSE::GetPluginHandle());
			}
		}

		_bCrosshairIsHidden = false;
	}
}

bool DirectionalMovementHandler::ProcessInput(RE::NiPoint2& a_inputDirection, RE::PlayerControlsData* a_playerControlsData)
{
	if (a_playerControlsData->fovSlideMode)
	{
		return false;
	}

	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
	if (!playerCharacter || !thirdPersonState) 
	{
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
		ResetDesiredAngle();
		return false;  // let the hook do the rotation
	}

	_bHasMovementInput = true;

	float currentCharacterRot = playerCharacter->data.angle.z;
	float currentCameraRotOffset = thirdPersonState->freeRotation.x;

	RE::NiPoint2 normalizedInputDirection = a_inputDirection;
	float inputLength = normalizedInputDirection.Unitize();

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

	if ((HasTargetLocked() && !bWantsToSprint) || (_dialogueMode == kFaceSpeaker && _dialogueSpeaker)) {
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

	a_playerControlsData->prevMoveVec = a_playerControlsData->moveInputVec;
	a_playerControlsData->moveInputVec.x = 0.f;
	a_playerControlsData->moveInputVec.y = _bStopOnDirectionChange && bPivoting ? 0.01f : inputLength;  // reduce input to almost 0 when trying to move in opposite direction

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
		float currentCharacterRot = a_playerCharacter->data.angle.z;
		float currentCameraRotOffset = thirdPersonState->freeRotation.x;

		_desiredAngle = NormalAbsoluteAngle(currentCharacterRot + currentCameraRotOffset);
	} else {
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

		if (_bHeadtracking && HasTargetLocked() && !_bShouldFaceTarget && !_bHasMovementInput && _attackState == kNone && !RE::PlayerCharacter::GetSingleton()->IsBlocking() && abs(angleDelta) < PI4) {
			return;
		}

		_desiredAngle = NormalAbsoluteAngle(GetAngle(forwardVector, directionToTarget));
	}
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

	bool bInstantRotation = _bShouldFaceCrosshair || (_bJustDodged && !playerCharacter->IsAnimationDriven());

	if (!bInstantRotation) {
		if (IsPlayerAnimationDriven() || _bIsDodging) {
			ResetDesiredAngle();
			return;
		}

		float rotationSpeedMult = PI;
		bool bRelativeSpeed = true;

		// Get the current movement type
		RE::BSTSmartPointer<RE::BSAnimationGraphManager> animationGraphManagerPtr;
		playerCharacter->GetAnimationGraphManager(animationGraphManagerPtr);

		RE::BSFixedString string;
		RE::BGSMovementType* movementType = nullptr;

		if (animationGraphManagerPtr)
		{
			RE::BShkbAnimationGraph* animationGraph = animationGraphManagerPtr->graphs[animationGraphManagerPtr->activeGraph].get();
			BShkbAnimationGraph_sub_140AF0C10(animationGraph, &string);

			const char* stringCstr = string.c_str();
			movementType = sub_140335150(&stringCstr);

			if (movementType) {
				rotationSpeedMult = movementType->movementTypeData.defaultData.rotateWhileMovingRun;
			}
		}
		
		bool bSkipAttackRotationMultipliers = false;

		if (_bDisableAttackRotationMultipliersForTransformations) {
			auto raceFormID = playerCharacter->GetRace()->GetFormID();
			if (raceFormID == werewolfFormID || raceFormID == vampireLordFormID) {
				bSkipAttackRotationMultipliers = true;
			}
		}

		RE::ATTACK_STATE_ENUM playerAttackState = playerCharacter->GetAttackState();
		if (playerCharacter->IsInMidair()) {
			rotationSpeedMult *= _airRotationSpeedMult;
			bRelativeSpeed = false;
		} else if (!bSkipAttackRotationMultipliers && playerAttackState > RE::ATTACK_STATE_ENUM::kNone && playerAttackState < RE::ATTACK_STATE_ENUM::kBowDraw) {
			if (_attackState == kStart) {
				rotationSpeedMult *= _attackStartRotationSpeedMult;
				bRelativeSpeed = false;
			} else if (_attackState == kMid) {
				rotationSpeedMult *= _attackMidRotationSpeedMult;
				bRelativeSpeed = false;
			} else if (_attackState == kEnd) {
				rotationSpeedMult *= _attackEndRotationSpeedMult;
				bRelativeSpeed = false;
			}
		} else if (playerCharacter->IsSprinting()) {
			rotationSpeedMult *= _sprintingRotationSpeedMult;
		} else {
			rotationSpeedMult *= _runningRotationSpeedMult;
		}

		if (rotationSpeedMult <= 0.f) {
			return;
		}

		float maxAngleDelta = rotationSpeedMult * *g_deltaTime;
		if (bRelativeSpeed) {
			maxAngleDelta *= (1.f + abs(angleDelta));
		}
		angleDelta = ClipAngle(angleDelta, -maxAngleDelta, maxAngleDelta);
	}

	SetDesiredAIProcessRotationSpeed(angleDelta * (1 / *g_deltaTime));
	
	playerCharacter->SetRotationZ(playerCharacter->data.angle.z + angleDelta);

	thirdPersonState->freeRotation.x = NormalRelativeAngle(thirdPersonState->freeRotation.x - angleDelta);

	if (_bIsTweening) {
		_yawDelta += angleDelta;
	}

	if (abs(angleDelta) < FLT_EPSILON) {
		ResetDesiredAngle();
	}
}

void DirectionalMovementHandler::UpdateRotationLockedCam()
{
	if (_bAiming) {
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

	playerCharacter->SetRotationZ(InterpAngleTo(currentCharacterYaw, desiredCharacterYaw, *g_deltaTime, _targetLockYawAdjustSpeed));

	// pitch
	RE::NiPoint3 playerAngle = ToOrientationRotation(playerDirectionToTarget);
	float desiredPlayerPitch = -playerAngle.x;

	playerCharacter->SetRotationX(InterpAngleTo(currentCharacterPitch, desiredPlayerPitch, *g_deltaTime, _targetLockPitchAdjustSpeed));
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

bool DirectionalMovementHandler::IsIFPV() const
{
	if (_IFPV_IsFirstPerson) {
		return _IFPV_IsFirstPerson->value;
	}

	return false;
}

bool DirectionalMovementHandler::IsImprovedCamera() const
{
	if (_ImprovedCamera_IsThirdPerson) {
		return *_ImprovedCamera_IsThirdPerson;
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
		return playerCharacter->GetWeaponState() == RE::WEAPON_STATE::kSheathed ? _bDirectionalMovementSheathed : _bDirectionalMovementDrawn;
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

bool DirectionalMovementHandler::IsPlayerAnimationDriven() const
{
	// workaround for 'IsNPC' issue
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (playerCharacter)
	{
		return playerCharacter->IsAnimationDriven() && !HasTargetLocked();
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

void DirectionalMovementHandler::ToggleTargetLock(bool bEnable)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	if (bEnable)
	{
		if (RE::MenuTopicManager::GetSingleton()->speaker) // don't enter lockon during dialogue
		{
			return;
		}

		RE::ActorHandle actor = FindTarget(_targetLockDistance);
		if (actor) 
		{
			SetTarget(actor);	

			// Set graph variable
			playerCharacter->SetGraphVariableBool("TDM_TargetLock", true);

			// Add spell so DAR can detect target lock
			if (_targetLockSpell) {
				playerCharacter->AddSpell(_targetLockSpell);
			}
			
			_bTargetLock = true;

			return;
		}

		// if we're here, this means toggle target lock was called and there was no valid target to be found, so fall through to disable a target lock if it's enabled
	}

	if (!bEnable || _bTargetLock)
	{
		SetTarget(RE::ActorHandle());

		// Set graph variable
		playerCharacter->SetGraphVariableBool("TDM_TargetLock", true);

		// Remove spell so DAR can detect target lock
		if (_targetLockSpell) {
			playerCharacter->RemoveSpell(_targetLockSpell);
		}

		ShowCrosshair();

		_bTargetLock = false;

		_lastLOSTimer = -1.f;

		auto playerCamera = RE::PlayerCamera::GetSingleton();
		// If on a mount, set player and horse pitch to avoid camera snap
		if (playerCharacter->IsOnMount() && playerCamera->currentState && playerCamera->currentState->id == RE::CameraState::kMount) {
			auto horseCameraState = static_cast<RE::HorseCameraState*>(RE::PlayerCamera::GetSingleton()->currentState.get());
			playerCharacter->data.angle.x = -horseCameraState->freeRotation.y;
			//horseCameraState->freeRotation.y = 0;

			if (auto horseRefPtr = horseCameraState->horseRefHandle.get()) {
				auto horse = horseRefPtr->As<RE::Actor>();
				if (horse) {
					horse->data.angle.x = -horseCameraState->freeRotation.y;
				}
			}
		}
	}
}

RE::ActorHandle DirectionalMovementHandler::GetTarget()
{
	return _bTargetLock ? _target : _softTarget;
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
		target->GetPosition().GetDistance(playerCharacter->GetPosition()) > (_targetLockDistance * _targetLockDistanceHysteresis) ||
		target->GetActorValue(RE::ActorValue::kInvisibility) > 0 ||
		//RE::UI::GetSingleton()->IsMenuOpen("Dialogue Menu"))
		RE::MenuTopicManager::GetSingleton()->speaker)
	{
		return false;
	}

	RE::ActorPtr playerMount = nullptr;
	if (playerCharacter->GetMount(playerMount) && playerMount.get() == a_target.get().get())
		return false;

	if (_bTargetLockTestLOS)
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
	if (_bTargetLock)
	{
		/*auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		RE::ActorPtr playerMount = nullptr;

		if (playerCharacter && playerCharacter->GetMount(playerMount))
		{
			ToggleTargetLock(false);
		}*/
		
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
	else if (WidgetHandler::ShowSoftTargetBar() && _softTarget) 
	{
		if (!CheckCurrentTarget(_softTarget)) 
		{
			SetSoftTarget(RE::ActorHandle());
		}
	}
}

void DirectionalMovementHandler::CheckBosses()
{
	if (!WidgetHandler::GetSingleton()->_bShowBossBar)
	{
		return;
	}

	std::unordered_map<RE::ActorHandle, bool> bossesToRemove;
	for (auto& bossHandle : _bossTargets)
	{
		auto boss = bossHandle.get();
		if (!boss || boss->IsDisabled() ||
			!boss->parentCell || boss->parentCell->cellDetached ||
			!boss->currentProcess || !boss->currentProcess->InHighProcess() ||
			!boss->IsInCombat() || !boss->IsHostileToActor(RE::PlayerCharacter::GetSingleton()))
		{
			bossesToRemove.emplace(bossHandle, false);
		}
		else if (boss->IsDead() || (boss->IsBleedingOut() && boss->IsEssential()))
		{
			bossesToRemove.emplace(bossHandle, true);
		}
	}

	for (auto& entry : bossesToRemove)
	{
		RemoveBoss(entry.first, entry.second);
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

	if (a_bCheckDistance && a_actor->GetPosition().GetDistance(playerCharacter->GetPosition()) > _targetLockDistance)
		return false;

	if (a_actor->GetActorValue(RE::ActorValue::kInvisibility) > 0)
		return false;

	/*if (a_actor->IsPlayerTeammate())
		return false;*/

	if (_bTargetLockHostileActorsOnly && !a_actor->IsHostileToActor(playerCharacter))
		return false;

	bool r8 = false;
	bool bHasLOS = playerCharacter->HasLineOfSight(a_actor.get(), r8);

	if (!bHasLOS)
		return false;

	return true;
}

std::vector<RE::ActorHandle> DirectionalMovementHandler::FindCloseActor(float distance, uint32_t sortOrder)
{
	RE::PlayerCamera* playerCamera = RE::PlayerCamera::GetSingleton();
	const float fovThreshold = playerCamera->worldFOV / 180.f * PI / 2;

	std::vector<RE::ActorHandle> result;

	auto& actorHandles = RE::ProcessLists::GetSingleton()->highActorHandles;
	if (actorHandles.size() == 0)
	{
		return result;
	}

	std::vector<std::pair<float, RE::ActorHandle>> vec;

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
			const float dd = sqrt(dx * dx + dy * dy + dz * dz);

			if (distance <= 0 || dd <= distance)
			{
				float point;
				const float angleZ = NormalRelativeAngle(atan2(dx, dy) - cameraAngle.z);
				const float angleX = NormalRelativeAngle(atan2(-dz, sqrt(dx * dx + dy * dy)) -cameraAngle.x);

				if (abs(angleZ) < fovThreshold)
				{
					switch (sortOrder)
					{
					case kSort_Distance:
						point = dd;
						break;
					case kSort_Crosshair:
						point = sqrt(angleZ * angleZ + angleX * angleX);
						break;
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

	if (sortOrder < kSort_Invalid)
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

RE::ActorHandle DirectionalMovementHandler::FindTarget(float a_distance)
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

	auto actors = FindCloseActor(a_distance, kSort_Crosshair);
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
	auto actors = FindCloseActor(a_distance, kSort_Distance);
	for (auto actor : actors) 
	{
		if (actor && actor != _target) 
		{
			return actor;
		}
	}

	return RE::ActorHandle();
}

void DirectionalMovementHandler::SetTarget(RE::ActorHandle a_target)
{
	_target = a_target;

	SetHeadtrackTarget(nullptr);

	auto widgetHandler = WidgetHandler::GetSingleton();
	widgetHandler->SetTarget(_target);
	
	if (_bHeadtracking && _target)
	{
		SetHeadtrackTarget(a_target.get().get());
	}
}

void DirectionalMovementHandler::SetSoftTarget(RE::ActorHandle a_softTarget)
{
	if (a_softTarget != _softTarget)
	{
		auto widgetHandler = WidgetHandler::GetSingleton();
		if (IsActorValidTarget(a_softTarget.get(), true))
		{
			_softTarget = a_softTarget;
			widgetHandler->SetSoftTarget(_softTarget);
		} else {
			auto nullHandle = RE::ActorHandle();
			if (_softTarget != nullHandle)
			{
				_softTarget = nullHandle;
				widgetHandler->SetSoftTarget(_softTarget);
			}
		}
	}
}

void DirectionalMovementHandler::AddBoss(RE::ActorHandle a_boss)
{
	auto result = _bossTargets.emplace(a_boss);
	if (result.second)
	{
		WidgetHandler::GetSingleton()->AddBoss(a_boss);
	}
}

void DirectionalMovementHandler::RemoveBoss(RE::ActorHandle a_boss, bool a_bBossDied)
{
	auto result = _bossTargets.erase(a_boss);
	if (result)
	{
		WidgetHandler::GetSingleton()->RemoveBoss(a_boss, a_bBossDied);
	}
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
		actor = FindNextTarget(_targetLockDistance, false);
		break;
	case Directions::kRight:
		actor = FindNextTarget(_targetLockDistance, true);
		break;
	case Directions::kBack:
		actor = FindClosestTarget(_targetLockDistance);
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

void DirectionalMovementHandler::SetHeadtrackTarget(RE::TESObjectREFR* a_target)
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();

	if (a_target) {
		playerCharacter->currentProcess->high->SetHeadtrackTarget(4, a_target);
	} else {
		playerCharacter->currentProcess->high->SetHeadtrackTarget(4, nullptr);
		playerCharacter->currentProcess->high->SetHeadtrackTarget(0, nullptr);
	}
}

void DirectionalMovementHandler::UpdateCameraHeadtracking()
{
	auto playerCharacter = RE::PlayerCharacter::GetSingleton();
	auto playerCamera = RE::PlayerCamera::GetSingleton();

	if (!playerCharacter || !playerCamera || !playerCamera->currentState) {
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

	float cameraYawOffset = NormalRelativeAngle(currentState->freeRotation.x);
	float cameraPitchOffset = currentState->freeRotation.y;
	if (_bStopCameraHeadtrackingBehindPlayer && !(cameraYawOffset < TWOTHIRDS_PI && cameraYawOffset > -TWOTHIRDS_PI)) {
		return;
	}

	cameraYawOffset *= _cameraHeadtrackingStrength;
	
	float yaw = NormalRelativeAngle(playerCharacter->data.angle.z + cameraYawOffset - PI2);
	float pitch = NormalRelativeAngle(playerCharacter->data.angle.x - cameraPitchOffset);

	pitch *= _cameraHeadtrackingStrength;

	RE::NiPoint3 direction = -RotationToDirection(yaw, pitch);
	direction.x *= -1.f;
	
	auto targetPos = playerCharacter->GetLookingAtLocation() + direction * 500.f;
	playerCharacter->currentProcess->SetHeadtrackTarget(playerCharacter, targetPos);
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
	if (_bAiming) {
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
	float zOffset = distanceToTarget * _targetLockPitchOffsetStrength;

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
	thirdPersonState->freeRotation.x = InterpAngleTo(currentCameraYawOffset, desiredFreeCameraRotation, *g_deltaTime, _targetLockYawAdjustSpeed);

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
		thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, desiredCameraAngle, *g_deltaTime, _targetLockPitchAdjustSpeed);
	} else {
		thirdPersonState->freeRotation.y = InterpAngleTo(thirdPersonState->freeRotation.y, -desiredCameraAngle, *g_deltaTime, _targetLockPitchAdjustSpeed);
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
	return _bTargetLock;
}

bool DirectionalMovementHandler::IsHeadtrackingEnabled() const
{
	return _bHeadtracking;
}

bool DirectionalMovementHandler::IsCameraHeadtrackingEnabled() const
{
	return _bCameraHeadtracking;
}

float DirectionalMovementHandler::GetDialogueHeadtrackTimer() const
{
	return _dialogueHeadtrackTimer;
}

void DirectionalMovementHandler::RefreshDialogueHeadtrackTimer()
{
	_dialogueHeadtrackTimer = _dialogueHeadtrackingDuration;
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

bool DirectionalMovementHandler::GetDirectionalMovementSheathed() const
{
	Locker locker(_lock);
	return _bDirectionalMovementSheathed;
}

void DirectionalMovementHandler::SetDirectionalMovementSheathed(bool a_enable)
{
	Locker locker(_lock);
	_bDirectionalMovementSheathed = a_enable;
}

bool DirectionalMovementHandler::GetDirectionalMovementDrawn() const
{
	Locker locker(_lock);
	return _bDirectionalMovementDrawn;
}

void DirectionalMovementHandler::SetDirectionalMovementDrawn(bool a_enable)
{
	Locker locker(_lock);
	_bDirectionalMovementDrawn = a_enable;
}

DirectionalMovementHandler::DialogueMode DirectionalMovementHandler::GetDialogueMode()
{
	Locker locker(_lock);
	return _dialogueMode;
}

void DirectionalMovementHandler::SetDialogueMode(DialogueMode a_mode)
{
	Locker locker(_lock);
	_dialogueMode = a_mode;
}

bool DirectionalMovementHandler::GetHeadtracking() const
{
	Locker locker(_lock);
	return _bHeadtracking;
}

void DirectionalMovementHandler::SetHeadtracking(bool a_enable)
{
	Locker locker(_lock);
	_bHeadtracking = a_enable;
	if (!a_enable)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (playerCharacter)
		{
			playerCharacter->actorState2.headTracking = false;
			playerCharacter->SetGraphVariableBool("IsNPC", false);
		}
	}
}

float DirectionalMovementHandler::GetDialogueHeadtrackingDuration() const
{
	Locker locker(_lock);
	return _dialogueHeadtrackingDuration;
}

void DirectionalMovementHandler::SetDialogueHeadtrackingDuration(float a_duration)
{
	Locker locker(_lock);
	_dialogueHeadtrackingDuration = a_duration;
}

bool DirectionalMovementHandler::GetCameraHeadtracking() const
{
	Locker locker(_lock);
	return _bCameraHeadtracking;
}

void DirectionalMovementHandler::SetCameraHeadtracking(bool a_enable)
{
	Locker locker(_lock);
	_bCameraHeadtracking = a_enable;
}

float DirectionalMovementHandler::GetCameraHeadtrackingStrength() const
{
	Locker locker(_lock);
	return _cameraHeadtrackingStrength;
}

void DirectionalMovementHandler::SetCameraHeadtrackingStrength(float a_strength)
{
	Locker locker(_lock);
	_cameraHeadtrackingStrength = a_strength;
}

bool DirectionalMovementHandler::GetStopCameraHeadtrackingBehindPlayer() const
{
	Locker locker(_lock);
	return _bStopCameraHeadtrackingBehindPlayer;
}

void DirectionalMovementHandler::SetStopCameraHeadtrackingBehindPlayer(bool a_enable)
{
	Locker locker(_lock);
	_bStopCameraHeadtrackingBehindPlayer = a_enable;
}

bool DirectionalMovementHandler::GetFaceCrosshairWhileAttacking() const
{
	Locker locker(_lock);
	return _bFaceCrosshairWhileAttacking;
}

void DirectionalMovementHandler::SetFaceCrosshairWhileAttacking(bool a_enable)
{
	Locker locker(_lock);
	_bFaceCrosshairWhileAttacking = a_enable;
}

bool DirectionalMovementHandler::GetFaceCrosshairWhileBlocking() const
{
	Locker locker(_lock);
	return _bFaceCrosshairWhileBlocking;
}

void DirectionalMovementHandler::SetFaceCrosshairWhileBlocking(bool a_enable)
{
	Locker locker(_lock);
	_bFaceCrosshairWhileBlocking = a_enable;
}

bool DirectionalMovementHandler::GetFaceCrosshairDuringAutoMove() const
{
	Locker locker(_lock);
	return _bFaceCrosshairDuringAutoMove;
}

void DirectionalMovementHandler::SetFaceCrosshairDuringAutoMove(bool a_enable)
{
	Locker locker(_lock);
	_bFaceCrosshairDuringAutoMove = a_enable;
}

float DirectionalMovementHandler::GetRunningRotationSpeedMult() const
{
	Locker locker(_lock);
	return _runningRotationSpeedMult;
}

void DirectionalMovementHandler::SetRunningRotationSpeedMult(float a_mult)
{
	Locker locker(_lock);
	_runningRotationSpeedMult = a_mult;
}

float DirectionalMovementHandler::GetSprintingRotationSpeedMult() const
{
	Locker locker(_lock);
	return _sprintingRotationSpeedMult;
}

void DirectionalMovementHandler::SetSprintingRotationSpeedMult(float a_mult)
{
	Locker locker(_lock);
	_sprintingRotationSpeedMult = a_mult;
}

float DirectionalMovementHandler::GetAttackStartRotationSpeedMult() const
{
	Locker locker(_lock);
	return _attackStartRotationSpeedMult;
}

void DirectionalMovementHandler::SetAttackStartRotationSpeedMult(float a_mult)
{
	Locker locker(_lock);
	_attackStartRotationSpeedMult = a_mult;
}

float DirectionalMovementHandler::GetAttackMidRotationSpeedMult() const
{
	Locker locker(_lock);
	return _attackMidRotationSpeedMult;
}

void DirectionalMovementHandler::SetAttackMidRotationSpeedMult(float a_mult)
{
	Locker locker(_lock);
	_attackMidRotationSpeedMult = a_mult;
}

float DirectionalMovementHandler::GetAttackEndRotationSpeedMult() const
{
	Locker locker(_lock);
	return _attackEndRotationSpeedMult;
}

void DirectionalMovementHandler::SetAttackEndRotationSpeedMult(float a_mult)
{
	Locker locker(_lock);
	_attackEndRotationSpeedMult = a_mult;
}

float DirectionalMovementHandler::GetAirRotationSpeedMult() const
{
	Locker locker(_lock);
	return _airRotationSpeedMult;
}

void DirectionalMovementHandler::SetAirRotationSpeedMult(float a_mult)
{
	Locker locker(_lock);
	_airRotationSpeedMult = a_mult;
}

bool DirectionalMovementHandler::GetDisableAttackRotationMultipliersForTransformations() const
{
	Locker locker(_lock);
	return _bDisableAttackRotationMultipliersForTransformations;
}

void DirectionalMovementHandler::SetDisableAttackRotationMultipliersForTransformations(bool a_enable)
{
	Locker locker(_lock);
	_bDisableAttackRotationMultipliersForTransformations = a_enable;
}

bool DirectionalMovementHandler::GetStopOnDirectionChange() const
{
	Locker locker(_lock);
	return _bStopOnDirectionChange;
}

void DirectionalMovementHandler::SetStopOnDirectionChange(bool a_enable)
{
	Locker locker(_lock);
	_bStopOnDirectionChange = a_enable;
}

float DirectionalMovementHandler::GetTargetLockDistance() const
{
	Locker locker(_lock);
	return _targetLockDistance;
}

void DirectionalMovementHandler::SetTargetLockDistance(float a_distance)
{
	Locker locker(_lock);
	_targetLockDistance = a_distance;
}

float DirectionalMovementHandler::GetTargetLockPitchAdjustSpeed() const
{
	Locker locker(_lock);
	return _targetLockPitchAdjustSpeed;
}

void DirectionalMovementHandler::SetTargetLockPitchAdjustSpeed(float a_speed)
{
	Locker locker(_lock);
	_targetLockPitchAdjustSpeed = a_speed;
}

float DirectionalMovementHandler::GetTargetLockYawAdjustSpeed() const
{
	Locker locker(_lock);
	return _targetLockYawAdjustSpeed;
}

void DirectionalMovementHandler::SetTargetLockYawAdjustSpeed(float a_speed)
{
	Locker locker(_lock);
	_targetLockYawAdjustSpeed = a_speed;
}

float DirectionalMovementHandler::GetTargetLockPitchOffsetStrength() const
{
	Locker locker(_lock);
	return _targetLockPitchOffsetStrength;
}

void DirectionalMovementHandler::SetTargetLockPitchOffsetStrength(float a_offsetStrength)
{
	Locker locker(_lock);
	_targetLockPitchOffsetStrength = a_offsetStrength;
}

bool DirectionalMovementHandler::GetTargetLockUseMouse() const
{
	Locker locker(_lock);
	return _bTargetLockUseMouse;
}

void DirectionalMovementHandler::SetTargetLockUseMouse(bool a_enable)
{
	Locker locker(_lock);
	_bTargetLockUseMouse = a_enable;
}

bool DirectionalMovementHandler::GetTargetLockUseScrollWheel() const
{
	Locker locker(_lock);
	return _bTargetLockUseScrollWheel;
}

void DirectionalMovementHandler::SetTargetLockUseScrollWheel(bool a_enable)
{
	Locker locker(_lock);
	_bTargetLockUseScrollWheel = a_enable;
}

bool DirectionalMovementHandler::GetTargetLockUseRightThumbstick() const
{
	Locker locker(_lock);
	return _bTargetLockUseRightThumbstick;
}

void DirectionalMovementHandler::SetTargetLockUseRightThumbstick(bool a_enable)
{
	Locker locker(_lock);
	_bTargetLockUseRightThumbstick = a_enable;
}

DirectionalMovementHandler::TargetLockProjectileAimType DirectionalMovementHandler::GetTargetLockArrowAimType()
{
	Locker locker(_lock);
	return _targetLockArrowAimType;
}

void DirectionalMovementHandler::SetTargetLockArrowAimType(TargetLockProjectileAimType a_type)
{
	Locker locker(_lock);
	_targetLockArrowAimType = a_type;
}

DirectionalMovementHandler::TargetLockProjectileAimType DirectionalMovementHandler::GetTargetLockMissileAimType()
{
	Locker locker(_lock);
	return _targetLockMissileAimType;
}

void DirectionalMovementHandler::SetTargetLockMissileAimType(TargetLockProjectileAimType a_type)
{
	Locker locker(_lock);
	_targetLockMissileAimType = a_type;
}

bool DirectionalMovementHandler::GetAutoTargetNextOnDeath() const
{
	Locker locker(_lock);
	return _bAutoTargetNextOnDeath;
}

void DirectionalMovementHandler::SetAutoTargetNextOnDeath(bool a_enable)
{
	Locker locker(_lock);
	_bAutoTargetNextOnDeath = a_enable;
}

bool DirectionalMovementHandler::GetTargetLockTestLOS() const
{
	Locker locker(_lock);
	return _bTargetLockTestLOS;
}

void DirectionalMovementHandler::SetTargetLockTestLOS(bool a_enable)
{
	Locker locker(_lock);
	_bTargetLockTestLOS = a_enable;
}

bool DirectionalMovementHandler::GetTargetLockHostileActorsOnly() const
{
	Locker locker(_lock);
	return _bTargetLockHostileActorsOnly;
}

void DirectionalMovementHandler::SetTargetLockHostileActorsOnly(bool a_enable)
{
	Locker locker(_lock);
	_bTargetLockHostileActorsOnly = a_enable;
}

bool DirectionalMovementHandler::GetTargetLockHideCrosshair() const
{
	Locker locker(_lock);
	return _bTargetLockHideCrosshair;
}

void DirectionalMovementHandler::SetTargetLockHideCrosshair(bool a_hide)
{
	Locker locker(_lock);
	_bTargetLockHideCrosshair = a_hide;
}

bool DirectionalMovementHandler::Save(const SKSE::SerializationInterface* a_intfc, std::uint32_t a_typeCode, std::uint32_t a_version)
{
	Locker locker(_lock);

	if (!a_intfc->OpenRecord(a_typeCode, a_version)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bDirectionalMovementSheathed)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bDirectionalMovementDrawn)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_dialogueMode)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bHeadtracking)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_dialogueHeadtrackingDuration)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bCameraHeadtracking)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_cameraHeadtrackingStrength)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bStopCameraHeadtrackingBehindPlayer)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bFaceCrosshairWhileAttacking)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bFaceCrosshairWhileBlocking)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bFaceCrosshairDuringAutoMove)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_runningRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_sprintingRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_attackStartRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_attackMidRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_attackEndRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_airRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bDisableAttackRotationMultipliersForTransformations)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bStopOnDirectionChange)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLockDistance)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLockPitchAdjustSpeed)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLockYawAdjustSpeed)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLockPitchOffsetStrength)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bTargetLockUseMouse)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bTargetLockUseScrollWheel)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bTargetLockUseRightThumbstick)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLockArrowAimType)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLockMissileAimType)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bAutoTargetNextOnDeath)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bTargetLockTestLOS)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bTargetLockHostileActorsOnly)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bTargetLockHideCrosshair)) {
		return false;
	}

	return true;
}

bool DirectionalMovementHandler::Load(const SKSE::SerializationInterface* a_intfc)
{
	Locker locker(_lock);

	if (!a_intfc->ReadRecordData(_bDirectionalMovementSheathed)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bDirectionalMovementDrawn)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_dialogueMode)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bHeadtracking)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_dialogueHeadtrackingDuration)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bCameraHeadtracking)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_cameraHeadtrackingStrength)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bStopCameraHeadtrackingBehindPlayer)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bFaceCrosshairWhileAttacking)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bFaceCrosshairWhileBlocking)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bFaceCrosshairDuringAutoMove)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_runningRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_sprintingRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_attackStartRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_attackMidRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_attackEndRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_airRotationSpeedMult)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bDisableAttackRotationMultipliersForTransformations)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bStopOnDirectionChange)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLockDistance)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLockPitchAdjustSpeed)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLockYawAdjustSpeed)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLockPitchOffsetStrength)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bTargetLockUseMouse)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bTargetLockUseScrollWheel)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bTargetLockUseRightThumbstick)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLockArrowAimType)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLockMissileAimType)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bAutoTargetNextOnDeath)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bTargetLockTestLOS)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bTargetLockHostileActorsOnly)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bTargetLockHideCrosshair)) {
		return false;
	}

	return true;
}

void DirectionalMovementHandler::Clear()
{
	Locker locker(_lock);
	_bDirectionalMovementSheathed = DF_DIRECTIONALMOVEMENTSHEATHED;
	_bDirectionalMovementDrawn = DF_DIRECTIONALMOVEMENTDRAWN;
	_dialogueMode = DF_DIALOGUEMODE;
	_bHeadtracking = DF_HEADTRACKING;
	_dialogueHeadtrackingDuration = DF_DIALOGUEHEADTRACKINGDURATION;
	_bCameraHeadtracking = DF_CAMERAHEADTRACKING;
	_cameraHeadtrackingStrength = DF_CAMERAHEADTRACKINGSTRENGTH;
	_bStopCameraHeadtrackingBehindPlayer = DF_STOPCAMERAHEADTRACKINGBEHINDPLAYER;
	_bFaceCrosshairWhileAttacking = DF_FACECROSSHAIRWHILEATTACKING;
	_bFaceCrosshairWhileBlocking = DF_FACECROSSHAIRWHILEBLOCKING;
	_bFaceCrosshairDuringAutoMove = DF_FACECROSSHAIRDURINGAUTOMOVE;
	_runningRotationSpeedMult = DF_RUNNINGROTATIONSPEEDMULT;
	_sprintingRotationSpeedMult = DF_SPRINTINGROTATIONSPEEDMULT;
	_attackStartRotationSpeedMult = DF_ATTACKSTARTROTATIONSPEEDMULT;
	_attackMidRotationSpeedMult = DF_ATTACKMIDROTATIONSPEEDMULT;
	_attackEndRotationSpeedMult = DF_ATTACKENDROTATIONSPEEDMULT;
	_airRotationSpeedMult = DF_AIRROTATIONSPEEDMULT;
	_bDisableAttackRotationMultipliersForTransformations = DF_DISABLEATTACKROTATIONMULTIPLIERSFORTRANSFORMATIONS;
	_bStopOnDirectionChange = DF_STOPONDIRECTIONCHANGE;
	_targetLockDistance = DF_TARGETLOCKDISTANCE;
	_targetLockPitchAdjustSpeed = DF_TARGETLOCKPITCHADJUSTSPEED;
	_targetLockYawAdjustSpeed = DF_TARGETLOCKYAWADJUSTSPEED;
	_targetLockPitchOffsetStrength = DF_TARGETLOCKPITCHOFFSETSTRENGTH;
	_bTargetLockUseMouse = DF_TARGETLOCKUSEMOUSE;
	_bTargetLockUseScrollWheel = DF_TARGETLOCKUSESCROLLWHEEL;
	_bTargetLockUseRightThumbstick = DF_TARGETLOCKUSERIGHTTHUMBSTICK;
	_targetLockArrowAimType = DF_TARGETLOCKARROWAIMTYPE;
	_targetLockMissileAimType = DF_TARGETLOCKMISSILEAIMTYPE;
	_bAutoTargetNextOnDeath = DF_AUTOTARGETNEXTONDEATH;
	_bTargetLockTestLOS = DF_TARGETLOCKTESTLOS;
	_bTargetLockHostileActorsOnly = DF_TARGETLOCKHOSTILEACTORSONLY;
	_bTargetLockHideCrosshair = DF_TARGETLOCKHIDECROSSHAIR;
}

void DirectionalMovementHandler::Initialize()
{
	auto dataHandler = RE::TESDataHandler::GetSingleton();
	if (dataHandler) {
		_targetLockSpell = dataHandler->LookupForm<RE::SpellItem>(0x805, "TrueDirectionalMovement.esp");
		_directionalMovementGlobal = dataHandler->LookupForm<RE::TESGlobal>(0x807, "TrueDirectionalMovement.esp");
	}
}

void DirectionalMovementHandler::OnLoad()
{
	ResetControls();
	ResetDesiredAngle();
	ToggleTargetLock(false);
	_bIsDodging = false;
	_attackState = kNone;
	_target = RE::ActorHandle();
	_softTarget = RE::ActorHandle();
	_dialogueSpeaker = RE::ObjectRefHandle();
	_bossTargets.clear();
	_playerIsNPC = false;
}

void DirectionalMovementHandler::LoadIniSettings()
{
	logger::info("Reading .inis...");

	constexpr auto path = L"Data/SKSE/Plugins/TrueDirectionalMovement";
	constexpr auto ext = L".ini";

	const auto dataHandler = RE::TESDataHandler::GetSingleton();

	for (const auto& file : std::filesystem::directory_iterator(path)) // read all ini files in Data/SKSE/Plugins/TrueDirectionalMovement folder
	{
		if (std::filesystem::is_regular_file(file) && file.path().extension() == ext)
		{
			CSimpleIniA ini;
			ini.SetUnicode();
			ini.SetMultiKey();

			ini.LoadFile(file.path().string().c_str());

			CSimpleIniA::TNamesDepend races;
			ini.GetAllValues("BossRecognition", "Race", races);

			CSimpleIniA::TNamesDepend locRefTypes;
			ini.GetAllValues("BossRecognition", "LocRefType", locRefTypes);

			CSimpleIniA::TNamesDepend npcs;
			ini.GetAllValues("BossRecognition", "NPC", npcs);

			CSimpleIniA::TNamesDepend blacklistedNPCs;
			ini.GetAllValues("BossRecognition", "NPCBlacklist", blacklistedNPCs);

			for (const auto& entry : races)
			{
				std::string str = entry.pItem;
				auto split = str.find(':');
				auto modName = str.substr(0, split);
				auto formIDstr = str.substr(split + 1, str.find(' ', split + 1) - (split + 1));
				RE::FormID formID;
				formID = std::stoi(formIDstr.data(), 0, 16);
				auto race = dataHandler->LookupForm<RE::TESRace>(formID, modName);
				if (race)
				{
					_bossRaces.emplace(race);
				}
			}

			for (const auto& entry : locRefTypes) {
				std::string str = entry.pItem;
				auto split = str.find(':');
				auto modName = str.substr(0, split);
				auto formIDstr = str.substr(split + 1, str.find(' ', split + 1) - (split + 1));
				if (formIDstr.empty()) {
					continue;
				}
				RE::FormID formID = std::stoi(formIDstr.data(), 0, 16);
				auto locRefType = dataHandler->LookupForm<RE::BGSLocationRefType>(formID, modName);
				if (locRefType) {
					_bossLocRefTypes.emplace(locRefType);
				}
			}

			for (const auto& entry : npcs) {
				std::string str = entry.pItem;
				auto split = str.find(':');
				auto modName = str.substr(0, split);
				auto formIDstr = str.substr(split + 1, str.find(' ', split + 1) - (split + 1));
				if (formIDstr.empty()) {
					continue;
				}
				RE::FormID formID = std::stoi(formIDstr.data(), 0, 16);
				auto npc = dataHandler->LookupForm<RE::TESNPC>(formID, modName);
				if (npc) {
					_bossNPCs.emplace(npc);
				}
			}

			for (const auto& entry : blacklistedNPCs) {
				std::string str = entry.pItem;
				auto split = str.find(':');
				auto modName = str.substr(0, split);
				auto formIDstr = str.substr(split + 1, str.find(' ', split + 1) - (split + 1));
				if (formIDstr.empty()) {
					continue;
				}
				RE::FormID formID = std::stoi(formIDstr.data(), 0, 16);
				auto npc = dataHandler->LookupForm<RE::TESNPC>(formID, modName);
				if (npc) {
					_bossNPCBlacklist.emplace(npc);
				}
			}
		}
	}

	logger::info("...success");
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

	// From SmoothCam - Improved Camera compatibility
	auto hMod = GetModuleHandle(L"ImprovedCamera.dll");
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
		auto sz = GetFileVersionInfoSize(L"ImprovedCamera.dll", &dwHandle);
		if (sz != 0) {
			LPSTR verData = (LPSTR)malloc(sizeof(char) * sz);

			if (GetFileVersionInfo(L"ImprovedCamera.dll", dwHandle, sz, verData)) {
				LPBYTE lpBuffer = NULL;
				UINT size = 0;
				if (VerQueryValue(verData, L"\\", reinterpret_cast<void**>(&lpBuffer), &size) && size) {
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
							_ImprovedCamera_IsThirdPerson = reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(hMod) + 0x4d510);
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

DirectionalMovementHandler::DirectionalMovementHandler() :
	_lock(),
	_bDirectionalMovementSheathed(DF_DIRECTIONALMOVEMENTSHEATHED),
	_bDirectionalMovementDrawn(DF_DIRECTIONALMOVEMENTDRAWN),
	_dialogueMode(DF_DIALOGUEMODE),
	_bHeadtracking(DF_HEADTRACKING),
	_dialogueHeadtrackingDuration(DF_DIALOGUEHEADTRACKINGDURATION),
	_bCameraHeadtracking(DF_CAMERAHEADTRACKING),
	_cameraHeadtrackingStrength(DF_CAMERAHEADTRACKINGSTRENGTH),
	_bStopCameraHeadtrackingBehindPlayer(DF_STOPCAMERAHEADTRACKINGBEHINDPLAYER),
	_bFaceCrosshairWhileAttacking(DF_FACECROSSHAIRWHILEATTACKING),
	_bFaceCrosshairWhileBlocking(DF_FACECROSSHAIRWHILEBLOCKING),
	_bFaceCrosshairDuringAutoMove(DF_FACECROSSHAIRDURINGAUTOMOVE),
	_runningRotationSpeedMult(DF_RUNNINGROTATIONSPEEDMULT),
	_sprintingRotationSpeedMult(DF_SPRINTINGROTATIONSPEEDMULT),
	_attackStartRotationSpeedMult(DF_ATTACKSTARTROTATIONSPEEDMULT),
	_attackMidRotationSpeedMult(DF_ATTACKMIDROTATIONSPEEDMULT),
	_attackEndRotationSpeedMult(DF_ATTACKENDROTATIONSPEEDMULT),
	_airRotationSpeedMult(DF_AIRROTATIONSPEEDMULT),
	_bDisableAttackRotationMultipliersForTransformations(DF_DISABLEATTACKROTATIONMULTIPLIERSFORTRANSFORMATIONS),
	_bStopOnDirectionChange(DF_STOPONDIRECTIONCHANGE),
	_targetLockDistance(DF_TARGETLOCKDISTANCE),
	_targetLockPitchAdjustSpeed(DF_TARGETLOCKPITCHADJUSTSPEED),
	_targetLockYawAdjustSpeed(DF_TARGETLOCKYAWADJUSTSPEED),
	_targetLockPitchOffsetStrength(DF_TARGETLOCKPITCHOFFSETSTRENGTH),
	_bTargetLockUseMouse(DF_TARGETLOCKUSEMOUSE),
	_bTargetLockUseScrollWheel(DF_TARGETLOCKUSESCROLLWHEEL),
	_bTargetLockUseRightThumbstick(DF_TARGETLOCKUSERIGHTTHUMBSTICK),
	_targetLockArrowAimType(DF_TARGETLOCKARROWAIMTYPE),
	_targetLockMissileAimType(DF_TARGETLOCKMISSILEAIMTYPE),
	_bAutoTargetNextOnDeath(DF_AUTOTARGETNEXTONDEATH),
	_bTargetLockTestLOS(DF_TARGETLOCKTESTLOS),
	_bTargetLockHostileActorsOnly(DF_TARGETLOCKHOSTILEACTORSONLY),
	_bTargetLockHideCrosshair(DF_TARGETLOCKHIDECROSSHAIR)
{}
