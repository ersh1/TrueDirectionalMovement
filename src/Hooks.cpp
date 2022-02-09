#include "Hooks.h"
#include "Settings.h"
#include "DirectionalMovementHandler.h"
#include "Offsets.h"
#include "Utils.h"

namespace Hooks
{
	struct SaveCamera
	{
		enum class RotationType : uint8_t
		{
			kNone,
			kFirstPerson,
			kThirdPerson,
			kHorse
		};
		RotationType rotationType = RotationType::kNone;
		RE::NiPoint2 rotation { 0.f, 0.f };
		bool bZoomSaved = false;
		float zoom = 0.f;

		void SaveX(float a_x, RotationType a_rotationType)
		{
			rotation.x = a_x;
			rotationType = a_rotationType;
		}

		void SaveY(float a_y, RotationType a_rotationType)
		{
			rotation.y = a_y;
			rotationType = a_rotationType;
		}

		void SaveXY(RE::NiPoint2 a_xy, RotationType a_rotationType)
		{
			rotation = a_xy;
			rotationType = a_rotationType;
		}

		float ConsumeX()
		{
			rotationType = RotationType::kNone;
			return rotation.x;
		}

		float ConsumeY()
		{
			rotationType = RotationType::kNone;
			return rotation.y;
		}

		RE::NiPoint2& ConsumeXY()
		{
			rotationType = RotationType::kNone;
			return rotation;
		}

		void SaveZoom(float a_zoom)
		{
			zoom = a_zoom;
			bZoomSaved = true;
		}

		float ConsumeZoom()
		{
			bZoomSaved = false;
			return zoom;
		}

	} savedCamera;

	void Install()
	{
		logger::trace("Hooking...");

		MovementHook::Hook();
		LookHook::Hook();
		TogglePOVHook::Hook();
		FirstPersonStateHook::Hook();
		ThirdPersonStateHook::Hook();
		HorseCameraStateHook::Hook();
		TweenMenuCameraStateHook::Hook();
		VATSCameraStateHook::Hook();
		PlayerCameraTransitionStateHook::Hook();
		MovementHandlerAgentPlayerControlsHook::Hook();
		ProjectileHook::Hook();
		PlayerCharacterHook::Hook();
		PlayerControlsHook::Hook();
		AIProcess_SetRotationSpeedZHook::Hook();
		Actor_SetRotationHook::Hook();
		EnemyHealthHook::Hook();
		HeadtrackingHook::Hook();
		NukeSetIsNPCHook::Hook();
		PlayerCameraHook::Hook();
		MainUpdateHook::Hook();
		HorseAimHook::Hook();
		//CrosshairPickHook::Hook();

		logger::trace("...success");
	}

	void MovementHook::ProcessThumbstick(RE::MovementHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data)
	{
		bool bHandled = false;
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (a_event && directionalMovementHandler->IsFreeCamera() && a_event->IsLeft() && playerCharacter && !playerCharacter->IsOnMount())
		{
			RE::NiPoint2 inputDirection(a_event->xValue, a_event->yValue);
			bHandled = directionalMovementHandler->ProcessInput(inputDirection, a_data);
		}

		if (!bHandled)
		{
			_ProcessThumbstick(a_this, a_event, a_data);
			if (Settings::bThumbstickBounceFix) {
				directionalMovementHandler->SetLastInputDirection(a_data->moveInputVec);
			}
		}
	}

	void MovementHook::ProcessButton(RE::MovementHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		bool bHandled = false;
		DirectionalMovementHandler* directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		auto pressedDirections = &directionalMovementHandler->_pressedDirections;
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (a_event && directionalMovementHandler->IsFreeCamera() && playerCharacter && !playerCharacter->IsOnMount())
		{
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			bool bRelevant = false;

			RE::NiPoint2 inputDirection(0.f, 0.f);

			if (userEvent == userEvents->forward) {
				a_event->IsPressed() ? pressedDirections->set(DirectionalMovementHandler::Directions::kForward) : pressedDirections->reset(DirectionalMovementHandler::Directions::kForward);
				bRelevant = true;
			} else if (userEvent == userEvents->back) {
				a_event->IsPressed() ? pressedDirections->set(DirectionalMovementHandler::Directions::kBack) : pressedDirections->reset(DirectionalMovementHandler::Directions::kBack);
				bRelevant = true;
			} else if (userEvent == userEvents->strafeLeft) {
				a_event->IsPressed() ? pressedDirections->set(DirectionalMovementHandler::Directions::kLeft) : pressedDirections->reset(DirectionalMovementHandler::Directions::kLeft);
				bRelevant = true;
			} else if (userEvent == userEvents->strafeRight) {
				a_event->IsPressed() ? pressedDirections->set(DirectionalMovementHandler::Directions::kRight) : pressedDirections->reset(DirectionalMovementHandler::Directions::kRight);
				bRelevant = true;
			}

			if (bRelevant)
			{	
				if (pressedDirections->any(DirectionalMovementHandler::Directions::kForward)) {
					inputDirection.y += 1.f;
				}
				if (pressedDirections->any(DirectionalMovementHandler::Directions::kBack)) {
					inputDirection.y -= 1.f;
				}
				if (pressedDirections->any(DirectionalMovementHandler::Directions::kRight)) {
					inputDirection.x += 1.f;
				} 
				if (pressedDirections->any(DirectionalMovementHandler::Directions::kLeft)) {
					inputDirection.x -= 1.f;
				}

				bHandled = directionalMovementHandler->ProcessInput(inputDirection, a_data);
			}
		}

		if (!bHandled)
		{
			*pressedDirections = DirectionalMovementHandler::Directions::kInvalid;
			_ProcessButton(a_this, a_event, a_data);
			if (Settings::bThumbstickBounceFix) {
				directionalMovementHandler->SetLastInputDirection(a_data->moveInputVec);
			}
			
		}
	}

	static bool bTargetRecentlySwitched;

	void LookHook::ProcessThumbstick(RE::LookHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && a_event->IsRight() && directionalMovementHandler->HasTargetLocked() && !directionalMovementHandler->ShouldFaceCrosshair()) 
		{
			if (!Settings::bTargetLockUseRightThumbstick)
			{
				return;  // ensure lock camera movement during lockon
			}

			float absX = fabs(a_event->xValue);
			float absY = fabs(a_event->yValue);

			if (absX + absY > 0.1f && !bTargetRecentlySwitched) {
				if (absX > absY) {
					if (a_event->xValue > 0) {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kRight);
					} else {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kLeft);
					}
				} else {
					if (a_event->yValue > 0) {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kBack);
					} else {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kForward);
					}
				}

				bTargetRecentlySwitched = true;
			} 
			else if (absX + absY <= 0.1f)
			{
				bTargetRecentlySwitched = false;
			}
		}
		else
		{
			bTargetRecentlySwitched = false;
			if (Settings::bCameraHeadtracking && Settings::fCameraHeadtrackingDuration > 0.f) {
				directionalMovementHandler->RefreshCameraHeadtrackTimer();
			}

			if (directionalMovementHandler->IsCameraResetting()) {
				return; // ensure lock camera movement during camera reset
			}

			if (Settings::uAdjustCameraYawDuringMovement > CameraAdjustMode::kDisable && Settings::fCameraAutoAdjustDelay > 0.f) {
				directionalMovementHandler->ResetCameraRotationDelay();
			}

			_ProcessThumbstick(a_this, a_event, a_data);
		}
	}

	void LookHook::ProcessMouseMove(RE::LookHandler* a_this, RE::MouseMoveEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && directionalMovementHandler->HasTargetLocked() && !directionalMovementHandler->ShouldFaceCrosshair())
		{
			if (!Settings::bTargetLockUseMouse)
			{
				return; // ensure lock camera movement during lockon
			}

			int32_t absX = abs(a_event->mouseInputX);
			int32_t absY = abs(a_event->mouseInputY);

			if (absX + absY > static_cast<int32_t>(Settings::uTargetLockMouseSensitivity))
			{
				if (absX > absY)
				{
					if (a_event->mouseInputX > 0) {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kRight);
					} else {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kLeft);
					}
				}
				else 
				{
					if (a_event->mouseInputY > 0) {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kBack);
					} else {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kForward);
					}
				}

				bTargetRecentlySwitched = true;
			} 
			else if (absX + absY <= static_cast<int32_t>(Settings::uTargetLockMouseSensitivity))
			{
				bTargetRecentlySwitched = false;
			}
		}
		else
		{
			bTargetRecentlySwitched = false;
			if (Settings::bCameraHeadtracking && Settings::fCameraHeadtrackingDuration > 0.f) {
				directionalMovementHandler->RefreshCameraHeadtrackTimer();
			}

			if (directionalMovementHandler->IsCameraResetting()) {
				return;  // ensure lock camera movement during camera reset
			}

			if (Settings::uAdjustCameraYawDuringMovement > CameraAdjustMode::kDisable && Settings::fCameraAutoAdjustDelay > 0.f) {
				directionalMovementHandler->ResetCameraRotationDelay();
			}

			_ProcessMouseMove(a_this, a_event, a_data);
		}
	}

	static bool bPressedPOVToUnlock = false;
	static bool bInTargetLockWindow = false;

	void TogglePOVHook::ProcessButton(RE::TogglePOVHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		if (a_event && BSInputDeviceManager_IsUsingGamepad(RE::BSInputDeviceManager::GetSingleton()) ? Settings::bTargetLockUsePOVSwitchGamepad : Settings::bTargetLockUsePOVSwitchKeyboard) {
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->togglePOV)
			{
				auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
				if (directionalMovementHandler->HasTargetLocked()) {
					if (a_event->IsDown()) {
						directionalMovementHandler->ToggleTargetLock(false, true);
						bPressedPOVToUnlock = true;
					}
					return;
				} else {
					if (a_event->HeldDuration() < Settings::fTargetLockPOVHoldDuration) {
						if (a_event->IsDown()) {
							bInTargetLockWindow = true;
						}

						if (a_event->IsUp()) {
							if (!bPressedPOVToUnlock) {
								directionalMovementHandler->ToggleTargetLock(true, true);
							}
							bPressedPOVToUnlock = false;
							bInTargetLockWindow = false;
						}

						return;
					} else {
						if (bInTargetLockWindow) {
							a_event->heldDownSecs = 0.f;
							bInTargetLockWindow = false;
						}						
						bPressedPOVToUnlock = false;
					}
				}
			}
		}

		_ProcessButton(a_this, a_event, a_data);
	}

	void FirstPersonStateHook::OnEnterState(RE::FirstPersonState* a_this)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (directionalMovementHandler->GetFreeCameraEnabled()) {
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			if (playerCharacter) {
				// turn character towards where the camera was looking in third person state before entering first person state
				if (savedCamera.rotationType == SaveCamera::RotationType::kThirdPerson) {
					auto x = savedCamera.ConsumeX();
					if (playerCharacter->actorState1.sitSleepState == RE::SIT_SLEEP_STATE::kNormal) {  // don't do this while sitting, sleeping etc.
						playerCharacter->SetRotationZ(x);
					}
				}
				savedCamera.bZoomSaved = false;

				directionalMovementHandler->ResetDesiredAngle();
			}
		}

		_OnEnterState(a_this);
	}

	void FirstPersonStateHook::OnExitState(RE::FirstPersonState* a_this)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		
		if (playerCharacter)
		{
			savedCamera.SaveX(playerCharacter->data.angle.z, SaveCamera::RotationType::kFirstPerson);
		}
		
		_OnExitState(a_this);
	}

	void FirstPersonStateHook::ProcessButton(RE::FirstPersonState* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && directionalMovementHandler->HasTargetLocked() && Settings::bTargetLockUseScrollWheel)
		{
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->zoomIn) {
				directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kLeft);
				return;
			} else if (userEvent == userEvents->zoomOut) {
				directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kRight);
				return;
			}
		}

		if (a_event && BSInputDeviceManager_IsUsingGamepad(RE::BSInputDeviceManager::GetSingleton()) ? Settings::bTargetLockUsePOVSwitchGamepad : Settings::bTargetLockUsePOVSwitchKeyboard) {
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->togglePOV && a_event->IsUp() && a_event->HeldDuration() < Settings::fTargetLockPOVHoldDuration) {
				return;
			}
		}

		_ProcessButton(a_this, a_event, a_data);
	}

	void ThirdPersonStateHook::OnEnterState(RE::ThirdPersonState* a_this)
	{
		_OnEnterState(a_this);

		if (DirectionalMovementHandler::GetSingleton()->GetFreeCameraEnabled()) {
			if (savedCamera.rotationType == SaveCamera::RotationType::kHorse) {
				a_this->freeRotation.x = savedCamera.ConsumeX();
			}

			DirectionalMovementHandler::GetSingleton()->ResetDesiredAngle();
		}
	}

	void ThirdPersonStateHook::OnExitState(RE::ThirdPersonState* a_this)
	{
		if (DirectionalMovementHandler::GetSingleton()->GetFreeCameraEnabled()) {
			RE::Actor* cameraTarget = nullptr;
			cameraTarget = static_cast<RE::PlayerCamera*>(a_this->camera)->cameraTarget.get().get();
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			
			RE::NiPoint2 rot = a_this->freeRotation;
			rot.x += playerCharacter->data.angle.z;
			rot.y += playerCharacter->data.angle.x;
			savedCamera.SaveXY(rot, SaveCamera::RotationType::kThirdPerson);
			savedCamera.SaveZoom(a_this->targetZoomOffset);
		}

		_OnExitState(a_this);
	}

	void ThirdPersonStateHook::SetFreeRotationMode(RE::ThirdPersonState* a_this, bool a_weaponSheathed)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();

		//directionalMovementHandler->Update();

		if (directionalMovementHandler->IsFreeCamera())
		{
			RE::Actor* cameraTarget = nullptr;
			cameraTarget = static_cast<RE::PlayerCamera*>(a_this->camera)->cameraTarget.get().get();

			bool bHasTargetLocked = directionalMovementHandler->HasTargetLocked();

			if (cameraTarget) {
				a_this->freeRotationEnabled = true;
				directionalMovementHandler->UpdateAIProcessRotationSpeed(cameraTarget);  // because the game is skipping the original call while in freecam

				if (!bHasTargetLocked) {
					float pitchDelta = -a_this->freeRotation.y;

					bool bIsSwimming = cameraTarget->IsSwimming();
					// swimming pitch fix and swim up/down buttons handling
					if (bIsSwimming) {
						float currentPitch = cameraTarget->data.angle.x;

						if (directionalMovementHandler->_pressedDirections.any(DirectionalMovementHandler::Directions::kUp) || directionalMovementHandler->_pressedDirections.any(DirectionalMovementHandler::Directions::kDown)) {
							if (directionalMovementHandler->_pressedDirections.any(DirectionalMovementHandler::Directions::kUp)) {
								directionalMovementHandler->SetDesiredSwimmingPitchOffset(directionalMovementHandler->HasMovementInput() ? -PI / 4 : -PI / 2);
							}
							if (directionalMovementHandler->_pressedDirections.any(DirectionalMovementHandler::Directions::kDown)) {
								directionalMovementHandler->SetDesiredSwimmingPitchOffset(directionalMovementHandler->HasMovementInput() ? PI / 4 : PI / 2);
							}
							auto playerControls = RE::PlayerControls::GetSingleton();
							playerControls->data.moveInputVec = RE::NiPoint2(0.f, 1.f);
						} else if (directionalMovementHandler->_pressedDirections.underlying()) {
							directionalMovementHandler->SetDesiredSwimmingPitchOffset((currentPitch - a_this->freeRotation.y) * cos(fabs(a_this->freeRotation.x)));
						} else {
							directionalMovementHandler->SetDesiredSwimmingPitchOffset(0.f);
						}

						float desiredPitch = directionalMovementHandler->GetCurrentSwimmingPitchOffset();
						pitchDelta = desiredPitch - currentPitch;
						auto characterController = cameraTarget->GetCharController();
						if (characterController) {
							characterController->pitchAngle = desiredPitch;
						}
					}

					bool bMoving = cameraTarget->actorState1.movingBack ||
					               cameraTarget->actorState1.movingForward ||
					               cameraTarget->actorState1.movingRight ||
					               cameraTarget->actorState1.movingLeft;

					if (bMoving || !a_weaponSheathed) {
						//cameraTarget->SetRotationX(cameraTarget->data.angle.x + pitchDelta);
						cameraTarget->data.angle.x += pitchDelta;
						a_this->freeRotation.y += pitchDelta;
					}
				}
			}
		} else {
			_SetFreeRotationMode(a_this, a_weaponSheathed);
		}
	}

	void ThirdPersonStateHook::ProcessButton(RE::ThirdPersonState* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && directionalMovementHandler->HasTargetLocked() && Settings::bTargetLockUseScrollWheel) {
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->zoomIn) {
				directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kLeft);
				return;
			} else if (userEvent == userEvents->zoomOut) {
				directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kRight);
				return;
			}
		}

		if (a_event && BSInputDeviceManager_IsUsingGamepad(RE::BSInputDeviceManager::GetSingleton()) ? Settings::bTargetLockUsePOVSwitchGamepad : Settings::bTargetLockUsePOVSwitchKeyboard)
		{
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->togglePOV && a_event->IsUp() && a_event->HeldDuration() < Settings::fTargetLockPOVHoldDuration) {
				//directionalMovementHandler->ToggleTargetLock(!directionalMovementHandler->HasTargetLocked());
				return;
			}
		}

		_ProcessButton(a_this, a_event, a_data);
	}
	
	void HorseCameraStateHook::OnEnterState(RE::HorseCameraState* a_this)
	{
		_OnEnterState(a_this);

		if (DirectionalMovementHandler::GetSingleton()->GetFreeCameraEnabled()) {
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();

			RE::Actor* horse = nullptr;
			horse = static_cast<RE::Actor*>(a_this->horseRefHandle.get().get());

			if (savedCamera.rotationType != SaveCamera::RotationType::kNone) {
				auto rotationType = savedCamera.rotationType;
				RE::NiPoint2 rot = savedCamera.ConsumeXY();
				if (rotationType == SaveCamera::RotationType::kThirdPerson) {
					playerCharacter->data.angle.x = -rot.y;
				}
				
				a_this->freeRotation.x = NormalAbsoluteAngle(rot.x - horse->data.angle.z);
			}

			if (savedCamera.bZoomSaved) {
				a_this->targetZoomOffset = savedCamera.ConsumeZoom();
				a_this->currentZoomOffset = a_this->targetZoomOffset;
				a_this->savedZoomOffset = a_this->targetZoomOffset;
			}

			a_this->horseCurrentDirection = horse->GetHeading(false);
		}
	}

	void HorseCameraStateHook::OnExitState(RE::HorseCameraState* a_this)
	{
		if (DirectionalMovementHandler::GetSingleton()->GetFreeCameraEnabled()) {
			savedCamera.SaveXY(a_this->freeRotation, SaveCamera::RotationType::kHorse);
			savedCamera.SaveZoom(a_this->currentZoomOffset);
		}

		_OnExitState(a_this);
	}

	void HorseCameraStateHook::UpdateRotation(RE::HorseCameraState* a_this)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (directionalMovementHandler->GetFreeCameraEnabled() && !directionalMovementHandler->IFPV_IsFirstPerson() && !directionalMovementHandler->ImprovedCamera_IsFirstPerson()) {
			float horseCurrentDirection = a_this->horseCurrentDirection;
			float freeRotationX = a_this->freeRotation.x;

			a_this->freeRotationEnabled = true;

			_UpdateRotation(a_this);

			a_this->horseCurrentDirection = horseCurrentDirection;
			a_this->freeRotation.x = freeRotationX;

			if (a_this->horseRefHandle) {
				RE::Actor* horse = nullptr;
				horse = static_cast<RE::Actor*>(a_this->horseRefHandle.get().get());
				if (horse) {
					float heading = horse->GetHeading(false);

					a_this->freeRotation.x += a_this->horseCurrentDirection - heading;

					NiQuaternion_SomeRotationManipulation(a_this->rotation, -a_this->freeRotation.y, 0.f, heading + a_this->freeRotation.x);
					a_this->horseCurrentDirection = heading;
				}
			}
		} else {
			_UpdateRotation(a_this);
		}
	}

	void HorseCameraStateHook::HandleLookInput(RE::HorseCameraState* a_this, const RE::NiPoint2& a_input)
	{
		if (DirectionalMovementHandler::GetSingleton()->HasTargetLocked()) {
			return;
		}

		_HandleLookInput(a_this, a_input);
	}

	void HorseCameraStateHook::ProcessButton(RE::HorseCameraState* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && directionalMovementHandler->HasTargetLocked() && Settings::bTargetLockUseScrollWheel) {
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->zoomIn) {
				directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kLeft);
				return;
			} else if (userEvent == userEvents->zoomOut) {
				directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kRight);
				return;
			}
		}

		if (a_event && BSInputDeviceManager_IsUsingGamepad(RE::BSInputDeviceManager::GetSingleton()) ? Settings::bTargetLockUsePOVSwitchGamepad : Settings::bTargetLockUsePOVSwitchKeyboard) {
			auto userEvent = a_event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

			if (userEvent == userEvents->togglePOV && a_event->IsUp() && a_event->HeldDuration() < Settings::fTargetLockPOVHoldDuration) {
				//directionalMovementHandler->ToggleTargetLock(!directionalMovementHandler->HasTargetLocked());
				return;
			}
		}

		_ProcessButton(a_this, a_event, a_data);
	}

	void TweenMenuCameraStateHook::OnEnterState(RE::TESCameraState* a_this)
	{
		if (DirectionalMovementHandler::GetSingleton()->IsFreeCamera()) {
			savedCamera.rotationType = SaveCamera::RotationType::kNone;
		}
		
		_OnEnterState(a_this);
	}

	void TweenMenuCameraStateHook::OnExitState(RE::TESCameraState* a_this)
	{
		if (DirectionalMovementHandler::GetSingleton()->GetFreeCameraEnabled()) {
			savedCamera.rotationType = SaveCamera::RotationType::kNone;
		}

		_OnExitState(a_this);
	}

	void VATSCameraStateHook::OnExitState(RE::TESCameraState* a_this)
	{
		if (DirectionalMovementHandler::GetSingleton()->GetFreeCameraEnabled()) {
			savedCamera.rotationType = SaveCamera::RotationType::kNone;
		}

		_OnExitState(a_this);
	}

	void PlayerCameraTransitionStateHook::OnEnterState(RE::PlayerCameraTransitionState* a_this)
	{
		if (a_this->transitionFrom->id == RE::CameraStates::kMount && a_this->transitionTo->id == RE::CameraStates::kThirdPerson)
		{
			if (savedCamera.rotationType == SaveCamera::RotationType::kHorse) {
				auto thirdPersonState = static_cast<RE::ThirdPersonState*>(a_this->transitionTo);
				auto playerCharacter = RE::PlayerCharacter::GetSingleton();
				thirdPersonState->freeRotation.x = savedCamera.ConsumeX();
				playerCharacter->data.angle.x = -savedCamera.ConsumeY();
			}
		} else if (a_this->transitionFrom->id == RE::CameraStates::kMount && a_this->transitionTo->id == RE::CameraStates::kFirstPerson) {
			if (savedCamera.rotationType == SaveCamera::RotationType::kHorse) {
				auto playerCharacter = RE::PlayerCharacter::GetSingleton();
				playerCharacter->data.angle.x = -savedCamera.ConsumeY();
			}
		}

		_OnEnterState(a_this);
	}

	void MovementHandlerAgentPlayerControlsHook::Func1(void* a1, void* a2)
	{
		// disable dampening controls while sprinting during lock-on, so you don't retain any weird momentum when rotating back to target after sprinting
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (DirectionalMovementHandler::GetSingleton()->HasTargetLocked() && playerCharacter && playerCharacter->IsSprinting()) {
			*g_bDampenPlayerControls = false;
		} else {
			*g_bDampenPlayerControls = true;
		}
		_Func1(a1, a2);
	}

	void ProjectileHook::ProjectileAimSupport(RE::Projectile* a_this)
	{
		auto projectileNode = a_this->Get3D2();

		// player only, 0x100000 == player
		if (projectileNode && a_this->shooter.native_handle() == 0x100000) {
			auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
			if (directionalMovementHandler->HasTargetLocked() || a_this->desiredTarget.native_handle() != 0) {
				TargetLockProjectileAimType aimType;

				switch (a_this->formType.get()) {
				case RE::FormType::ProjectileArrow:
					aimType = Settings::uTargetLockArrowAimType;
					break;
				case RE::FormType::ProjectileMissile:
					aimType = Settings::uTargetLockMissileAimType;
					break;
				default:
					aimType = TargetLockProjectileAimType::kFreeAim;
				}

				if (aimType != TargetLockProjectileAimType::kFreeAim) {
					if (!a_this->desiredTarget.get()) {
						auto target = directionalMovementHandler->GetTarget();
						if (!target) {
							return;
						}
						a_this->desiredTarget = target;

						if (aimType == TargetLockProjectileAimType::kPredict) {
							// predict only at the start (desiredTarget not yet set), then let the projectile go unchanged in next updates
							RE::NiPoint3 targetPos;
							if (GetTargetPos(a_this->desiredTarget, targetPos)) {
								RE::NiPoint3 targetVelocity;
								target.get()->GetLinearVelocity(targetVelocity);

								float projectileGravity = 0.f;
								if (auto ammo = a_this->ammoSource) {
									if (auto bgsProjectile = ammo->data.projectile) {
										projectileGravity = bgsProjectile->data.gravity;
										if (auto bhkWorld = a_this->parentCell->GetbhkWorld()) {
											if (auto hkpWorld = bhkWorld->GetWorld1()) {
												auto vec4 = hkpWorld->gravity;
												float quad[4];
												_mm_store_ps(quad, vec4.quad);
												float gravity = -quad[2] * bhkWorld->GetWorldScaleInverse();
												projectileGravity *= gravity;
											}
										}
									}
								}

								PredictAimProjectile(a_this->data.location, targetPos, targetVelocity, projectileGravity, a_this->linearVelocity);

								// rotate
								RE::NiPoint3 direction = a_this->linearVelocity;
								direction.Unitize();

								a_this->data.angle.x = asin(direction.z);
								a_this->data.angle.z = atan2(direction.x, direction.y);

								if (a_this->data.angle.z < 0.0) {
									a_this->data.angle.z += PI;
								}

								if (direction.x < 0.0) {
									a_this->data.angle.z += PI;
								}

								SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
							}
						}
					}

					RE::NiPoint3 targetPos;
					if (aimType == TargetLockProjectileAimType::kHoming && GetTargetPos(a_this->desiredTarget, targetPos)) {
						// homing
						RE::NiPoint3& velocity = a_this->linearVelocity;
						float speed = velocity.Length();

						//if (speed < 1500.f) {
						//	return _GetLinearVelocity(a_this, a_outVelocity);
						//}

						RE::NiPoint3 direction = (targetPos - a_this->data.location);

						// normalize direction
						direction.Unitize();

						// rotate
						a_this->data.angle.x = asin(direction.z);
						a_this->data.angle.z = atan2(direction.x, direction.y);

						if (a_this->data.angle.z < 0.0) {
							a_this->data.angle.z += PI;
						}

						if (direction.x < 0.0) {
							a_this->data.angle.z += PI;
						}

						SetRotationMatrix(projectileNode->local.rotate, -direction.x, direction.y, direction.z);
						velocity = direction * speed;
					}
				}
			}
		}
	}

	void ProjectileHook::GetLinearVelocityProjectile(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity)
	{
		_GetLinearVelocityProjectile(a_this, a_outVelocity);

		ProjectileAimSupport(a_this);
	}

	void ProjectileHook::GetLinearVelocityArrow(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity)
	{
		_GetLinearVelocityArrow(a_this, a_outVelocity);

		ProjectileAimSupport(a_this);
	}

	void ProjectileHook::GetLinearVelocityMissile(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity)
	{
		_GetLinearVelocityMissile(a_this, a_outVelocity);

		ProjectileAimSupport(a_this);
	}

	void ProjectileHook::Func183(RE::Projectile* a_this)
	{
		// player only, 0x100000 == player
		if (a_this->shooter.native_handle() == 0x100000) {
			auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
			if (directionalMovementHandler->HasTargetLocked() &&
					a_this->desiredTarget.native_handle() == 0 &&                                          // the chained projectiles from spells like chain lightning will have the desiredTarget  
					Settings::uTargetLockMissileAimType != TargetLockProjectileAimType::kFreeAim) {        // handle variable filled by the aim support feature, the parent one doesn't yet.
				auto beamProjectile = skyrim_cast<RE::BeamProjectile*>(a_this);
				auto target = directionalMovementHandler->GetTarget();
				if (beamProjectile && target) {
					a_this->desiredTarget = target;

					RE::NiPoint3 targetPos;

					if (GetTargetPos(a_this->desiredTarget, targetPos)) {
						RE::NiPoint3 direction = (targetPos - a_this->data.location);

						// normalize direction
						direction.Unitize();

						// rotate
						a_this->data.angle.x = atan2(-direction.z, std::sqrtf(direction.x * direction.x + direction.y * direction.y));
						a_this->data.angle.z = atan2(direction.x, direction.y);

						if (a_this->data.angle.z < 0.0) {
							a_this->data.angle.z += PI;
						}

						if (direction.x < 0.0) {
							a_this->data.angle.z += PI;
						}
					}
				}
			}
		}

		// call the original vfunc
		typedef void (__thiscall RE::Projectile::*Func183)() const;
		(a_this->*reinterpret_cast<Func183>(&RE::Projectile::Unk_B7))();
	}

	void PlayerCharacterHook::UpdateAnimation(RE::Actor* a_this, float a_delta)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		auto previousAimAngle = directionalMovementHandler->GetCurrentHorseAimAngle();
		directionalMovementHandler->SetCurrentHorseAimAngle(0.f); // the horse aim function only gets called when you're actually aiming on a horse, so we have to reset here to detect when you stop aiming

		_UpdateAnimation(a_this, a_delta);

		auto updatedAimAngle = directionalMovementHandler->GetCurrentHorseAimAngle(); // if we were aiming, it will be updated by now

		if (updatedAimAngle != previousAimAngle) {
			directionalMovementHandler->SetPreviousHorseAimAngle(previousAimAngle);
			directionalMovementHandler->UpdateHorseAimDirection();
		}

		//directionalMovementHandler->UpdateLeaning();
	}

	void PlayerCharacterHook::ProcessTracking(RE::Actor* a_this, float a_delta, RE::NiAVObject* a_obj3D)
	{
		// Handle TDM headtracking stuff that needs to be done before calling the original

		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();

		bool bBehaviorPatchInstalled = DirectionalMovementHandler::IsBehaviorPatchInstalled(a_this);

		if (bBehaviorPatchInstalled)
		{
			// for good measure
			a_this->SetGraphVariableBool("IsNPC", false);
		}

		if (directionalMovementHandler->IFPV_IsFirstPerson() || directionalMovementHandler->ImprovedCamera_IsFirstPerson())
		{
			a_this->actorState2.headTracking = false;
			a_this->SetGraphVariableBool("IsNPC", false);
			return _ProcessTracking(a_this, a_delta, a_obj3D);
		}
		
		bool bIsHeadtrackingEnabled = Settings::bHeadtracking && !directionalMovementHandler->GetForceDisableHeadtracking();
		bool bIsBlocking = false;
		bool bIsSprinting = false;

		if (bIsHeadtrackingEnabled && a_this->currentProcess) {
			if (!bBehaviorPatchInstalled) {
				a_this->actorState2.headTracking = true;
				a_this->SetGraphVariableBool("IsNPC", true);
			}

			a_this->SetGraphVariableBool("bHeadTrackSpine", Settings::bHeadtrackSpine ? true : false);
			
			// expire dialogue headtrack if timer is up
			if (a_this->currentProcess->high && a_this->currentProcess->high->headTrack3) {
				if (directionalMovementHandler->GetDialogueHeadtrackTimer() <= 0.f) {
					a_this->currentProcess->high->SetHeadtrackTarget(3, nullptr);
				}
			}

			// set headtracking variables if we have any target set
			auto target = a_this->currentProcess->GetHeadtrackTarget();
			if (target) {
				a_this->actorState2.headTracking = true;
				if (!bBehaviorPatchInstalled) {
					a_this->SetGraphVariableBool("IsNPC", true);
				}
			} else {
				a_this->actorState2.headTracking = false;
				/*if (!bBehaviorPatchInstalled) {
					a_this->SetGraphVariableBool("IsNPC", false);
				}	*/
			}

			bIsBlocking = a_this->actorState2.wantBlocking;
			// disable headtracking while attacking or blocking without behavior patch
			if ((!bBehaviorPatchInstalled && bIsBlocking) ||
				(a_this->actorState1.meleeAttackState > RE::ATTACK_STATE_ENUM::kNone)) {
				a_this->actorState2.headTracking = false;
				if (!bBehaviorPatchInstalled) {
					a_this->SetGraphVariableBool("IsNPC", false);
				}
			}
		}

		RE::NiPoint3 targetPos;

		//a_this->actorState2.headTracking = true;

		// reset headtrack
		if (bIsHeadtrackingEnabled && a_this->currentProcess && a_this->currentProcess->high) {
			// clear the 0 and 1 targets if they're set for whatever reason
			auto selfHandle = a_this->GetHandle();
			if (a_this->currentProcess->high->headTrack0) {
				a_this->currentProcess->high->SetHeadtrackTarget(0, nullptr);
			}
			if (a_this->currentProcess->high->headTrack1) {
				a_this->currentProcess->high->SetHeadtrackTarget(1, nullptr);
			}			
		}

		// run original function
		// If player is on a mount, need to force the headtracking target to forward direction because for some reason it gets stuck otherwise
		if (a_this->IsOnMount()) {
			targetPos = a_this->GetLookingAtLocation();
			float yaw = NormalRelativeAngle(a_this->data.angle.z - PI2);
			RE::NiPoint3 offset = -RotationToDirection(yaw, 0.1f) * 500.f;
			offset.x *= -1.f;
			targetPos += offset;
			a_this->currentProcess->high->headTrackTargetOffset = targetPos;
			a_this->currentProcess->SetHeadtrackTarget(a_this, targetPos);
		}

		_ProcessTracking(a_this, a_delta, a_obj3D);

		// handle fake IsNPC
		if (!bIsHeadtrackingEnabled && bBehaviorPatchInstalled && !DirectionalMovementHandler::GetSingleton()->GetPlayerIsNPC()) {
			//a_this->currentProcess->SetHeadtrackTarget(a_this, targetPos);
			a_this->actorState2.headTracking = false;
		}

		bIsSprinting = a_this->actorState1.sprinting;

		if (bIsHeadtrackingEnabled &&
			Settings::bCameraHeadtracking &&
			(Settings::fCameraHeadtrackingDuration == 0.f || directionalMovementHandler->GetCameraHeadtrackTimer() > 0.f) &&
			!bIsSprinting &&
			!bIsBlocking &&
			
			a_this->currentProcess &&
			a_this->boolBits.none(RE::Actor::BOOL_BITS::kHasSceneExtra)) {
			// try camera headtracking
			auto highProcess = a_this->currentProcess->high;
			if (highProcess && 
				a_this->actorState1.meleeAttackState == RE::ATTACK_STATE_ENUM::kNone &&
				a_this->actorState1.sitSleepState != RE::SIT_SLEEP_STATE::kIsSleeping &&
				!highProcess->headTrack2 && !highProcess->headTrack3 && !highProcess->headTrack4 && !highProcess->headTrack5)
			{
				if (!bBehaviorPatchInstalled) {
					a_this->actorState2.headTracking = true;
					a_this->SetGraphVariableBool("IsNPC", true);
				}

				directionalMovementHandler->UpdateCameraHeadtracking();
			}
		}
	}

	/*std::string time_in_HH_MM_SS_MMM()
	{
		using namespace std::chrono;

		// get current time
		auto now = system_clock::now();

		// get number of milliseconds for the current second
		// (remainder after division into seconds)
		auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

		// convert to std::time_t in order to convert to std::tm (broken time)
		auto timer = system_clock::to_time_t(now);

		// convert to broken time
		std::tm bt;
		localtime_s(&bt, &timer);

		std::ostringstream oss;

		oss << std::put_time(&bt, "%H:%M:%S");	// HH:MM:SS
		oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

		return oss.str();
	}*/

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

	void PlayerCharacterHook::ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_dispatcher)
	{
		if (a_event)
		{
			std::string_view eventTag = a_event->tag.data();

			switch (hash(eventTag.data(), eventTag.size())) {
			// Start phase
			case "CastOKStart"_h:
			case "preHitFrame"_h:
			case "TDM_AttackStart"_h:
				DirectionalMovementHandler::GetSingleton()->SetAttackState(DirectionalMovementHandler::AttackState::kStart);
				break;

			// Mid phase. Ignore vanilla events if we're tracing already
			case "weaponSwing"_h:
			case "weaponLeftSwing"_h:
				if (DirectionalMovementHandler::GetSingleton()->GetAttackState() != DirectionalMovementHandler::AttackState::kTracing)
				{
					DirectionalMovementHandler::GetSingleton()->SetAttackState(DirectionalMovementHandler::AttackState::kMid);
				}
				break;
			case "TDM_AttackMid"_h:
			case "MeleeTrace_Right_Start"_h:
			case "MeleeTrace_Left_Start"_h:
				DirectionalMovementHandler::GetSingleton()->SetAttackState(DirectionalMovementHandler::AttackState::kMid);
				break;

			// End phase. Ignore vanilla events if we're tracing already
			case "HitFrame"_h:
			case "attackWinStart"_h:
			case "SkySA_AttackWinStart"_h:
				if (DirectionalMovementHandler::GetSingleton()->GetAttackState() != DirectionalMovementHandler::AttackState::kTracing)
				{
					DirectionalMovementHandler::GetSingleton()->SetAttackState(DirectionalMovementHandler::AttackState::kEnd);
				}
				break;
			case "TDM_AttackEnd"_h:
			case "MeleeTrace_Right_Stop"_h:
			case "MeleeTrace_Left_Stop"_h:
				DirectionalMovementHandler::GetSingleton()->SetAttackState(DirectionalMovementHandler::AttackState::kEnd);
				break;

			// Back to none
			case "attackStop"_h:
			case "TDM_AttackStop"_h:
			case "SkySA_AttackWinEnd"_h:
				DirectionalMovementHandler::GetSingleton()->SetAttackState(DirectionalMovementHandler::AttackState::kNone);
				break;
			}
			
			//logger::info("{} - {}", time_in_HH_MM_SS_MMM(), a_event->tag);
		}
		_ProcessEvent(a_this, a_event, a_dispatcher);
	}

	static void ApplyYawDelta(RE::ActorState* a_actorState, RE::NiPoint3& a_angle)
	{
		auto actor = static_cast<RE::Actor*>(a_actorState);
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		bool bIsAIDriven = actor->movementController && !actor->movementController->unk1C5;
		if (!bIsAIDriven) {
			a_angle.z -= DirectionalMovementHandler::GetSingleton()->GetYawDelta();
		} else {
			directionalMovementHandler->ResetYawDelta();
		}
	}

	void PlayerCharacterHook::GetAngle(RE::ActorState* a_this, RE::NiPoint3& a_angle)
	{
		_GetAngle(a_this, a_angle);
		
		ApplyYawDelta(a_this, a_angle);
	}

	void PlayerCharacterHook::UpdateSprintState(RE::PlayerCharacter* a_this)
	{
		auto playerControls = RE::PlayerControls::GetSingleton();
		RE::NiPointer<RE::Actor> mount = nullptr;
		bool bMounted = a_this->GetMount(mount);
		auto actor = bMounted ? mount.get() : a_this;
		if (a_this != nullptr) {
			bool bShouldBeSprinting = false;

			bool bIsSyncSprintState = Actor_IsSyncSprintState(actor);
			bool bIsSprintingRunningOrBlocking = actor->actorState1.sprinting == true || actor->IsRunning() || actor->IsBlocking();
			bool bUnk1 = Actor_CanSprint_CheckCharacterControllerValues(actor);
			bool bIsOverEncumbered = actor->IsOverEncumbered();
			bool bUnk2 = Actor_IsSyncSprintState(a_this) || (actor->GetAttackState() == RE::ATTACK_STATE_ENUM::kNone);
			bool bIsPreviousMoveInputForward = playerControls->data.prevMoveVec.y > 0.f;
			bool bIsNotStrafing = *g_fSprintStopThreshold > fabs(playerControls->data.prevMoveVec.x);
			bool bIsStaminaNotZero = actor->GetActorValue(RE::ActorValue::kStamina) > 0.f;
			bool bHasUnkBDD_SprintingFlag = (a_this->unkBDD & RE::PlayerCharacter::FlagBDD::kSprinting) != RE::PlayerCharacter::FlagBDD::kNone;

			// added
			auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
			bool bHasMovementInput = playerControls->data.prevMoveVec.x != 0.f || playerControls->data.prevMoveVec.y != 0.f;
			//bool bIsAttacking = directionalMovementHandler->GetAttackState() != DirectionalMovementHandler::AttackState::kNone;
			bool bIsDodging = directionalMovementHandler->IsDodging();
			//int iState;
			//a_this->GetGraphVariableInt("iState", iState);
			//bool bIsCasting = iState == 10;
			bool bFreeCamTargetLocked = directionalMovementHandler->HasTargetLocked() && directionalMovementHandler->IsFreeCamera();
			
			if (bMounted)
			{
				bIsPreviousMoveInputForward = bHasMovementInput;
				bIsNotStrafing = bHasMovementInput;
			}

			bool bSpecific = bFreeCamTargetLocked ? bHasMovementInput && !bIsDodging : bIsSprintingRunningOrBlocking && bIsPreviousMoveInputForward && bIsNotStrafing;  // branch depending on the mode we're in

			if (bHasUnkBDD_SprintingFlag &&
				!bUnk1 &&
				!bIsOverEncumbered &&
				bUnk2 &&
				bIsStaminaNotZero &&
				bSpecific)
			{
				bShouldBeSprinting = true;
			} else {
				bShouldBeSprinting = false;
				a_this->unkBDD.reset(RE::PlayerCharacter::FlagBDD::kSprinting);
			}

			if (bIsSyncSprintState != bShouldBeSprinting) {
				PlayerControls_CalledWhenSprintStateChanges(playerControls, 66 - bShouldBeSprinting, 2);  // ?
			}
		}
	}

	//void AIProcess_SetRotationSpeedZHook::AIProcess_SetRotationSpeedZ(RE::AIProcess* a_this, float a_rotationSpeed)
	//{
	//	if (a_this) {
	//		if (RE::PlayerCharacter::GetSingleton()->currentProcess == a_this && DirectionalMovementHandler::GetSingleton()->IsFreeCamera()) {
	//			return;	 // skip because we're setting it elsewhere and it'd overwrite to 0
	//		}
	//		a_this->middleHigh->rotationSpeed.z = a_rotationSpeed;
	//	}
	//}

	void AIProcess_SetRotationSpeedZHook::AIProcess_SetRotationSpeedZ1(RE::AIProcess* a_this, float a_rotationSpeed)
	{
		if (a_this == RE::PlayerCharacter::GetSingleton()->currentProcess && DirectionalMovementHandler::GetSingleton()->IsFreeCamera()) {
			return; // skip because we're setting it elsewhere and it'd overwrite to 0
		}
		return _AIProcess_SetRotationSpeedZ1(a_this, a_rotationSpeed);
	}

	void AIProcess_SetRotationSpeedZHook::AIProcess_SetRotationSpeedZ2(RE::AIProcess* a_this, float a_rotationSpeed)
	{
		if (a_this == RE::PlayerCharacter::GetSingleton()->currentProcess && DirectionalMovementHandler::GetSingleton()->IsFreeCamera()) {
			return; // skip because we're setting it elsewhere and it'd overwrite to 0
		}
		return _AIProcess_SetRotationSpeedZ2(a_this, a_rotationSpeed);
	}

	void AIProcess_SetRotationSpeedZHook::AIProcess_SetRotationSpeedZ3(RE::AIProcess* a_this, float a_rotationSpeed)
	{
		if (a_this == RE::PlayerCharacter::GetSingleton()->currentProcess && DirectionalMovementHandler::GetSingleton()->IsFreeCamera()) {
			return; // skip because we're setting it elsewhere and it'd overwrite to 0
		}
		return _AIProcess_SetRotationSpeedZ3(a_this, a_rotationSpeed);
	}

	void Actor_SetRotationHook::Actor_SetRotationX(RE::Actor* a_this, float a_angle)
	{
		if (a_this->IsPlayerRef()) {
			auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
			if (RE::PlayerCamera::GetSingleton()->currentState.get() == thirdPersonState && thirdPersonState->freeRotationEnabled) {
				float angleDelta = a_angle - a_this->data.angle.x;
				thirdPersonState->freeRotation.y += angleDelta;
			}
		}

		_Actor_SetRotationX(a_this, a_angle);
	}

	void Actor_SetRotationHook::Actor_SetRotationZ(RE::Actor* a_this, float a_angle)
	{
		if (a_this->IsPlayerRef())
		{
			auto thirdPersonState = static_cast<RE::ThirdPersonState*>(RE::PlayerCamera::GetSingleton()->cameraStates[RE::CameraState::kThirdPerson].get());
			if (RE::PlayerCamera::GetSingleton()->currentState.get() == thirdPersonState && thirdPersonState->freeRotationEnabled) {
				float angleDelta = a_angle - a_this->data.angle.z;
				thirdPersonState->freeRotation.x -= angleDelta;
			}
		}

		_Actor_SetRotationZ(a_this, a_angle);
	}

	bool EnemyHealthHook::ProcessMessage(uintptr_t a_enemyHealth, RE::HUDData* a_hudData)
	{
		bool bReturn = _ProcessMessage(a_enemyHealth, a_hudData);

		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();

		if (!directionalMovementHandler->IsMagnetismActive()) {
			RE::ActorHandle actorHandle = *(RE::ActorHandle*)(a_enemyHealth + 0x28);

			if (actorHandle) {
				DirectionalMovementHandler::GetSingleton()->SetSoftTarget(actorHandle);
			} else {
				DirectionalMovementHandler::GetSingleton()->SetSoftTarget(RE::ActorHandle());
			}
		}

		return bReturn;
	}

	void HeadtrackingHook::SetHeadtrackTarget0(RE::AIProcess* a_this, RE::Actor* a_target)
	{	
		// Skip for player so we don't get random headtracking targets
		if (Settings::bHeadtracking && !DirectionalMovementHandler::GetSingleton()->GetForceDisableHeadtracking() && a_this == RE::PlayerCharacter::GetSingleton()->currentProcess) {
			_SetHeadtrackTarget0(a_this, nullptr);
			return;
		}
		_SetHeadtrackTarget0(a_this, a_target);
	}

	// ridiculous, I know
	RE::TESObjectREFR* RecursiveSearchForParent(RE::NiAVObject* a_object)
	{
		if (!a_object) {
			return nullptr;
		}

		if (a_object->userData) {
			return a_object->userData;
		} else if (a_object->parent) {
			return RecursiveSearchForParent(a_object->parent);
		}
		return nullptr;
	}

	void SetHeadtrackTarget4(RE::AIProcess* a_this, RE::Actor* a_target)
	{
		if (Settings::bHeadtracking && !DirectionalMovementHandler::GetSingleton()->GetForceDisableHeadtracking() && a_target && a_target->IsPlayerRef() && a_this->middleHigh) {
			RE::NiAVObject* object = a_this->middleHigh->torsoNode;
			if (!object) {
				object = a_this->middleHigh->headNode;
			}
			auto refr = RecursiveSearchForParent(object);
			if (refr) {
				auto actor = refr->As<RE::Actor>();
				if (actor) {
					//_SetHeadtrackTarget4(a_target->currentProcess, actor);
					if (a_target->currentProcess && a_target->currentProcess->high) {
						a_target->currentProcess->high->SetHeadtrackTarget(3, actor);  // for player, use lower priority so target lock overrides dialogue targets
						DirectionalMovementHandler::GetSingleton()->RefreshDialogueHeadtrackTimer();
					}
				}
			}
		}
	}

	void HeadtrackingHook::SetHeadtrackTarget4A(RE::AIProcess* a_this, RE::Actor* a_target)
	{
		_SetHeadtrackTarget4A(a_this, a_target);

		SetHeadtrackTarget4(a_this, a_target);
	}

	void HeadtrackingHook::SetHeadtrackTarget4B(RE::AIProcess* a_this, RE::Actor* a_target)
	{
		_SetHeadtrackTarget4B(a_this, a_target);

		SetHeadtrackTarget4(a_this, a_target);
	}

	void HeadtrackingHook::SetHeadtrackTarget4C(RE::AIProcess* a_this, RE::Actor* a_target)
	{
		_SetHeadtrackTarget4C(a_this, a_target);

		SetHeadtrackTarget4(a_this, a_target);
	}

	void HeadtrackingHook::SetHeadtrackTarget4D(RE::AIProcess* a_this, RE::Actor* a_target)
	{
		_SetHeadtrackTarget4D(a_this, a_target);

		SetHeadtrackTarget4(a_this, a_target);
	}

	static RE::InputEvent* lastInputEvent = nullptr;

	bool PlayerControlsHook::Handle(RE::PlayerControls* a_this, RE::InputEvent** a_event)
	{
		DirectionalMovementHandler::ResetControls();
		lastInputEvent = *a_event;
		return _Handle(a_this, a_event);
	}

	void ApplyCameraMovement()
	{
		if (lastInputEvent) {
			auto playerControls = RE::PlayerControls::GetSingleton();
			for (RE::InputEvent* inputEvent = lastInputEvent; inputEvent != nullptr; inputEvent = inputEvent->next) {
				if (inputEvent->eventType == RE::INPUT_EVENT_TYPE::kMouseMove) {
					RE::MouseMoveEvent* mouseMoveEvent = static_cast<RE::MouseMoveEvent*>(inputEvent);
					playerControls->lookHandler->ProcessMouseMove(mouseMoveEvent, &playerControls->data);
					PlayerControls_ApplyLookSensitivitySettings(playerControls, &playerControls->data.lookInputVec);
				} else if (inputEvent->GetEventType() == RE::INPUT_EVENT_TYPE::kThumbstick) {
					RE::ThumbstickEvent* thumbstickEvent = static_cast<RE::ThumbstickEvent*>(inputEvent);
					if (thumbstickEvent->IsRight()) {
						playerControls->lookHandler->ProcessThumbstick(thumbstickEvent, &playerControls->data);
						PlayerControls_ApplyLookSensitivitySettings(playerControls, &playerControls->data.lookInputVec);
					}
				}
			}
		}
	}

	bool PlayerControlsHook::CheckIsInSyncAnim(void* a_a1, void* a_a2)
	{
		bool bIsInSyncAnim = _CheckIsInSyncAnim(a_a1, a_a2);

		if (bIsInSyncAnim) {
			ApplyCameraMovement();
		}

		return bIsInSyncAnim;
	}

	bool PlayerControlsHook::Check2(RE::PlayerCharacter* a_this)
	{
		bool bResult = _Check2(a_this);

		if (bResult) {
			ApplyCameraMovement();
		}

		return bResult;
	}

	bool PlayerControlsHook::Check3(RE::PlayerCharacter* a_this)
	{
		bool bResult = _Check3(a_this);

		if (bResult) {
			ApplyCameraMovement();
		}

		return bResult;
	}

	//bool PlayerControlsHook::CanProcessControls(RE::PlayerControls* a_this, RE::InputEvent** a_eventPtr)
	//{
	//	bool bCanProcessControls = _CanProcessControls(a_this, a_eventPtr);

	//	// process camera movement during locked controls
	//	if (!bCanProcessControls && !RE::UI::GetSingleton()->GameIsPaused()) {
	//		for (RE::InputEvent* inputEvent = *a_eventPtr; inputEvent != nullptr; inputEvent = inputEvent->next) {
	//			if (inputEvent->eventType == RE::INPUT_EVENT_TYPE::kMouseMove) {
	//				RE::MouseMoveEvent* mouseMoveEvent = static_cast<RE::MouseMoveEvent*>(inputEvent);
	//				a_this->lookHandler->ProcessMouseMove(mouseMoveEvent, &a_this->data);
	//				PlayerControls_ApplyLookSensitivitySettings(a_this, &a_this->data.lookInputVec);
	//			} else if (inputEvent->GetEventType() == RE::INPUT_EVENT_TYPE::kThumbstick) {
	//				RE::ThumbstickEvent* thumbstickEvent = static_cast<RE::ThumbstickEvent*>(inputEvent);
	//				if (thumbstickEvent->IsRight()) {
	//					a_this->lookHandler->ProcessThumbstick(thumbstickEvent, &a_this->data);
	//					PlayerControls_ApplyLookSensitivitySettings(a_this, &a_this->data.lookInputVec);
	//				}
	//			}
	//		}
	//	}

	//	return bCanProcessControls;
	//}

	void NukeSetIsNPCHook::SetBool(RE::IAnimationGraphManagerHolder* a_this, RE::BSFixedString* a_variableName, bool a_value)
	{
		if (a_variableName && a_variableName->c_str() == "IsNPC"sv)
		{
			auto ref = static_cast<RE::TESObjectREFR*>(a_this);
			auto formID = ref->formID;
			if (formID == 0x14 && DirectionalMovementHandler::IsBehaviorPatchInstalled(ref)) // player
			{
				DirectionalMovementHandler::GetSingleton()->SetPlayerIsNPC(a_value);
				a_value = false;
			}
		}
		_SetBool(a_this, a_variableName, a_value);
	}

	void NukeSetIsNPCHook::SetInt(RE::IAnimationGraphManagerHolder* a_this, RE::BSFixedString* a_variableName, int32_t a_value)
	{
		if (a_variableName && a_variableName->c_str() == "IsNPC"sv) {
			auto ref = static_cast<RE::TESObjectREFR*>(a_this);
			auto formID = ref->formID;
			if (formID == 0x14 && DirectionalMovementHandler::IsBehaviorPatchInstalled(ref))  // player
			{
				DirectionalMovementHandler::GetSingleton()->SetPlayerIsNPC(a_value);
				a_value = 0;
			}
		}
		_SetInt(a_this, a_variableName, a_value);
	}

	void PlayerCameraHook::Update(RE::TESCamera* a_this)
	{
		_Update(a_this);

		DirectionalMovementHandler::GetSingleton()->UpdatePlayerPitch();
	}

	void PlayerCameraHook::SetCameraState(RE::TESCamera* a_this, RE::TESCameraState* a_newState)
	{
		if (a_this->currentState)
		{
			auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
			directionalMovementHandler->SetCameraStateBeforeTween(a_this->currentState->id);
		}

		_SetCameraState(a_this, a_newState);
	}

	void MainUpdateHook::Update(RE::Main* a_this, float a2)
	{
		_Update(a_this, a2);

		DirectionalMovementHandler::GetSingleton()->Update();
	}

	static float angleToTarget = 0.f;
	float* HorseAimHook::GetHorseCameraFreeRotationYaw(RE::PlayerCamera* a_this)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		float* ret = nullptr;
		if (directionalMovementHandler->HasTargetLocked() && directionalMovementHandler->GetTarget())
		{
			auto playerCharacter = RE::PlayerCharacter::GetSingleton();
			auto target = directionalMovementHandler->GetTarget();
			RE::NiPoint2 playerPos;
			playerPos.x = playerCharacter->GetPositionX();
			playerPos.y = playerCharacter->GetPositionY();
			RE::NiPoint2 targetPos;
			targetPos.x = target.get()->GetPositionX();
			targetPos.y = target.get()->GetPositionY();

			RE::NiPoint2 directionToTarget = RE::NiPoint2(-(targetPos.x - playerPos.x), targetPos.y - playerPos.y);
			directionToTarget.Unitize();

			RE::NiPoint2 forwardVector(0.f, 1.f);
			RE::NiPoint2 currentCharacterDirection = Vec2Rotate(forwardVector, playerCharacter->data.angle.z);

			angleToTarget = GetAngle(currentCharacterDirection, directionToTarget);

			ret = &angleToTarget;
		} else {
			ret = _GetHorseCameraFreeRotationYaw(a_this);
		}
		
		directionalMovementHandler->SetCurrentHorseAimAngle(*ret);

		return ret;
	}

	/*void HorseAimHook::GetMovementAgentPosition(RE::Actor* a_this, RE::NiPoint3& a_pos)
	{
		_GetMovementAgentPosition(a_this, a_pos);

		a_pos.z += 100.f;
	}*/

	void HorseAimHook::Func(RE::PlayerCamera* a_this)
	{	
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		if (DirectionalMovementHandler::GetSingleton()->HasTargetLocked() && playerCharacter && playerCharacter->IsOnMount()) {
			return;
		}

		_Func(a_this);
	}

	float HorseAimHook::GetYaw(RE::Actor* a_this)
	{
		// originally it returns the value of AimHeadingCurrent graph variable minus the character yaw
		[[maybe_unused]] float original = _GetYaw(a_this);
		RE::ActorPtr mount;
		float angleOffset = 0.f;
		if (a_this->GetMount(mount)) {
			angleOffset = mount->data.angle.z;
		}
		return DirectionalMovementHandler::GetSingleton()->GetCurrentHorseAimAngle() + angleOffset;
	}

	//void CrosshairPickHook::Pick(uintptr_t a_this, RE::bhkWorld* a_bhkWorld, RE::NiPoint3& a_sourcePoint, RE::NiPoint3& a_sourceRotation)
	//{
	//	if (DirectionalMovementHandler::GetSingleton()->IsFreeCamera()) {
	//		auto playerCamera = RE::PlayerCamera::GetSingleton();
	//		
	//		RE::NiPoint3 playerPos = a_sourcePoint;

	//		// Set the source point to camera location instead of player location
	//		playerCamera->currentState->GetTranslation(a_sourcePoint);

	//		// Increase the pick length temporarily by the distance between player and camera
	//		float originalPickLength = *g_fActivatePickLength;
	//		*g_fActivatePickLength += playerPos.GetDistance(a_sourcePoint);

	//		_Pick(a_this, a_bhkWorld, a_sourcePoint, a_sourceRotation);

	//		// Restore original value
	//		*g_fActivatePickLength = originalPickLength;
	//	} else {
	//		_Pick(a_this, a_bhkWorld, a_sourcePoint, a_sourceRotation);
	//	}
	//}

	

}
