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
	void UpdateFacingCrosshair();
	void UpdateDodgingState();
	void UpdateSwimmingPitchOffset();
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

	enum TargetSortOrder : std::uint32_t
	{
		kSort_CameraDistance = 0,					// descending order of distance from camera
		kSort_CharacterDistanceAndCrosshair = 1,    // descending order of distance from character
		kSort_Crosshair = 2,						// descending order of distance from crosshair
		kSort_ZAxisClock = 3,						// z axis clockwise
		kSort_ZAxisRClock = 4,						// z axis counterclockwise
		kSort_Invalid = 5
	};

	bool ToggleTargetLock(bool bEnable);
	RE::ActorHandle GetTarget();
	void ClearTargets();

	void OverrideControllerBufferDepth(bool a_override);

	float GetTargetLockDistanceRaceSizeMultiplier(RE::TESRace* a_race) const;
	bool CheckCurrentTarget(RE::ActorHandle a_target, bool bInstantLOS = false);
	void UpdateTargetLock();
	void CheckBosses();

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

	float GetDialogueHeadtrackTimer() const;
	void RefreshDialogueHeadtrackTimer();

	void Initialize();
	void OnPreLoadGame();

	void OnSettingsUpdated();

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

	mutable Lock _lock;
		
	float _freecamControllerBufferDepth = 0.02f;
	float _defaultControllerBufferDepth = -1.f;
	
	bool _bMagnetismActive = false;
	bool _bCurrentlyTurningToCrosshair = false;

	RE::TESGlobal* _IFPV_IsFirstPerson = nullptr;
	bool* _ImprovedCamera_IsThirdPerson = nullptr;
	bool _bACCInstalled = false;

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

	float _desiredSwimmingPitchOffset = 0.f;
	float _currentSwimmingPitchOffset = 0.f;
	
	static constexpr float _meleeMagnetismRange = 250.f;
	static constexpr float _faceCrosshairDuration = 0.4f;
	static constexpr float _targetLockDistanceHysteresis = 1.05f;

	bool _playerIsNPC = false;

	bool _bHasMovementInput = false;
	bool _bIsDodging = false;
	bool _bJustDodged = false;
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
