#pragma once
#include "Utils.h"
#include "API/APIManager.h"
#include "Widgets/TargetLockReticle.h"
#include <unordered_set>

namespace std
{
	template <>
	struct hash<RE::ActorHandle>
	{
		uint32_t operator()(const RE::ActorHandle& a_handle) const
		{
			uint32_t nativeHandle = const_cast<RE::ActorHandle*>(&a_handle)->native_handle();  // ugh
			return nativeHandle;
		}
	};

	template <>
	struct hash<RE::ObjectRefHandle>
	{
		uint32_t operator()(const RE::ObjectRefHandle& a_handle) const
		{
			uint32_t nativeHandle = const_cast<RE::ObjectRefHandle*>(&a_handle)->native_handle();
			return nativeHandle;
		}
	};
}

class DirectionalMovementHandler :
	public RE::BSTEventSink<RE::BSAnimationGraphEvent>
{
public:
	enum class AttackState : std::uint8_t
	{
		kNone = 0,
		kStart = 1,
		kMid = 2,
		kEnd = 3
	};

	using EventResult = RE::BSEventNotifyControl;

	static DirectionalMovementHandler* GetSingleton();
	static void Register();

	// override BSTEventSink
	virtual EventResult ProcessEvent(const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_eventSource) override;

	static void ResetControls();

	void Update();
	void UpdateDirectionalMovement();
	void UpdateFacingState();
	void UpdateFacingCrosshair();
	void UpdateDodgingState();
	void UpdateJumpingState();
	void UpdateSwimmingPitchOffset();
	void UpdateMountedArchery();
	void ProgressTimers();

	void OnDodge();
	
	void UpdateProjectileTargetMap();

	void UpdateLeaning(RE::Actor* a_actor, float a_deltaTime);

	void UpdateCameraAutoRotation();
	void ResetCameraRotationDelay() { _cameraRotationDelayTimer = Settings::fCameraAutoAdjustDelay; }

	bool IsCrosshairVisible() const;
	void HideCrosshair();
	void ShowCrosshair();

	bool IsAiming() const { return _bIsAiming; }
	void SetIsAiming(bool a_bIsAiming);

	bool ProcessInput(RE::NiPoint2& a_inputDirection, RE::PlayerControlsData* a_playerControlsData);
	void SetDesiredAngleToTarget(RE::PlayerCharacter* a_playerCharacter, RE::ActorHandle a_target);
	void UpdateRotation(bool bForceInstant = false);
	void UpdateRotationLockedCam();
	void UpdateTweeningState();
	void UpdateAIProcessRotationSpeed(RE::Actor* a_actor);
	void SetDesiredAIProcessRotationSpeed(float a_rotationSpeed) { _desiredAIProcessRotationSpeed = a_rotationSpeed; }
	
	bool IFPV_IsFirstPerson() const;
	bool ImprovedCamera_IsFirstPerson() const;
	bool IsImprovedCameraInstalled() const;

	TDM_API::DirectionalMovementMode GetDirectionalMovementMode() const;
	bool IsFreeCamera() const { return _bDirectionalMovement; }
	bool Is360Movement() const { return _bDirectionalMovement && !_bShouldFaceCrosshair && !_bCurrentlyTurningToCrosshair; }
	bool GetFreeCameraEnabled() const;
	bool HasMovementInput() const { return _bHasMovementInput; }

	bool IsDodging() const { return _DF_bIsDodging || _bIsDodging_Legacy; }
	bool IsMagnetismActive() const { return _bMagnetismActive; }

	bool IsPlayerAIDriven() const;
	bool IsPlayerAnimationDriven() const;
	bool IsTDMRotationLocked() const;

	AttackState GetAttackState() const { return _attackState; }
	void SetAttackState(AttackState a_state) { _attackState = a_state; }

	bool IsCameraResetting() const { return _bResetCamera; }
	void ResetCamera();

	void ResetDesiredAngle() { _desiredAngle = -1.f; }

	float GetYawDelta() const { return _yawDelta; }
	void ResetYawDelta() { _yawDelta = 0.f; }

	RE::NiPoint2 GetActualInputDirection() const { return _actualInputDirection; }

	enum class Direction
	{
		kInvalid = 0,
		kLeft = 1 << 0,
		kRight = 1 << 1,
		kForward = 1 << 2,
		kBack = 1 << 3,
		kUp = 1 << 4,
		kDown = 1 << 5
	};

	enum class TargetLockSelectionMode : std::uint32_t
	{
		kClosest = 0,
		kCenter = 1,
		kCombined = 2
	};

	SKSE::stl::enumeration<Direction, std::uint8_t> _pressedDirections;

	bool ToggleTargetLock(bool bEnable, bool bPressedManually = false);
	RE::ActorHandle GetTarget() const;
	RE::NiPointer<RE::NiAVObject> GetTargetPoint() const;
	RE::NiPoint3 GetTargetPosition() const;
	void ClearTargets();

	void OverrideControllerBufferDepth(bool a_override);

	float GetTargetLockDistanceRaceSizeMultiplier(RE::TESRace* a_race) const;
	bool CheckCurrentTarget(RE::ActorHandle a_target, bool bInstantLOS = false);
	void UpdateTargetLock();

	bool IsActorValidTarget(RE::ActorPtr a_actor, bool a_bCheckDistance = false) const;

	RE::ActorHandle FindTarget(TargetLockSelectionMode a_mode, bool a_bSkipCurrent = true);
	void SwitchTarget(Direction a_direction);
	bool SwitchTargetPoint(Direction a_direction);
	RE::ActorHandle SwitchScreenTarget(Direction a_direction);
	std::vector<RE::NiPointer<RE::NiAVObject>> GetTargetPoints(RE::ActorHandle a_actorHandle) const;
	RE::NiPointer<RE::NiAVObject> GetBestTargetPoint(RE::ActorHandle a_actorHandle) const;

	bool SetDesiredAngleToMagnetismTarget();
	
	float GetCurrentSwimmingPitchOffset() const { return _currentSwimmingPitchOffset; }
	void SetDesiredSwimmingPitchOffset(float a_value) { _desiredSwimmingPitchOffset = a_value; }

	void SetTarget(RE::ActorHandle a_target);
	void SetSoftTarget(RE::ActorHandle a_softTarget);
	void SetTargetPoint(RE::NiPointer<RE::NiAVObject> a_targetPoint) { _currentTargetPoint = a_targetPoint; }

	RE::NiAVObject* GetProjectileTargetPoint(RE::ObjectRefHandle a_projectileHandle) const;
	void AddProjectileTarget(RE::ObjectRefHandle a_projectileHandle, RE::NiPointer<RE::NiAVObject> a_targetPoint);
	void RemoveProjectileTarget(RE::ObjectRefHandle a_projectileHandle);

	void AddTargetLockReticle(RE::ActorHandle a_target, RE::NiPointer<RE::NiAVObject> a_targetPoint);
	void ReticleRemoved();
	void RemoveTargetLockReticle();
	
	void SetHeadtrackTarget(RE::HighProcessData::HEAD_TRACK_TYPE a_headtrackType, RE::TESObjectREFR* a_target);

	void UpdateCameraHeadtracking();

	void SetPreviousHorseAimAngle(float a_angle) { _previousHorseAimAngle = a_angle; }
	void SetCurrentHorseAimAngle(float a_angle);
	bool GetCurrentlyMountedAiming() const { return _currentlyMountedAiming; }
	void SetCurrentlyMountedAiming(bool a_aiming) { _currentlyMountedAiming = a_aiming; }
	void UpdateHorseAimDirection();
	void SetNewHorseAimDirection(float a_angle);
	float GetCurrentHorseAimAngle() const { return _horseAimAngle; }

	void SetLastInputDirection(RE::NiPoint2& a_inputDirection);
	bool CheckInputDot(float a_dot) const { return a_dot < _analogBounceDotThreshold; }
	bool DetectInputAnalogStickBounce() const;

	void SetCameraStateBeforeTween(RE::CameraStates::CameraState a_cameraState) { _cameraStateBeforeTween = a_cameraState; }

	RE::NiPoint3 GetCameraRotation();

	void LookAtTarget(RE::ActorHandle a_target);

	bool ShouldFaceTarget() const { return _bShouldFaceTarget; }
	bool ShouldFaceCrosshair() const { return _bShouldFaceCrosshair; }

	bool HasTargetLocked() const { return static_cast<bool>(_target); }

	float GetDialogueHeadtrackTimer() const { return _dialogueHeadtrackTimer; }
	void RefreshDialogueHeadtrackTimer() { _dialogueHeadtrackTimer = Settings::fDialogueHeadtrackingDuration; }
	float GetCameraHeadtrackTimer() const { return _cameraHeadtrackTimer; }
	void RefreshCameraHeadtrackTimer() { _cameraHeadtrackTimer = Settings::fCameraHeadtrackingDuration; }

	void Initialize();
	void OnPreLoadGame();

	void OnSettingsUpdated();	

	void InitCameraModsCompatibility();

	static bool IsBehaviorPatchInstalled(RE::TESObjectREFR* a_ref);
	static bool IsMountedArcheryPatchInstalled(RE::TESObjectREFR* a_ref);

	bool GetPlayerIsNPC() const { return _playerIsNPC; }
	void SetPlayerIsNPC(bool a_enable) { _playerIsNPC = a_enable; }

	void UpdatePlayerPitch();	

	std::atomic_bool _bReticleRemoved{ false };

	bool GetForceDisableDirectionalMovement() const { return _bForceDisableDirectionalMovement || !_papyrusDisableDirectionalMovement.empty(); }
	bool GetForceDisableHeadtracking() const { return _bForceDisableHeadtracking || !_papyrusDisableHeadtracking.empty(); }
	bool GetYawControl() const { return _bYawControlledByPlugin; }
	void SetForceDisableDirectionalMovement(bool a_disable) { _bForceDisableDirectionalMovement = a_disable; }
	void SetForceDisableHeadtracking(bool a_disable) { _bForceDisableHeadtracking = a_disable; }
	void SetYawControl(bool a_enable, float a_yawRotationSpeedMultiplier = 0);
	void SetPlayerYaw(float a_yaw) { _desiredAngle = NormalAbsoluteAngle(a_yaw); }

	void PapyrusDisableDirectionalMovement(std::string_view a_modName, bool a_bDisable);
	void PapyrusDisableHeadtracking(std::string_view a_modName, bool a_bDisable);

	bool IsACCInstalled() const { return _bACCInstalled; }
	bool IsICInstalled() const { return _bICInstalled; }

	bool HasDodgeRotationLock() const { return (_DF_bIsDodging && (!_DF_bUnlockRotation || !_DF_bUnlockRotationFull)) || _bIsDodging_Legacy; }

private:
	using Lock = std::recursive_mutex;
	using Locker = std::lock_guard<Lock>;

	DirectionalMovementHandler();
	DirectionalMovementHandler(const DirectionalMovementHandler&) = delete;
	DirectionalMovementHandler(DirectionalMovementHandler&&) = delete;
	~DirectionalMovementHandler() = default;

	DirectionalMovementHandler& operator=(const DirectionalMovementHandler&) = delete;
	DirectionalMovementHandler& operator=(DirectionalMovementHandler&&) = delete;

	mutable Lock _lock;

	float _defaultControllerBufferDepth = -1.f;
	float _defaultAcrobatics = -1.f;
	
	bool _bMagnetismActive = false;
	bool _bCurrentlyTurningToCrosshair = false;

	float _desiredAngle = -1.f;

	RE::NiPoint2 _actualInputDirection;

	bool _bDirectionalMovement = false;
	bool _bShouldFaceCrosshair = false;
	bool _bShouldFaceTarget = false;

	bool _bUpdatePlayerPitch = false;
	float _desiredPlayerPitch;

	bool _bResetCamera = false;
	float _desiredCameraAngleX;
	float _desiredCameraAngleY;		

	bool _bIsTweening = false;
	float _yawDelta = 0.f;
	
	float _desiredAIProcessRotationSpeed = 0.f;
	Direction _lastTargetSwitchDirection = Direction::kInvalid;

	float _previousHorseAimAngle = 0.f;
	float _horseAimAngle = 0.f;
	Direction _currentHorseAimDirection = Direction::kForward;
	bool _currentlyMountedAiming = false;

	RE::CameraStates::CameraState _cameraStateBeforeTween;

	// for analog bounce fix
	static constexpr float _analogBounceDotThreshold = 0.25f;
	static constexpr size_t _inputBufferSize = 5;
	std::deque<RE::NiPoint2> _lastInputs;

	RE::NiPoint2 _unalteredInputVec;

	float _lastTargetSwitchTimer = 0.f;
	float _lastLOSTimer = 0.f;
	float _dialogueHeadtrackTimer = 0.f;
	float _faceCrosshairTimer = 0.f;
	float _aimingTimer = 0.f;
	float _cameraHeadtrackTimer = 0.f;
	float _cameraRotationDelayTimer = 0.f;
	float _tutorialHintTimer = 0.f;

	bool _bCrosshairIsHidden = false;
	bool _bIsAiming = false;

	float _desiredSwimmingPitchOffset = 0.f;
	float _currentSwimmingPitchOffset = 0.f;

	float _currentAutoCameraRotationSpeed = 0.f;
	
	static constexpr float _lostSightAllowedDuration = 2.f;
	static constexpr float _meleeMagnetismRange = 250.f;
	static constexpr float _faceCrosshairDuration = 0.4f;
	static constexpr float _aimingDuration = 0.1f;
	static constexpr float _targetLockDistanceHysteresis = 1.05f;
	static constexpr float _hintDuration = 5.f;

	bool _playerIsNPC = false;

	bool _bHasMovementInput = false;

	bool _DF_bIsDodging = false;
	bool _DF_bUnlockRotation = false;
	bool _DF_bUnlockRotationFull = false;

	bool _bIsDodging_Legacy = false;
	bool _bJustDodged_Legacy = false;

	AttackState _attackState;

	bool _bForceDisableDirectionalMovement = false;
	std::unordered_set<std::string> _papyrusDisableDirectionalMovement{};
    bool _bForceDisableHeadtracking = false;
	std::unordered_set<std::string> _papyrusDisableHeadtracking{};
	bool _bYawControlledByPlugin = false;
	float _controlledYawRotationSpeedMultiplier = 0;

	RE::ActorHandle _target;
	RE::ActorHandle _softTarget;
	RE::ObjectRefHandle _dialogueSpeaker;
	RE::NiPointer<RE::NiAVObject> _currentTargetPoint;
	
	std::unordered_map<RE::ObjectRefHandle, RE::NiPointer<RE::NiAVObject>> _projectileTargets;

	// Compatibility
	RE::TESGlobal* _IFPV_IsFirstPerson = nullptr;
	bool* _ImprovedCamera_IsFirstPerson = nullptr;
	bool _bACCInstalled = false;
	bool _bICInstalled = false;
	bool _bControlsTrueHUDTarget = false;

	bool _mountedArcheryRequestedSmoothCamCrosshair = false;
	bool _targetLockRequestedSmoothCamCrosshair = false;

	std::weak_ptr<Scaleform::TargetLockReticle> _targetLockReticle;
};
