#pragma once
#include <unordered_set>
#include "SmoothCamAPI.h"

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
	enum TargetLockProjectileAimType : std::uint32_t
	{
		kFreeAim = 0,
		kPredict = 1,
		kHoming = 2
	};

	enum DialogueMode : std::uint32_t
	{
		kDisable = 0,
		kNormal = 1,
		kFaceSpeaker = 2
	};

	enum AttackState : std::uint8_t
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
	void ProgressTimers();

	void HideCrosshair();
	void ShowCrosshair();

	bool ProcessInput(RE::NiPoint2& a_inputDirection, RE::PlayerControlsData* a_playerControlsData);
	void SetDesiredAngleToTarget(RE::PlayerCharacter* a_playerCharacter, RE::ActorHandle a_target);
	void UpdateRotation();
	void UpdateRotationLockedCam();
	void UpdateTweeningState();
	void UpdateAIProcessRotationSpeed(RE::Actor* a_actor);
	void SetDesiredAIProcessRotationSpeed(float a_rotationSpeed);
	
	bool IsIFPV() const;
	bool IsImprovedCamera() const;

	bool IsFreeCamera() const;
	bool GetFreeCameraEnabled() const;
	bool HasMovementInput() const;

	bool IsDodging() const;

	bool IsPlayerAnimationDriven() const;

	AttackState GetAttackState() const;
	void SetAttackState(AttackState a_state);

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

	enum : std::uint32_t
	{
		kSort_Distance = 0,		// descending order of distance from actor
		kSort_Crosshair = 1,	// descending order of distance from crosshair
		kSort_ZAxisClock = 2,	// z axis clockwise
		kSort_ZAxisRClock = 3,	// z axis counterclockwise
		kSort_Invalid = 4
	};

	void ToggleTargetLock(bool bEnable);
	RE::ActorHandle GetTarget();
	void ClearTargets();

	void OverrideControllerBufferDepth(bool a_override);

	bool CheckCurrentTarget(RE::ActorHandle a_target, bool bInstantLOS = false);
	void UpdateTargetLock();
	void CheckBosses();

	bool IsActorValidTarget(RE::ActorPtr a_actor, bool a_bCheckDistance = false) const;

	std::vector<RE::ActorHandle> FindCloseActor(float a_distance, uint32_t a_sortOrder);
	RE::ActorHandle FindTarget(float a_distance);
	RE::ActorHandle FindNextTarget(float a_distance, bool bRight);
	RE::ActorHandle FindClosestTarget(float a_distance);

	void SetTarget(RE::ActorHandle a_target);
	void SetSoftTarget(RE::ActorHandle a_softTarget);

	void AddBoss(RE::ActorHandle a_boss);
	void RemoveBoss(RE::ActorHandle a_boss, bool a_bBossDied);

	void SwitchTarget(Directions a_direction);
	
	void SetHeadtrackTarget(RE::TESObjectREFR* a_target);

	void UpdateCameraHeadtracking();

	RE::NiPoint3 GetCameraRotation();

	void LookAtTarget(RE::ActorHandle a_target);

	bool ShouldFaceTarget() const;
	bool ShouldFaceCrosshair() const;

	bool HasTargetLocked() const;
	bool IsHeadtrackingEnabled() const;
	bool IsCameraHeadtrackingEnabled() const;

	float GetDialogueHeadtrackTimer() const;
	void RefreshDialogueHeadtrackTimer();

	bool GetDirectionalMovementSheathed() const;
	void SetDirectionalMovementSheathed(bool a_enable);
	bool GetDirectionalMovementDrawn() const;
	void SetDirectionalMovementDrawn(bool a_enable);
	DialogueMode GetDialogueMode();
	void SetDialogueMode(DialogueMode a_mode);
	bool GetHeadtracking() const;
	void SetHeadtracking(bool a_enable);
	float GetDialogueHeadtrackingDuration() const;
	void SetDialogueHeadtrackingDuration(float a_duration);
	bool GetCameraHeadtracking() const;
	void SetCameraHeadtracking(bool a_enable);
	float GetCameraHeadtrackingStrength() const;
	void SetCameraHeadtrackingStrength(float a_strength);
	bool GetStopCameraHeadtrackingBehindPlayer() const;
	void SetStopCameraHeadtrackingBehindPlayer(bool a_enable);
	bool GetFaceCrosshairWhileAttacking() const;
	void SetFaceCrosshairWhileAttacking(bool a_enable);
	bool GetFaceCrosshairWhileBlocking() const;
	void SetFaceCrosshairWhileBlocking(bool a_enable);
	bool GetFaceCrosshairDuringAutoMove() const;
	void SetFaceCrosshairDuringAutoMove(bool a_enable);
	float GetRunningRotationSpeedMult() const;
	void SetRunningRotationSpeedMult(float a_mult);
	float GetSprintingRotationSpeedMult() const;
	void SetSprintingRotationSpeedMult(float a_mult);
	float GetAttackStartRotationSpeedMult() const;
	void SetAttackStartRotationSpeedMult(float a_mult);
	float GetAttackMidRotationSpeedMult() const;
	void SetAttackMidRotationSpeedMult(float a_mult);
	float GetAttackEndRotationSpeedMult() const;
	void SetAttackEndRotationSpeedMult(float a_mult);
	float GetAirRotationSpeedMult() const;
	void SetAirRotationSpeedMult(float a_mult);
	bool GetDisableAttackRotationMultipliersForTransformations() const;
	void SetDisableAttackRotationMultipliersForTransformations(bool a_enable);
	bool GetStopOnDirectionChange() const;
	void SetStopOnDirectionChange(bool a_enable);

	float GetTargetLockDistance() const;
	void SetTargetLockDistance(float a_distance);
	float GetTargetLockPitchAdjustSpeed() const;
	void SetTargetLockPitchAdjustSpeed(float a_speed);
	float GetTargetLockYawAdjustSpeed() const;
	void SetTargetLockYawAdjustSpeed(float a_speed);
	float GetTargetLockPitchOffsetStrength() const;
	void SetTargetLockPitchOffsetStrength(float a_offsetStrength);
	bool GetTargetLockUseMouse() const;
	void SetTargetLockUseMouse(bool a_enable);
	bool GetTargetLockUseScrollWheel() const;
	void SetTargetLockUseScrollWheel(bool a_enable);
	bool GetTargetLockUseRightThumbstick() const;
	void SetTargetLockUseRightThumbstick(bool a_enable);
	TargetLockProjectileAimType GetTargetLockArrowAimType();
	void SetTargetLockArrowAimType(TargetLockProjectileAimType a_type);
	TargetLockProjectileAimType GetTargetLockMissileAimType();
	void SetTargetLockMissileAimType(TargetLockProjectileAimType a_type);
	bool GetAutoTargetNextOnDeath() const;
	void SetAutoTargetNextOnDeath(bool a_enable);
	bool GetTargetLockTestLOS() const;
	void SetTargetLockTestLOS(bool a_enable);
	bool GetTargetLockHostileActorsOnly() const;
	void SetTargetLockHostileActorsOnly(bool a_enable);
	bool GetTargetLockHideCrosshair() const;
	void SetTargetLockHideCrosshair(bool a_hide);

	bool Save(const SKSE::SerializationInterface* a_intfc, std::uint32_t a_typeCode, std::uint32_t a_version);
	bool Load(const SKSE::SerializationInterface* a_intfc);
	void Clear();
	void Initialize();
	void OnLoad();

	void LoadIniSettings();

	void InitCameraModsCompatibility();

	static bool IsBehaviorPatchInstalled(RE::TESObjectREFR* a_ref);

	bool GetPlayerIsNPC() const;
	void SetPlayerIsNPC(bool a_enable);

	void UpdatePlayerPitch();

	inline auto& GetBossRaces() const
	{
		return _bossRaces;
	}

	inline auto& GetBossNPCs() const
	{
		return _bossNPCs;
	}

	inline auto& GetBossLocRefTypes() const
	{
		return _bossLocRefTypes;
	}

	inline auto& GetBossNPCBlacklist() const
	{
		return _bossNPCBlacklist;
	}

	SmoothCamAPI::IVSmoothCam1* g_SmoothCam = nullptr;

private:
	using Lock = std::recursive_mutex;
	using Locker = std::lock_guard<Lock>;

	DirectionalMovementHandler();
	DirectionalMovementHandler(const DirectionalMovementHandler&) = delete;
	DirectionalMovementHandler(DirectionalMovementHandler&&) = delete;
	~DirectionalMovementHandler() = default;

	DirectionalMovementHandler& operator=(const DirectionalMovementHandler&) = delete;
	DirectionalMovementHandler& operator=(DirectionalMovementHandler&&) = delete;

	static constexpr bool DF_DIRECTIONALMOVEMENTSHEATHED = true;
	static constexpr bool DF_DIRECTIONALMOVEMENTDRAWN = true;
	static constexpr DialogueMode DF_DIALOGUEMODE = kFaceSpeaker;
	static constexpr bool DF_HEADTRACKING = true;
	static constexpr float DF_DIALOGUEHEADTRACKINGDURATION = 3.0f;
	static constexpr bool DF_CAMERAHEADTRACKING = true;
	static constexpr float DF_CAMERAHEADTRACKINGSTRENGTH = 0.5f;
	static constexpr bool DF_STOPCAMERAHEADTRACKINGBEHINDPLAYER = true;
	static constexpr bool DF_FACECROSSHAIRWHILEATTACKING = false;
	static constexpr bool DF_FACECROSSHAIRWHILEBLOCKING = true;
	static constexpr bool DF_FACECROSSHAIRDURINGAUTOMOVE = true;
	static constexpr float DF_RUNNINGROTATIONSPEEDMULT = 1.5f;
	static constexpr float DF_SPRINTINGROTATIONSPEEDMULT = 2.f;
	static constexpr float DF_ATTACKSTARTROTATIONSPEEDMULT = 5.f;
	static constexpr float DF_ATTACKMIDROTATIONSPEEDMULT = 1.f;
	static constexpr float DF_ATTACKENDROTATIONSPEEDMULT = 0.f;
	static constexpr float DF_AIRROTATIONSPEEDMULT = 0.5f;
	static constexpr bool DF_DISABLEATTACKROTATIONMULTIPLIERSFORTRANSFORMATIONS = true; 
	static constexpr bool DF_STOPONDIRECTIONCHANGE = true;

	static constexpr float DF_TARGETLOCKDISTANCE = 2000.f;
	static constexpr float DF_TARGETLOCKPITCHADJUSTSPEED = 2.f;
	static constexpr float DF_TARGETLOCKYAWADJUSTSPEED = 8.f;
	static constexpr float DF_TARGETLOCKPITCHOFFSETSTRENGTH = 0.25f;
	static constexpr bool DF_TARGETLOCKUSEMOUSE = true;
	static constexpr bool DF_TARGETLOCKUSESCROLLWHEEL = true;
	static constexpr bool DF_TARGETLOCKUSERIGHTTHUMBSTICK = true;
	static constexpr TargetLockProjectileAimType DF_TARGETLOCKARROWAIMTYPE = kPredict;
	static constexpr TargetLockProjectileAimType DF_TARGETLOCKMISSILEAIMTYPE = kPredict;
	static constexpr bool DF_AUTOTARGETNEXTONDEATH = true;
	static constexpr bool DF_TARGETLOCKTESTLOS = true;
	static constexpr bool DF_TARGETLOCKHOSTILEACTORSONLY = true;
	static constexpr bool DF_TARGETLOCKHIDECROSSHAIR = true;

	mutable Lock _lock;

	bool _bDirectionalMovementSheathed = true;
	bool _bDirectionalMovementDrawn = true;
	DialogueMode _dialogueMode = kFaceSpeaker;
	bool _bHeadtracking = true;
	float _dialogueHeadtrackingDuration = 3.0f;
	bool _bCameraHeadtracking = true;
	float _cameraHeadtrackingStrength = 0.5f;
	bool _bStopCameraHeadtrackingBehindPlayer = true;

	bool _bFaceCrosshairWhileAttacking = false;
	bool _bFaceCrosshairWhileBlocking = true;
	bool _bFaceCrosshairDuringAutoMove = true;
	float _runningRotationSpeedMult = 1.5f;
	float _sprintingRotationSpeedMult = 2.f;
	float _attackStartRotationSpeedMult = 5.f;
	float _attackMidRotationSpeedMult = 1.f;
	float _attackEndRotationSpeedMult = 0.f;
	float _airRotationSpeedMult = 0.5f;
	bool _bDisableAttackRotationMultipliersForTransformations = true;
	bool _bStopOnDirectionChange = true;

	float _targetLockDistance = 2000.f;
	float _targetLockPitchAdjustSpeed = 2.f;
	float _targetLockYawAdjustSpeed = 8.f; 
	float _targetLockPitchOffsetStrength = 0.25f;
	bool _bTargetLockUseMouse = true;
	bool _bTargetLockUseScrollWheel = true;
	bool _bTargetLockUseRightThumbstick = true;
	TargetLockProjectileAimType _targetLockArrowAimType = kPredict;
	TargetLockProjectileAimType _targetLockMissileAimType = kPredict;
	bool _bAutoTargetNextOnDeath = true;
	bool _bTargetLockTestLOS = true;
	bool _bTargetLockHostileActorsOnly = true;
	bool _bTargetLockHideCrosshair = true;
		
	float _freecamControllerBufferDepth = 0.02f;
	float _defaultControllerBufferDepth = -1.f;
	float _targetLockDistanceHysteresis = 1.05f;

	RE::TESGlobal* _IFPV_IsFirstPerson = nullptr;
	bool* _ImprovedCamera_IsThirdPerson = nullptr;

	RE::SpellItem* _targetLockSpell = nullptr;
	RE::TESGlobal* _directionalMovementGlobal = nullptr;

	float _desiredAngle = -1.f;

	bool _bDirectionalMovement = false;
	bool _bShouldFaceCrosshair = false;
	bool _bShouldFaceTarget = false;

	bool _bUpdatePlayerPitch = false;
	float _desiredPlayerPitch;

	bool _bIsTweening = false;
	float _yawDelta = 0.f;
	
	bool _bTargetLock = false;
	float _desiredAIProcessRotationSpeed = 0.f;
	Directions _lastTargetSwitchDirection = Directions::kInvalid;
	float _lastTargetSwitchTimer = 0.f;
	float _lastLOSTimer = 0.f;
	float _dialogueHeadtrackTimer = 0.f;
	float _faceCrosshairTimer = 0.f;

	bool _bCrosshairIsHidden = false;
	bool _bAiming = false;

	static constexpr float faceCrosshairDuration = 0.2f;

	bool _playerIsNPC = false;

	bool _bHasMovementInput = false;
	bool _bIsDodging = false;
	AttackState _attackState;

	RE::ActorHandle _target;
	RE::ActorHandle _softTarget;
	RE::ObjectRefHandle _dialogueSpeaker;

	std::unordered_set<RE::ActorHandle> _bossTargets;

	std::unordered_set<RE::TESRace*> _bossRaces;
	std::unordered_set<RE::BGSLocationRefType*> _bossLocRefTypes;
	std::unordered_set<RE::TESNPC*> _bossNPCs;
	std::unordered_set<RE::TESNPC*> _bossNPCBlacklist;
};
