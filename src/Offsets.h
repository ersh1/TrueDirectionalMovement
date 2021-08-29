#pragma once

static float* g_deltaTime = (float*)REL::ID(523660).address();							// 2F6B948
static float* g_deltaTimeRealTime = (float*)REL::ID(523661).address();                  // 2F6B94C
static float* g_DurationOfApplicationRunTimeMS = (float*)REL::ID(523662).address();     // 2F6B950
static float* g_fControllerBufferDepth = (float*)REL::ID(509447).address();				// 1DF1468
static bool* g_bDampenPlayerControls = (bool*)REL::ID(509451).address();				// 1DF1498
static float* g_fSprintStopThreshold = (float*)REL::ID(509489).address();				// 1DF1680
static float* g_fHUDOpacity = (float*)REL::ID(510579).address();						// 1DF58F8
static float* g_fNearDistance = (float*)REL::ID(512125).address();                      // 1E06ED0

static uintptr_t g_worldToCamMatrix = REL::ID(519579).address();						// 2F4C910
static RE::NiRect<float>* g_viewPort = (RE::NiRect<float>*)REL::ID(519618).address();	// 2F4DED0


typedef RE::BGSMovementType*(__fastcall* tsub_140335150)(const char** a1);
static REL::Relocation<tsub_140335150> sub_140335150{ REL::ID(23268) };

typedef int64_t(__fastcall* tBShkbAnimationGraph_sub_140AF0C10)(RE::BShkbAnimationGraph* a_this, RE::BSFixedString* a_string);
static REL::Relocation<tBShkbAnimationGraph_sub_140AF0C10> BShkbAnimationGraph_sub_140AF0C10{ REL::ID(62654) };

typedef bool(__fastcall* tActor__IsSyncSprintState_140608800)(RE::Actor* a_this);
static REL::Relocation<tActor__IsSyncSprintState_140608800> Actor__IsSyncSprintState_140608800{ REL::ID(36924) };

typedef bool(__fastcall* tActor__sub_1405D16B0)(RE::Actor* a_this);
static REL::Relocation<tActor__sub_1405D16B0> Actor__sub_1405D16B0{ REL::ID(36260) };

typedef void(__fastcall* tPlayerControls__sub_140705530)(RE::PlayerControls* a_this, int32_t a2, int32_t a3);
static REL::Relocation<tPlayerControls__sub_140705530> PlayerControls__sub_140705530{ REL::ID(41271) };

typedef float(__fastcall* tActor_sub_140608C60)(RE::Actor* a_this);
static REL::Relocation<tActor_sub_140608C60> Actor_sub_140608C60{ REL::ID(36932) };

typedef int64_t(__fastcall* tActor_sub_140634590)(RE::Actor* a_this);
static REL::Relocation<tActor_sub_140634590> Actor_sub_140634590{ REL::ID(37852) };

typedef void(__fastcall* tsub_140C6E180)(RE::NiQuaternion& a1, float a2, float a3, float a4);
static REL::Relocation<tsub_140C6E180> sub_140C6E180{ REL::ID(69466) };

typedef void(__fastcall* tPlayerControls__ApplyLookSensitivitySettings_140705AE0)(RE::PlayerControls* a_this, RE::NiPoint2* a_lookVector);
static REL::Relocation<tPlayerControls__ApplyLookSensitivitySettings_140705AE0> PlayerControls__ApplyLookSensitivitySettings_140705AE0{ REL::ID(41275) };

typedef float(__fastcall* tGetSubmergeLevel)(RE::TESObjectREFR* a_this, float a_zPos, RE::TESObjectCELL* a_parentCell);
static REL::Relocation<tGetSubmergeLevel> GetSubmergeLevel{ REL::ID(36452) };
