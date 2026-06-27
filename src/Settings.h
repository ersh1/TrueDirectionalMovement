#pragma once

enum DirectionalMovementMode : std::uint32_t
{
	kDisabled = 0,
	kVanilla = 1,
	kDirectional = 2
};

enum TargetLockProjectileAimType : std::uint32_t
{
	kFreeAim = 0,
	kPredict = 1,
	kHoming = 2
};

enum class DialogueMode : std::uint32_t
{
	kDisable = 0,
	kNormal = 1,
	kFaceSpeaker = 2
};

enum class CameraHeadtrackingMode : std::uint32_t
{
	kDisable = 0,
	kNormal = 1,
	kFaceCamera = 2
};

enum class CameraAdjustMode : std::uint32_t
{
	kDisable = 0,
	kDuringSprint = 1,
	kAlways = 2
};

enum class ReticleStyle : std::uint32_t
{
	kCrosshair = 0,
	kCrosshairNoTransform = 1,
	kDot = 2,
	kGlow = 3
};

enum class WidgetAnchor : std::uint32_t
{
	kBody = 0,
	kHead = 1,
};

struct Settings
{
	static void Initialize();
	static void ReadSettings();
	static void OnPostLoadGame();
	static void UpdateGlobals();

	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);
	static void ReadUInt32Setting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);

	// Directional Movement related
	static inline DirectionalMovementMode uDirectionalMovementSheathed = DirectionalMovementMode::kDirectional;
	static inline DirectionalMovementMode uDirectionalMovementDrawn = DirectionalMovementMode::kDirectional;
	static inline DialogueMode uDialogueMode = DialogueMode::kFaceSpeaker;
	static inline float fMeleeMagnetismAngle = 60.f;
	static inline bool bMagnetismWhileBlocking = true;
	static inline bool bFaceCrosshairWhileAttacking = false;
	static inline bool bFaceCrosshairWhileShouting = true;
	static inline bool bFaceCrosshairWhileBlocking = true;
	static inline bool bFaceCrosshairDuringAutoMove = true;
	static inline bool bStopOnDirectionChange = true;
	static inline CameraAdjustMode uAdjustCameraYawDuringMovement = CameraAdjustMode::kDuringSprint;
	static inline float fRunningRotationSpeedMult = 1.5f;
	static inline float fSprintingRotationSpeedMult = 2.f;
	static inline float fAttackStartRotationSpeedMult = 5.f;
	static inline float fAttackMidRotationSpeedMult = 1.f;
	static inline float fAttackEndRotationSpeedMult = 0.f;
	static inline float fAirRotationSpeedMult = 0.25f;
	static inline float fGlidingRotationSpeedMult = 0.5f;
	static inline float fWaterRotationSpeedMult = 0.5f;
	static inline float fSwimmingRotationSpeedMult = 0.5f;
	static inline float fDodgeUnlockedRotationSpeedMult = 0.5f;
	static inline float fFaceCrosshairRotationSpeedMultiplier = 2.f;
	static inline bool bFaceCrosshairInstantly = false;
	static inline float fCameraAutoAdjustDelay = 0.1f;
	static inline float fCameraAutoAdjustSpeedMult = 1.5f;
	static inline bool bIgnoreSlowTime = false;
	static inline bool bDisableAttackRotationMultipliersForTransformations = true;
	static inline float fSwimmingPitchSpeed = 3.f;
	static inline float fControllerBufferDepth = 0.02f;

	// Leaning
	static inline bool bEnableLeaning = true;
	static inline bool bEnableLeaningNPC = true;
	static inline float fLeaningMult = 2.f;
	static inline float fLeaningSpeed = 4.f;
	static inline float fMaxLeaningStrength = 10.f;

	// Headtracking
	static inline bool bHeadtracking = true;
	static inline bool bHeadtrackSpine = true;
	static inline float fDialogueHeadtrackingDuration = 3.0f;
	static inline bool bHeadtrackSoftTarget = true;
	static inline bool bCameraHeadtracking = true;
	static inline float fCameraHeadtrackingStrength = 0.75f;
	static inline float fCameraHeadtrackingDuration = 1.f;
	static inline CameraHeadtrackingMode uCameraHeadtrackingMode = CameraHeadtrackingMode::kDisable;

	// Target Lock
	static inline bool bAutoTargetNextOnDeath = true;
	static inline bool bTargetLockTestLOS = true;
	static inline bool bTargetLockHostileActorsOnly = true;
	static inline bool bTargetLockHideCrosshair = true;
	static inline float fTargetLockDistance = 2000.f;
	static inline float fTargetLockDistanceMultiplierSmall = 1.f;
	static inline float fTargetLockDistanceMultiplierLarge = 2.f;
	static inline float fTargetLockDistanceMultiplierExtraLarge = 4.f;
	static inline float fTargetLockPitchAdjustSpeed = 2.f;
	static inline float fTargetLockYawAdjustSpeed = 8.f;
	static inline float fTargetLockPitchOffsetStrength = 0.25f;
	static inline TargetLockProjectileAimType uTargetLockArrowAimType = TargetLockProjectileAimType::kPredict;
	static inline TargetLockProjectileAimType uTargetLockMissileAimType = TargetLockProjectileAimType::kPredict;
	static inline bool bTargetLockUsePOVSwitchKeyboard = false;
	static inline bool bTargetLockUsePOVSwitchGamepad = true;
	static inline float fTargetLockPOVHoldDuration = 0.25f;
	static inline bool bTargetLockUseMouse = true;
	static inline uint32_t uTargetLockMouseSensitivity = 32;
	static inline bool bTargetLockUseScrollWheel = true;
	static inline bool bTargetLockUseRightThumbstick = true;
	static inline bool bResetCameraWithTargetLock = true;
	static inline bool bResetCameraPitch = false;
	static inline bool bTargetLockConsiderGroundLevel = true;
	static inline float fTargetLockMinHeightAboveGround = 35.f;
	static inline bool bTargetLockEnableLockBehindTarget = false;
	static inline float fCameraBehindTargetMinDistance = 50.f;
	static inline float fCameraBehindTargetNoSwitchRange = 150.f;

	// HUD
	static inline bool bEnableTargetLockReticle = true;
	static inline WidgetAnchor uReticleAnchor = WidgetAnchor::kBody;
	static inline ReticleStyle uReticleStyle = ReticleStyle::kCrosshair;
	static inline float fReticleScale = 1.f;
	static inline bool bReticleUseHUDOpacity = true;
	static inline float fReticleOpacity = 1.f;

	// Misc
	static inline bool bOverrideAcrobatics = true;
	static inline float fAcrobatics = 0.025f;
	static inline float fAcrobaticsGliding = 0.060f;

	// Controller
	static inline bool bOverrideControllerDeadzone = true;
	static inline float fControllerRadialDeadzone = 0.24f;
	static inline float fControllerAxialDeadzone = 0.12f;
	static inline bool bThumbstickBounceFix = false;

	// Keys
	static inline uint32_t uTargetLockKey = 258;
	static inline uint32_t uSwitchTargetLeftKey = static_cast<uint32_t>(-1);
	static inline uint32_t uSwitchTargetRightKey = static_cast<uint32_t>(-1);
	static inline uint32_t uTargetLockBehindTargetKey = static_cast<uint32_t>(-1);

	// Non-MCM
	static inline std::unordered_map<RE::BGSBodyPartData*, std::vector<std::string>> targetPoints;

	static inline RE::BGSKeyword* kywd_magicWard = nullptr;
	static inline RE::BGSKeyword* kywd_furnitureForces1stPerson = nullptr;
	static inline RE::BGSKeyword* kywd_furnitureForces3rdPerson = nullptr;
	static inline RE::SpellItem* spel_targetLockSpell = nullptr;
	static inline RE::TESGlobal* glob_directionalMovement = nullptr;
	static inline RE::TESGlobal* glob_targetLockHint = nullptr;
	static inline RE::TESGlobal* glob_trueHUD = nullptr;
	static inline RE::TESGlobal* glob_nemesisHeadtracking = nullptr;
	static inline RE::TESGlobal* glob_nemesisMountedArchery = nullptr;
	static inline RE::TESGlobal* glob_nemesisLeaning = nullptr;
};
