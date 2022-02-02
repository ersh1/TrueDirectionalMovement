#pragma once
#include "SmoothCamAPI.h"
#include "TrueHUDAPI.h"
#include "Widgets/TargetLockReticle.h"

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
}

class DirectionalMovementHandler
{
public:
	enum class AttackState : std::uint8_t
	{
		kNone = 0,
		kStart = 1,
		kMid = 2,
		kTracing = 3, // Melee Tracing compatibility
		kEnd = 4
	};

	static DirectionalMovementHandler* GetSingleton();

	static void ResetControls();

	void Update();
	void UpdateDirectionalMovement();
	void UpdateFacingState();
	void UpdateFacingCrosshair();
	void UpdateDodgingState();
	void UpdateSwimmingPitchOffset();
	void ProgressTimers();

	void UpdateLeaning();

	void UpdateCameraAutoRotation();
	void ResetCameraRotationDelay();

	bool IsCrosshairVisible() const;
	void HideCrosshair();
	void ShowCrosshair();

	bool IsAiming() const;
	void SetIsAiming(bool a_bIsAiming);

	bool ProcessInput(RE::NiPoint2& a_inputDirection, RE::PlayerControlsData* a_playerControlsData);
	void SetDesiredAngleToTarget(RE::PlayerCharacter* a_playerCharacter, RE::ActorHandle a_target);
	void UpdateRotation();
	void UpdateRotationLockedCam();
	void UpdateTweeningState();
	void UpdateAIProcessRotationSpeed(RE::Actor* a_actor);
	void SetDesiredAIProcessRotationSpeed(float a_rotationSpeed);
	
	bool IFPV_IsFirstPerson() const;
	bool ImprovedCamera_IsFirstPerson() const;
	bool IsImprovedCameraInstalled() const;

	bool IsFreeCamera() const;
	bool GetFreeCameraEnabled() const;
	bool HasMovementInput() const;

	bool IsDodging() const;
	bool IsMagnetismActive() const;

	bool IsPlayerAnimationDriven() const;

	AttackState GetAttackState() const;
	void SetAttackState(AttackState a_state);

	bool IsCameraResetting() const;
	void ResetCamera();

	void ResetDesiredAngle();

	float GetYawDelta() const;
	void ResetYawDelta();


	enum class Directions
	{
		kInvalid = 0,
		kLeft = 1 << 0,
		kRight = 1 << 1,
		kForward = 1 << 2,
		kBack = 1 << 3,
		kUp = 1 << 4,
		kDown = 1 << 5
	};

	SKSE::stl::enumeration<Directions, std::uint8_t> _pressedDirections;

	enum TargetSortOrder : std::uint32_t
	{
		kSort_CameraDistance = 0,					// descending order of distance from camera
		kSort_CharacterDistanceAndCrosshair = 1,    // descending order of distance from character and crosshair
		kSort_Crosshair = 2,						// descending order of distance from crosshair
		kSort_Combined = 3,                         // descending order of combined angle to character and distance and crosshair
		kSort_ZAxisClock = 4,						// z axis clockwise
		kSort_ZAxisRClock = 5,						// z axis counterclockwise
		kSort_Invalid = 6
	};

	bool ToggleTargetLock(bool bEnable, bool bPressedManually = false);
	RE::ActorHandle GetTarget();
	void ClearTargets();

	void OverrideControllerBufferDepth(bool a_override);

	float GetTargetLockDistanceRaceSizeMultiplier(RE::TESRace* a_race) const;
	bool CheckCurrentTarget(RE::ActorHandle a_target, bool bInstantLOS = false);
	void UpdateTargetLock();

	bool IsActorValidTarget(RE::ActorPtr a_actor, bool a_bCheckDistance = false) const;

	std::vector<RE::ActorHandle> FindCloseActor(float a_distance, TargetSortOrder a_sortOrder);
	RE::ActorHandle FindTarget(float a_distance, TargetSortOrder a_sortOrder = kSort_Crosshair);
	RE::ActorHandle FindNextTarget(float a_distance, bool bRight);
	RE::ActorHandle FindClosestTarget(float a_distance);

	bool SetDesiredAngleToMagnetismTarget();
	
	float GetCurrentSwimmingPitchOffset() const;
	void SetDesiredSwimmingPitchOffset(float a_value);

	void SetTarget(RE::ActorHandle a_target);
	void SetSoftTarget(RE::ActorHandle a_softTarget);

	void AddTargetLockReticle(RE::ActorHandle a_target);
	void ReticleRemoved();
	void RemoveTargetLockReticle();

	void SwitchTarget(Directions a_direction);
	
	void SetHeadtrackTarget(int32_t a_headtrackPriority, RE::TESObjectREFR* a_target);

	void UpdateCameraHeadtracking();

	void SetPreviousHorseAimAngle(float a_angle);
	void SetCurrentHorseAimAngle(float a_angle);
	void UpdateHorseAimDirection();
	void SetNewHorseAimDirection(float a_angle);
	float GetCurrentHorseAimAngle() const;

	void SetLastInputDirection(RE::NiPoint2& a_inputDirection);
	bool CheckInputDot(float a_dot) const;
	bool DetectInputAnalogStickBounce() const;

	void SetCameraStateBeforeTween(RE::CameraStates::CameraState a_cameraState);

	RE::NiPoint3 GetCameraRotation();

	void LookAtTarget(RE::ActorHandle a_target);

	bool ShouldFaceTarget() const;
	bool ShouldFaceCrosshair() const;

	bool HasTargetLocked() const;

	float GetDialogueHeadtrackTimer() const;
	void RefreshDialogueHeadtrackTimer();
	float GetCameraHeadtrackTimer() const;
	void RefreshCameraHeadtrackTimer();

	void Initialize();
	void OnPreLoadGame();

	void OnSettingsUpdated();	

	void InitCameraModsCompatibility();

	static bool IsBehaviorPatchInstalled(RE::TESObjectREFR* a_ref);

	bool GetPlayerIsNPC() const;
	void SetPlayerIsNPC(bool a_enable);

	void UpdatePlayerPitch();	

	SmoothCamAPI::IVSmoothCam3* g_SmoothCam = nullptr;
	TRUEHUD_API::IVTrueHUD1* g_trueHUD = nullptr;
	std::atomic_bool _bReticleRemoved{ false };

	bool GetForceDisableDirectionalMovement() const;
	bool GetForceDisableHeadtracking() const;
	void SetForceDisableDirectionalMovement(bool a_disable);
	void SetForceDisableHeadtracking(bool a_disable);

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

	struct LeanAmount
	{
		float LR;
		float FB;
	} _leanAmount;

	RE::NiPoint3 _previousVelocity;
		
	float _freecamControllerBufferDepth = 0.02f;
	float _defaultControllerBufferDepth = -1.f;
	
	bool _bMagnetismActive = false;
	bool _bCurrentlyTurningToCrosshair = false;

	float _desiredAngle = -1.f;

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
	Directions _lastTargetSwitchDirection = Directions::kInvalid;

	float _previousHorseAimAngle = 0.f;
	float _horseAimAngle = 0.f;
	Directions _currentHorseAimDirection = Directions::kForward;

	RE::CameraStates::CameraState _cameraStateBeforeTween;

	// for analog bounce fix
	static constexpr float _analogBounceDotThreshold = 0.25f;
	static constexpr size_t _inputBufferSize = 5;
	std::deque<RE::NiPoint2> _lastInputs;

	float _lastTargetSwitchTimer = 0.f;
	float _lastLOSTimer = 0.f;
	float _dialogueHeadtrackTimer = 0.f;
	float _faceCrosshairTimer = 0.f;
	float _cameraHeadtrackTimer = 0.f;
	float _cameraRotationDelayTimer = 0.f;
	float _tutorialHintTimer = 0.f;

	bool _bCrosshairIsHidden = false;
	bool _bIsAiming = false;

	float _desiredSwimmingPitchOffset = 0.f;
	float _currentSwimmingPitchOffset = 0.f;

	float _currentAutoCameraRotationSpeed = 0.f;
	
	static constexpr float _meleeMagnetismRange = 250.f;
	static constexpr float _faceCrosshairDuration = 0.4f;
	static constexpr float _targetLockDistanceHysteresis = 1.05f;
	static constexpr float _cameraAutoRotationAngleDeadzone = 0.2f;
	static constexpr float _hintDuration = 5.f;

	static constexpr float _leanInterpSpeed = 4.f;
	//static constexpr size_t _velocityBufferSize = 10;
	//std::deque<RE::NiPoint3> _velocityBuffer;

	bool _playerIsNPC = false;

	bool _bHasMovementInput = false;
	bool _bIsDodging = false;
	bool _bJustDodged = false;
	AttackState _attackState;

	bool _bForceDisableDirectionalMovement = false;
	bool _bForceDisableHeadtracking = false;

	RE::ActorHandle _target;
	RE::ActorHandle _softTarget;
	RE::ObjectRefHandle _dialogueSpeaker;

	// Compatibility
	RE::TESGlobal* _IFPV_IsFirstPerson = nullptr;
	bool* _ImprovedCamera_IsFirstPerson = nullptr;
	bool _bACCInstalled = false;
	bool _bControlsTrueHUDTarget = false;

	std::weak_ptr<Scaleform::TargetLockReticle> _targetLockReticle;
};
