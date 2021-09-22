#pragma once

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


enum class ReticleStyle : std::uint32_t
{
	kDefault = 0,
	kSimpleDot = 1,
	kSimpleGlow = 2
};

enum class WidgetAnchor : std::uint32_t
{
	kBody = 0,
	kHead = 1,
};

enum class TargetLockLevelMode : std::uint32_t
{
	kDisable = 0,
	kIcon = 1,
	kText = 2,
	kOutline = 3
};

enum class BossLevelMode : std::uint32_t
{
	kDisable = 0,
	kIcon = 1,
	kText = 2
};

enum class TextAlignment : std::uint32_t
{
	kCenter = 0,
	kLeft = 1,
	kRight = 2
};

struct Settings
{
	static void ReadSettings();

	static void ReadBoolSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, bool& a_setting);
	static void ReadFloatSetting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, float& a_setting);
	static void ReadUInt32Setting(CSimpleIniA& a_ini, const char* a_sectionName, const char* a_settingName, uint32_t& a_setting);

	// General
	static inline bool bDirectionalMovementSheathed = true;
	static inline bool bDirectionalMovementDrawn = true;
	static inline DialogueMode uDialogueMode = DialogueMode::kFaceSpeaker;
	static inline float fMeleeMagnetismAngle = 60.f;

	// Directional Movement related
	static inline bool bFaceCrosshairWhileAttacking = false;
	static inline bool bFaceCrosshairWhileShouting = true;
	static inline bool bFaceCrosshairWhileBlocking = true;
	static inline bool bFaceCrosshairDuringAutoMove = true;
	static inline bool bStopOnDirectionChange = true;
	static inline float fRunningRotationSpeedMult = 1.5f;
	static inline float fSprintingRotationSpeedMult = 2.f;
	static inline float fAttackStartRotationSpeedMult = 5.f;
	static inline float fAttackMidRotationSpeedMult = 1.f;
	static inline float fAttackEndRotationSpeedMult = 0.f;
	static inline float fAirRotationSpeedMult = 0.5f;
	static inline float fWaterRotationSpeedMult = 0.5f;
	static inline float fSwimmingRotationSpeedMult = 0.5f;
	static inline float fFaceCrosshairRotationSpeedMultiplier = 2.f;
	static inline bool bFaceCrosshairInstantly = false;
	static inline bool bDisableAttackRotationMultipliersForTransformations = true;
	static inline float fSwimmingPitchSpeed = 3.f;
	static inline bool bThumbstickBounceFix = false;

	// Headtracking
	static inline bool bHeadtracking = true;
	static inline bool bHeadtrackSpine = true;
	static inline float fDialogueHeadtrackingDuration = 3.0f;
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
	static inline bool bTargetLockUsePOVSwitch = false;
	static inline float fTargetLockPOVHoldDuration = 0.25f;
	static inline bool bTargetLockUseMouse = true;
	static inline uint32_t uTargetLockMouseSensitivity = 32;
	static inline bool bTargetLockUseScrollWheel = true;
	static inline bool bTargetLockUseRightThumbstick = true;
	static inline bool bResetCameraWithTargetLock = false;
	
	// Target Lock Widget
	static inline bool bShowReticle = true;
	static inline bool bShowTargetBar = true;
	static inline bool bShowSoftTargetBar = true;
	static inline bool bShowTargetName = true;
	static inline bool bShowHealthPhantom = true;
	static inline bool bShowDamage = true;
	static inline bool bHideVanillaTargetBar = true;
	static inline WidgetAnchor uReticleAnchor = WidgetAnchor::kBody;
	static inline ReticleStyle uReticleStyle = ReticleStyle::kDefault;
	static inline WidgetAnchor uTargetBarAnchor = WidgetAnchor::kHead;
	static inline float fTargetBarZOffset = 30.f;
	static inline TargetLockLevelMode uTargetLevelMode = TargetLockLevelMode::kOutline;
	static inline uint16_t uTargetLevelThreshold = 10;
	static inline float fHealthPhantomDuration = 0.75f;
	static inline float fDamageDuration = 2.f;
	static inline float fReticleScale = 1.f;
	static inline bool bUseHUDOpacityForReticle = true;
	static inline float fReticleOpacity = 1.f;
	static inline float fTargetBarScale = 1.f;
	static inline bool bUseHUDOpacityForTargetBar = true;
	static inline float fTargetBarOpacity = 1.f;

	// Boss Bar Widget
	static inline bool bShowBossBar = true;
	static inline bool bShowBossHealthPhantom = true;
	static inline bool bShowBossDamage = true;
	static inline bool bBossHideVanillaTargetBar = true;
	static inline TextAlignment uBossNameAlignment = TextAlignment::kCenter;
	static inline BossLevelMode uBossLevelMode = BossLevelMode::kText;
	static inline uint16_t uBossLevelThreshold = 10;
	static inline float fBossHealthPhantomDuration = 0.75f;
	static inline float fBossDamageDuration = 2.f;
	static inline float fBossBarScale = 1.f;
	static inline bool bUseHUDOpacityForBossBar = true;
	static inline float fBossBarOpacity = 1.f;
	static inline float fBossBarX = 0.5f;
	static inline float fBossBarY = 0.87f;
	static inline bool bBossBarOffsetSubtitles = true;
	static inline float fMultipleBossBarsOffset = 45.f;
	static inline bool bMultipleBossBarsStackUpwards = true;

	// Keys
	static inline std::uint32_t uTargetLockKey = static_cast<std::uint32_t>(-1);
	static inline std::uint32_t uSwitchTargetLeftKey = static_cast<std::uint32_t>(-1);
	static inline std::uint32_t uSwitchTargetRightKey = static_cast<std::uint32_t>(-1);
};
