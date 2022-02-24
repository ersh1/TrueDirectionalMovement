#pragma once

// variables
#ifdef IS_SKYRIM_AE
static float* g_deltaTime = (float*)REL::ID(410199).address();                         // 30064C8
static float* g_deltaTimeRealTime = (float*)REL::ID(410200).address();                 // 30064CC
static float* g_DurationOfApplicationRunTimeMS = (float*)REL::ID(410201).address();    // 30064D0
static float* g_fHUDOpacity = (float*)REL::ID(383659).address();                       // 1E89D38
static float* g_fControllerBufferDepth = (float*)REL::ID(381879).address();            // 1E85878
static bool* g_bDampenPlayerControls = (bool*)REL::ID(381885).address();               // 1E858A8
static float* g_fSprintStopThreshold = (float*)REL::ID(381942).address();              // 1E85A90
static float* g_fLThumbDeadzone = (float*)REL::ID(388471).address();                   // 1E9A278
static float* g_fLThumbDeadzoneMax = (float*)REL::ID(388480).address();                // 1E9A2C0
static float* g_fActivatePickLength = (float*)REL::ID(370109).address();               // 1E6BF68
static float* g_f3PArrowTiltUpAngle = (float*)REL::ID(360295).address();               // 1E49338
static uintptr_t g_worldToCamMatrix = REL::ID(406126).address();                       // 2FE75F0
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)REL::ID(406160).address();  // 2FE8B98  // manually found
static uintptr_t g_highActorCuller = REL::ID(403540).address();                        // 2FC1A50
static float* g_fNear = (float*)(REL::ID(403540).address() + 0x40);                    // 2FC1A90
static float* g_fFar = (float*)(REL::ID(403540).address() + 0x44);                     // 2FC1A94
#else
static float* g_deltaTime = (float*)REL::ID(523660).address();                         // 2F6B948
static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();				   // 2F6B94C
static float* g_DurationOfApplicationRunTimeMS = (float*)REL::ID(523662).address();    // 2F6B950
static float* g_fHUDOpacity = (float*)REL::ID(510579).address();                       // 1DF58F8
static float* g_fControllerBufferDepth = (float*)REL::ID(509447).address();            // 1DF1468
static bool* g_bDampenPlayerControls = (bool*)REL::ID(509451).address();               // 1DF1498
static float* g_fSprintStopThreshold = (float*)REL::ID(509489).address();              // 1DF1680
static float* g_fLThumbDeadzone = (float*)REL::ID(511905).address();                   // 1E05DD0
static float* g_fLThumbDeadzoneMax = (float*)REL::ID(511911).address();                // 1E05E18
static float* g_fActivatePickLength = (float*)REL::ID(502527).address();               // 1DD7E88
static float* g_f3PArrowTiltUpAngle = (float*)REL::ID(501506).address();               // 1DB52C8
static uintptr_t g_worldToCamMatrix = REL::ID(519579).address();                       // 2F4C910
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)REL::ID(519618).address();  // 2F4DED0
static uintptr_t g_highActorCuller = REL::ID(517032).address();                        // 2F26F80
static float* g_fNear = (float*)(REL::ID(517032).address() + 0x40);                    // 2F26FC0
static float* g_fFar = (float*)(REL::ID(517032).address() + 0x44);                     // 2F26FC4
#endif

// functions
typedef RE::BGSMovementType*(__fastcall* tGetMovementTypeFromString)(const char** a1);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tGetMovementTypeFromString> GetMovementTypeFromString{ REL::ID(23727) };
#else
static REL::Relocation<tGetMovementTypeFromString> GetMovementTypeFromString{ REL::ID(23268) };
#endif

typedef bool(__fastcall* tBSInputDeviceManager_IsUsingGamepad)(RE::BSInputDeviceManager* a_this);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tBSInputDeviceManager_IsUsingGamepad> BSInputDeviceManager_IsUsingGamepad{ REL::ID(68622) };
#else
static REL::Relocation<tBSInputDeviceManager_IsUsingGamepad> BSInputDeviceManager_IsUsingGamepad{ REL::ID(67320) };
#endif

typedef int64_t(__fastcall* tBShkbAnimationGraph_GetCurrentMovementTypeName)(RE::BShkbAnimationGraph* a_this, RE::BSFixedString* a_string);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tBShkbAnimationGraph_GetCurrentMovementTypeName> BShkbAnimationGraph_GetCurrentMovementTypeName{ REL::ID(63599) };
#else
static REL::Relocation<tBShkbAnimationGraph_GetCurrentMovementTypeName> BShkbAnimationGraph_GetCurrentMovementTypeName{ REL::ID(62654) };
#endif

typedef bool(__fastcall* tActor_IsSyncSprintState)(RE::Actor* a_this);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tActor_IsSyncSprintState> Actor_IsSyncSprintState{ REL::ID(37949) }; // partial
#else
static REL::Relocation<tActor_IsSyncSprintState> Actor_IsSyncSprintState{ REL::ID(36924) };
#endif

typedef bool(__fastcall* tActor_CanSprint_CheckCharacterControllerValues)(RE::Actor* a_this);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tActor_CanSprint_CheckCharacterControllerValues> Actor_CanSprint_CheckCharacterControllerValues{ REL::ID(37244) };  // manual
#else
static REL::Relocation<tActor_CanSprint_CheckCharacterControllerValues> Actor_CanSprint_CheckCharacterControllerValues{ REL::ID(36260) };
#endif

typedef void(__fastcall* tPlayerControls_CalledWhenSprintStateChanges)(RE::PlayerControls* a_this, int32_t a2, int32_t a3);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tPlayerControls_CalledWhenSprintStateChanges> PlayerControls_CalledWhenSprintStateChanges{ REL::ID(42350) };
#else
static REL::Relocation<tPlayerControls_CalledWhenSprintStateChanges> PlayerControls_CalledWhenSprintStateChanges{ REL::ID(41271) };
#endif

typedef void(__fastcall* tNiQuaternion_SomeRotationManipulation)(RE::NiQuaternion& a1, float a2, float a3, float a4);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tNiQuaternion_SomeRotationManipulation> NiQuaternion_SomeRotationManipulation{ REL::ID(70843) };
#else
static REL::Relocation<tNiQuaternion_SomeRotationManipulation> NiQuaternion_SomeRotationManipulation{ REL::ID(69466) };
#endif

typedef void(__fastcall* tPlayerControls_ApplyLookSensitivitySettings)(RE::PlayerControls* a_this, RE::NiPoint2* a_lookVector);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tPlayerControls_ApplyLookSensitivitySettings> PlayerControls_ApplyLookSensitivitySettings{ REL::ID(42354) }; // manual
#else
static REL::Relocation<tPlayerControls_ApplyLookSensitivitySettings> PlayerControls_ApplyLookSensitivitySettings{ REL::ID(41275) };
#endif

typedef float(__fastcall* tTESObjectREFR_GetSubmergeLevel)(RE::TESObjectREFR* a_this, float a_zPos, RE::TESObjectCELL* a_parentCell);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tTESObjectREFR_GetSubmergeLevel> TESObjectREFR_GetSubmergeLevel{ REL::ID(37448) };
#else
static REL::Relocation<tTESObjectREFR_GetSubmergeLevel> TESObjectREFR_GetSubmergeLevel{ REL::ID(36452) };
#endif

typedef RE::NiAVObject*(__fastcall* tNiAVObject_LookupBoneNodeByName)(RE::NiAVObject* a_this, const RE::BSFixedString& a_name, bool a3);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tNiAVObject_LookupBoneNodeByName> NiAVObject_LookupBoneNodeByName{ REL::ID(76207) };
#else
static REL::Relocation<tNiAVObject_LookupBoneNodeByName> NiAVObject_LookupBoneNodeByName{ REL::ID(74481) };
#endif

typedef void*(__fastcall* tCastRay)(RE::hkpWorld* a_this, RE::bhkWorld::RAYCAST_DATA& a_input, RE::hkpRayHitCollector& a_collector);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tCastRay> CastRay{ REL::ID(61400) };
#else
static REL::Relocation<tCastRay> CastRay{ REL::ID(60552) };
#endif

typedef void*(__fastcall* tLinearCast)(RE::hkpWorld* world, const RE::hkpCollidable* collA, const RE::hkpLinearCastInput* input, RE::hkpCdPointCollector* castCollector, RE::hkpCdPointCollector* startCollector);
#ifdef IS_SKYRIM_AE
static REL::Relocation<tLinearCast> LinearCast{ REL::ID(61402) };
#else
static REL::Relocation<tLinearCast> LinearCast{ REL::ID(60554) };
#endif
