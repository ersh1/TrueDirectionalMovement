#pragma once

// variables
static float* g_deltaTime = (float*)RELOCATION_ID(523660, 410199).address();                         // 2F6B948, 30064C8
static float* g_deltaTimeRealTime = (float*)RELOCATION_ID(523661, 410200).address();                 // 2F6B94C, 30064CC
static float* g_DurationOfApplicationRunTimeMS = (float*)RELOCATION_ID(523662, 410201).address();    // 2F6B950, 30064D0
static float* g_fHUDOpacity = (float*)RELOCATION_ID(510579, 383659).address();                       // 1DF58F8, 1E89D38
static float* g_fControllerBufferDepth = (float*)RELOCATION_ID(509447, 381879).address();            // 1DF1468, 1E85878
static bool* g_bDampenPlayerControls = (bool*)RELOCATION_ID(509451, 381885).address();               // 1DF1498, 1E858A8
static float* g_fSprintStopThreshold = (float*)RELOCATION_ID(509489, 381942).address();              // 1DF1680, 1E85A90
static float* g_fLThumbDeadzone = (float*)RELOCATION_ID(511905, 388471).address();                   // 1E05DD0, 1E9A278
static float* g_fLThumbDeadzoneMax = (float*)RELOCATION_ID(511911, 388480).address();                // 1E05E18, 1E9A2C0
static float* g_fActivatePickLength = (float*)RELOCATION_ID(502527, 370109).address();               // 1DD7E88, 1E6BF68
static float* g_f3PArrowTiltUpAngle = (float*)RELOCATION_ID(501506, 360295).address();               // 1DB52C8, 1E49338
static uintptr_t g_worldToCamMatrix = RELOCATION_ID(519579, 406126).address();                       // 2F4C910, 2FE75F0
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)RELOCATION_ID(519618, 406160).address();  // 2F4DED0, 2FE8B98
static uintptr_t g_highActorCuller = RELOCATION_ID(517032, 403540).address();                        // 2F26F80, 2FC1A50
static float* g_fNear = (float*)(RELOCATION_ID(517032, 403540).address() + 0x40);                    // 2F26FC0, 2FC1A90
static float* g_fFar = (float*)(RELOCATION_ID(517032, 403540).address() + 0x44);                     // 2F26FC4, 2FC1A94
static float* g_worldScale = (float*)RELOCATION_ID(231896, 188105).address();                        // 154064C, 1637AA0
static float* g_worldScaleInverse = (float*)RELOCATION_ID(230692, 187407).address();                 // 1536BA0, 162DF48
static float* g_fFreeRotationSpeed = (float*)RELOCATION_ID(509884, 382636).address();                // 1DF3820, 1E87BC8
static float* g_fVanityModeMinDist = (float*)RELOCATION_ID(509874, 382621).address();                // 1DF37A8, 1E87B50
static float* g_fVanityModeMaxDist = (float*)RELOCATION_ID(509878, 382627).address();                // 1DF37D8, 1E87B80
static void** g_142EC5C60 = (void**)RELOCATION_ID(514725, 400883).address();                         // 2EC5C60, 2F603B0

// functions
typedef RE::BGSMovementType*(__fastcall* tGetMovementTypeFromString)(const char** a1);
static REL::Relocation<tGetMovementTypeFromString> GetMovementTypeFromString{ RELOCATION_ID(23268, 23727) };

typedef bool(__fastcall* tBSInputDeviceManager_IsUsingGamepad)(RE::BSInputDeviceManager* a_this);
static REL::Relocation<tBSInputDeviceManager_IsUsingGamepad> BSInputDeviceManager_IsUsingGamepad{ RELOCATION_ID(67320, 68622) };

typedef int64_t(__fastcall* tBShkbAnimationGraph_GetCurrentMovementTypeName)(RE::BShkbAnimationGraph* a_this, RE::BSFixedString* a_string);
static REL::Relocation<tBShkbAnimationGraph_GetCurrentMovementTypeName> BShkbAnimationGraph_GetCurrentMovementTypeName{ RELOCATION_ID(62654, 63599) };

typedef bool(__fastcall* tActor_IsSyncSprintState)(RE::Actor* a_this);
static REL::Relocation<tActor_IsSyncSprintState> Actor_IsSyncSprintState{ RELOCATION_ID(36924, 37949) };

typedef bool(__fastcall* tActor_CanSprint_CheckCharacterControllerValues)(RE::Actor* a_this);
static REL::Relocation<tActor_CanSprint_CheckCharacterControllerValues> Actor_CanSprint_CheckCharacterControllerValues{ RELOCATION_ID(36260, 37244) };

typedef void(__fastcall* tPlayerControls_CalledWhenSprintStateChanges)(RE::PlayerControls* a_this, int32_t a2, int32_t a3);
static REL::Relocation<tPlayerControls_CalledWhenSprintStateChanges> PlayerControls_CalledWhenSprintStateChanges{ RELOCATION_ID(41271, 42350) };

typedef void(__fastcall* tNiQuaternion_SomeRotationManipulation)(RE::NiQuaternion& a1, float a2, float a3, float a4);
static REL::Relocation<tNiQuaternion_SomeRotationManipulation> NiQuaternion_SomeRotationManipulation{ RELOCATION_ID(69466, 70843) };

typedef void(__fastcall* tPlayerControls_ApplyLookSensitivitySettings)(RE::PlayerControls* a_this, RE::NiPoint2* a_lookVector);
static REL::Relocation<tPlayerControls_ApplyLookSensitivitySettings> PlayerControls_ApplyLookSensitivitySettings{ RELOCATION_ID(41275, 42354) };

typedef float(__fastcall* tTESObjectREFR_GetSubmergeLevel)(RE::TESObjectREFR* a_this, float a_zPos, RE::TESObjectCELL* a_parentCell);
static REL::Relocation<tTESObjectREFR_GetSubmergeLevel> TESObjectREFR_GetSubmergeLevel{ RELOCATION_ID(36452, 37448) };

typedef RE::NiAVObject*(__fastcall* tNiAVObject_LookupBoneNodeByName)(RE::NiAVObject* a_this, const RE::BSFixedString& a_name, bool a3);
static REL::Relocation<tNiAVObject_LookupBoneNodeByName> NiAVObject_LookupBoneNodeByName{ RELOCATION_ID(74481, 76207) };

typedef void*(__fastcall* tCastRay)(RE::hkpWorld* a_this, RE::hkpWorldRayCastInput& a_input, RE::hkpRayHitCollector& a_collector);
static REL::Relocation<tCastRay> CastRay{ RELOCATION_ID(60552, 61400) };

typedef void*(__fastcall* tLinearCast)(RE::hkpWorld* world, const RE::hkpCollidable* collA, const RE::hkpLinearCastInput* input, RE::hkpCdPointCollector* castCollector, RE::hkpCdPointCollector* startCollector);
static REL::Relocation<tLinearCast> LinearCast{ RELOCATION_ID(60554, 61402) };

typedef void(tPlayerCamera_SetCameraState)(RE::PlayerCamera* a_this, RE::CameraState a_cameraState);
static REL::Relocation<tPlayerCamera_SetCameraState> PlayerCamera_SetCameraState{ RELOCATION_ID(49947, 50880) };  // 84EA70, 87A930

typedef void(tAIProcess_ClearHeadTrackTarget)(RE::AIProcess* a_this);
static REL::Relocation<tAIProcess_ClearHeadTrackTarget> AIProcess_ClearHeadTrackTarget{ RELOCATION_ID(38852, 39889) };  // 67D190, 6A4900

typedef float (*tGetPlayerTimeMult)(void* a1);
static REL::Relocation<tGetPlayerTimeMult> GetPlayerTimeMult{ RELOCATION_ID(43104, 44301) };  // 759420, 7873C0
