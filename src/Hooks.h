#pragma once

namespace Hooks
{
	class MovementHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> MovementHandlerVtbl{ RE::VTABLE_MovementHandler[0] };
			_ProcessThumbstick = MovementHandlerVtbl.write_vfunc(0x2, ProcessThumbstick);
			_ProcessButton = MovementHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static void ProcessThumbstick(RE::MovementHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data);
		static void ProcessButton(RE::MovementHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(ProcessThumbstick)> _ProcessThumbstick;
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class GamepadHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook{ REL::ID(68808) };                // C40F80
			_ProcessInput = trampoline.write_call<5>(hook.address() + 0x13F, ProcessInput);  // C410BF
#else
			REL::Relocation<uintptr_t> hook{ REL::ID(67497) };                // C1AB40
			_ProcessInput = trampoline.write_call<5>(hook.address() + 0x13F, ProcessInput);  // C1AC7F
#endif
		}

	private:
		static void ProcessInput(RE::BSWin32GamepadDevice* a_this, int32_t a_rawX, int32_t a_rawY, float a_deadzoneMin, float a_deadzoneMax, float& a_outX, float& a_outY);

		static inline REL::Relocation<decltype(ProcessInput)> _ProcessInput;
	};

	class LookHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> LookHandlerVtbl{ RE::VTABLE_LookHandler[0] };
			_ProcessThumbstick = LookHandlerVtbl.write_vfunc(0x2, ProcessThumbstick);
			_ProcessMouseMove = LookHandlerVtbl.write_vfunc(0x3, ProcessMouseMove);
		}

	private:
		static void ProcessThumbstick(RE::LookHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data);
		static void ProcessMouseMove(RE::LookHandler* a_this, RE::MouseMoveEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(ProcessThumbstick)> _ProcessThumbstick;
		static inline REL::Relocation<decltype(ProcessMouseMove)> _ProcessMouseMove;
	};

	class TogglePOVHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> TogglePOVHandlerVtbl{ RE::VTABLE_TogglePOVHandler[0] };
			_ProcessButton = TogglePOVHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static void ProcessButton(RE::TogglePOVHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class FirstPersonStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> FirstPersonStateVtbl{ RE::VTABLE_FirstPersonState[0] };
			_OnEnterState = FirstPersonStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = FirstPersonStateVtbl.write_vfunc(0x2, OnExitState);
			REL::Relocation<std::uintptr_t> PlayerInputHandlerVtbl{ RE::VTABLE_FirstPersonState[1] };
			_ProcessButton = PlayerInputHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static void OnEnterState(RE::FirstPersonState* a_this);
		static void OnExitState(RE::FirstPersonState* a_this);
		static void ProcessButton(RE::FirstPersonState* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);
		
		static inline REL::Relocation<decltype(OnEnterState)> _OnEnterState;
		static inline REL::Relocation<decltype(OnExitState)> _OnExitState;
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class ThirdPersonStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> ThirdPersonStateVtbl{ RE::VTABLE_ThirdPersonState[0] };
			_OnEnterState = ThirdPersonStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = ThirdPersonStateVtbl.write_vfunc(0x2, OnExitState);
			_SetFreeRotationMode = ThirdPersonStateVtbl.write_vfunc(0xD, SetFreeRotationMode);
			REL::Relocation<std::uintptr_t> PlayerInputHandlerVtbl{ RE::VTABLE_ThirdPersonState[1] };
			_ProcessButton = PlayerInputHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}
	private:		
		static void OnEnterState(RE::ThirdPersonState* a_this);
		static void OnExitState(RE::ThirdPersonState* a_this);
		static void SetFreeRotationMode(RE::ThirdPersonState* a_this, bool a_weaponSheathed);
		static void ProcessButton(RE::ThirdPersonState* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(OnEnterState)> _OnEnterState;
		static inline REL::Relocation<decltype(OnExitState)> _OnExitState;
		static inline REL::Relocation<decltype(SetFreeRotationMode)> _SetFreeRotationMode;
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class HorseCameraStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> HorseCameraStateVtbl{ RE::VTABLE_HorseCameraState[0] };
			_OnEnterState = HorseCameraStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = HorseCameraStateVtbl.write_vfunc(0x2, OnExitState);
			_UpdateRotation = HorseCameraStateVtbl.write_vfunc(0xE, UpdateRotation);
			_HandleLookInput = HorseCameraStateVtbl.write_vfunc(0xF, HandleLookInput);
			REL::Relocation<std::uintptr_t> PlayerInputHandlerVtbl{ RE::VTABLE_HorseCameraState[1] };
			_ProcessButton = PlayerInputHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static void OnEnterState(RE::HorseCameraState* a_this);
		static void OnExitState(RE::HorseCameraState* a_this);
		static void UpdateRotation(RE::HorseCameraState* a_this);
		static void HandleLookInput(RE::HorseCameraState* a_this, const RE::NiPoint2& a_input); 
		static void ProcessButton(RE::HorseCameraState* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(OnEnterState)> _OnEnterState;
		static inline REL::Relocation<decltype(OnExitState)> _OnExitState;
		static inline REL::Relocation<decltype(UpdateRotation)> _UpdateRotation;
		static inline REL::Relocation<decltype(HandleLookInput)> _HandleLookInput;
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class TweenMenuCameraStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> TweenMenuCameraStateVtbl{ RE::VTABLE_TweenMenuCameraState[0] };
			_OnEnterState = TweenMenuCameraStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = TweenMenuCameraStateVtbl.write_vfunc(0x2, OnExitState);
		}

	private:
		static void OnEnterState(RE::TESCameraState* a_this);
		static void OnExitState(RE::TESCameraState* a_this);

		static inline REL::Relocation<decltype(OnEnterState)> _OnEnterState;
		static inline REL::Relocation<decltype(OnExitState)> _OnExitState;
	};

	class VATSCameraStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> VATSCameraStateVtbl{ RE::VTABLE_VATSCameraState[0] };
			_OnExitState = VATSCameraStateVtbl.write_vfunc(0x2, OnExitState);
		}

	private:
		static void OnExitState(RE::TESCameraState* a_this);

		static inline REL::Relocation<decltype(OnExitState)> _OnExitState;
	};

	class PlayerCameraTransitionStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> PlayerCameraTransitionStateVtbl{ RE::VTABLE_PlayerCameraTransitionState[0] };
			_OnEnterState = PlayerCameraTransitionStateVtbl.write_vfunc(0x1, OnEnterState);
		}

	private:
		static void OnEnterState(RE::PlayerCameraTransitionState* a_this);

		static inline REL::Relocation<decltype(OnEnterState)> _OnEnterState;
	};

	class MovementHandlerAgentPlayerControlsHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> MovementHandlerAgentPlayerControls_IMovementHandlerAgentVtbl{ RE::VTABLE_MovementHandlerAgentPlayerControls[1] };  // 166ABA0
			_Func1 = MovementHandlerAgentPlayerControls_IMovementHandlerAgentVtbl.write_vfunc(0x2, Func1);
		}
	private:
		static void Func1(void* a1, void* a2);

		static inline REL::Relocation<decltype(Func1)> _Func1;
	};

	class ProjectileHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> ProjectileVtbl{ RE::VTABLE_Projectile[0] };				// 167C888
			REL::Relocation<std::uintptr_t> ArrowProjectileVtbl{ RE::VTABLE_ArrowProjectile[0] };			// 1676318
			REL::Relocation<std::uintptr_t> MissileProjectileVtbl{ RE::VTABLE_MissileProjectile[0] };		// 167AE78
			REL::Relocation<std::uintptr_t> BeamProjectileVtbl{ RE::VTABLE_BeamProjectile[0] };          // 1677660
			_GetLinearVelocityProjectile = ProjectileVtbl.write_vfunc(0x86, GetLinearVelocityProjectile);
			_GetLinearVelocityArrow = ArrowProjectileVtbl.write_vfunc(0x86, GetLinearVelocityArrow);
			_GetLinearVelocityMissile = MissileProjectileVtbl.write_vfunc(0x86, GetLinearVelocityMissile);

			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(44222) };  // 7821A0  // manual
			trampoline.write_call<6>(hook1.address() + 0x6FF, Func183);  // 78289F // vfunc call

			REL::Relocation<uintptr_t> hook2{ REL::ID(44222) };  // 7821A0
			_InitProjectile = trampoline.write_call<5>(hook2.address() + 0x78A, InitProjectile);
#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(43030) };  // 754820
			trampoline.write_call<6>(hook1.address() + 0x318, Func183);  // 754B24 // vfunc call

			REL::Relocation<uintptr_t> hook2{ REL::ID(43030) };  // 754820
			_InitProjectile = trampoline.write_call<5>(hook2.address() + 0x3B8, InitProjectile); 

#endif
		}
	private:
		static void ProjectileAimSupport(RE::Projectile* a_this);
		static void GetLinearVelocityProjectile(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity);
		static void GetLinearVelocityArrow(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity);
		static void GetLinearVelocityMissile(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity);
		static void Func183(RE::Projectile* a_this);
		static void InitProjectile(RE::Projectile* a_this);

		static inline REL::Relocation<decltype(GetLinearVelocityProjectile)> _GetLinearVelocityProjectile;
		static inline REL::Relocation<decltype(GetLinearVelocityArrow)> _GetLinearVelocityArrow;
		static inline REL::Relocation<decltype(GetLinearVelocityMissile)> _GetLinearVelocityMissile;
		static inline REL::Relocation<decltype(InitProjectile)> _InitProjectile;
	};

	class CharacterHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> CharacterVtbl{ RE::VTABLE_Character[0] };
			_Update = CharacterVtbl.write_vfunc(0xAD, Update);
		}

	private:
		static void Update(RE::Actor* a_this, float a_delta);

		static inline REL::Relocation<decltype(Update)> _Update;
	};

	class PlayerCharacterHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::VTABLE_PlayerCharacter[0] };					// 1665E0
			_UpdateAnimation = PlayerCharacterVtbl.write_vfunc(0x7D, UpdateAnimation);
			_ProcessTracking = PlayerCharacterVtbl.write_vfunc(0x122, ProcessTracking);
			REL::Relocation<std::uintptr_t> PlayerCharacter_BSTEventSink_BSAnimationGraphEventVtbl{ RE::VTABLE_PlayerCharacter[2] };  // 1663F78
			_ProcessEvent = PlayerCharacter_BSTEventSink_BSAnimationGraphEventVtbl.write_vfunc(0x1, ProcessEvent);
			REL::Relocation<std::uintptr_t> PlayerCharacter_ActorStateVtbl{ RE::VTABLE_PlayerCharacter[6] };  // 16640E8
			_GetAngle = PlayerCharacter_ActorStateVtbl.write_vfunc(0x4, GetAngle);

			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook{ REL::ID(40447) };  // 6C6440
			_UpdateSprintState = trampoline.write_call<5>(hook.address() + 0x140B, UpdateSprintState);
#else
			REL::Relocation<uintptr_t> hook{ REL::ID(39375) };  // 69E580
			_UpdateSprintState = trampoline.write_call<5>(hook.address() + 0xDAE, UpdateSprintState);
#endif
		}

	private:
		static void UpdateAnimation(RE::Actor* a_this, float a_delta);
		static void ProcessTracking(RE::Actor* a_this, float a_delta, RE::NiAVObject* a_obj3D);
		static void ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_dispatcher);
		static void GetAngle(RE::ActorState* a_this, RE::NiPoint3 &a_angle);
		static void UpdateSprintState(RE::PlayerCharacter* a_this);

		static inline REL::Relocation<decltype(UpdateAnimation)> _UpdateAnimation;
		static inline REL::Relocation<decltype(ProcessTracking)> _ProcessTracking;
		static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;
		static inline REL::Relocation<decltype(GetAngle)> _GetAngle;
		static inline REL::Relocation<decltype(UpdateSprintState)> _UpdateSprintState;
	};

	class PlayerControlsHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> PlayerControlsVtbl{ RE::VTABLE_PlayerControls[0] };  // 166E838
			_Handle = PlayerControlsVtbl.write_vfunc(0x1, Handle);

			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook{ REL::ID(42338) };  // 72E720
			_CheckIsInSyncAnim = trampoline.write_call<5>(hook.address() + 0x153, CheckIsInSyncAnim);
			_Check2 = trampoline.write_call<5>(hook.address() + 0x106, Check2);
			_Check3 = trampoline.write_call<5>(hook.address() + 0x12A, Check3);
#else
			//REL::Relocation<uintptr_t> hook{ REL::ID(41259) };  // 704DE0
			REL::Relocation<uintptr_t> hook{ REL::ID(41288) };  // 706AF0
			_CheckIsInSyncAnim = trampoline.write_call<5>(hook.address() + 0xD8, CheckIsInSyncAnim);
			_Check2 = trampoline.write_call<5>(hook.address() + 0x99, Check2);
			_Check3 = trampoline.write_call<5>(hook.address() + 0xB9, Check3);
			//_CanProcessControls = trampoline.write_call<5>(hook.address() + 0x2E, CanProcessControls);  // 704E0E  // had to be revamped for AE
#endif
			
		}

	private:
		static bool Handle(RE::PlayerControls* a_this, RE::InputEvent** a_event);
		//static bool CanProcessControls(RE::PlayerControls* a_this, RE::InputEvent** a_eventPtr);
		static bool CheckIsInSyncAnim(void* a_a1, void* a_a2);
		static bool Check2(RE::PlayerCharacter* a_this);
		static bool Check3(RE::PlayerCharacter* a_this);
		static inline REL::Relocation<decltype(Handle)> _Handle;
		//static inline REL::Relocation<decltype(CanProcessControls)> _CanProcessControls;
		static inline REL::Relocation<decltype(CheckIsInSyncAnim)> _CheckIsInSyncAnim;
		static inline REL::Relocation<decltype(Check2)> _Check2;
		static inline REL::Relocation<decltype(Check3)> _Check3;

	};

	class AIProcess_SetRotationSpeedZHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(37356) };  // 5FD7E0
			REL::Relocation<uintptr_t> hook2{ REL::ID(42373) };  // 731330

			_AIProcess_SetRotationSpeedZ1 = trampoline.write_call<5>(hook1.address() + 0x3EF, AIProcess_SetRotationSpeedZ1);
			_AIProcess_SetRotationSpeedZ2 = trampoline.write_call<5>(hook1.address() + 0x632, AIProcess_SetRotationSpeedZ2);
			_AIProcess_SetRotationSpeedZ3 = trampoline.write_branch<5>(hook2.address() + 0x49, AIProcess_SetRotationSpeedZ3);
#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(36365) };  // 5D87F0
			REL::Relocation<uintptr_t> hook2{ REL::ID(41293) };  // 707210

			_AIProcess_SetRotationSpeedZ1 = trampoline.write_call<5>(hook1.address() + 0x356, AIProcess_SetRotationSpeedZ1);
			_AIProcess_SetRotationSpeedZ2 = trampoline.write_call<5>(hook1.address() + 0x5E4, AIProcess_SetRotationSpeedZ2);
			_AIProcess_SetRotationSpeedZ3 = trampoline.write_branch<5>(hook2.address() + 0x49, AIProcess_SetRotationSpeedZ3);
#endif
		}

	private:
		static void AIProcess_SetRotationSpeedZ1(RE::AIProcess* a_this, float a_rotationSpeed);
		static void AIProcess_SetRotationSpeedZ2(RE::AIProcess* a_this, float a_rotationSpeed);
		static void AIProcess_SetRotationSpeedZ3(RE::AIProcess* a_this, float a_rotationSpeed);
		static inline REL::Relocation<decltype(AIProcess_SetRotationSpeedZ1)> _AIProcess_SetRotationSpeedZ1;
		static inline REL::Relocation<decltype(AIProcess_SetRotationSpeedZ2)> _AIProcess_SetRotationSpeedZ2;
		static inline REL::Relocation<decltype(AIProcess_SetRotationSpeedZ3)> _AIProcess_SetRotationSpeedZ3;
	};

	class Actor_SetRotationHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();
#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(32796) };  // 504B30  // synchronized anims
			REL::Relocation<uintptr_t> hook2{ REL::ID(37356) };  // 5FD7E0

			_Actor_SetRotationX = trampoline.write_call<5>(hook1.address() + 0x667, Actor_SetRotationX);  // 4EC7DC
			_Actor_SetRotationZ = trampoline.write_call<5>(hook2.address() + 0xA87, Actor_SetRotationZ);  // 5FE267

#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(32042) };  // 4EC300  // synchronized anims
			REL::Relocation<uintptr_t> hook2{ REL::ID(36365) };  // 5D87F0

			_Actor_SetRotationX = trampoline.write_call<5>(hook1.address() + 0x4DC, Actor_SetRotationX); // 4EC7DC
			_Actor_SetRotationZ = trampoline.write_call<5>(hook2.address() + 0x9C7, Actor_SetRotationZ); // 5D91B7 
#endif
		}

	private:
		static void Actor_SetRotationX(RE::Actor* a_this, float a_angle);
		static void Actor_SetRotationZ(RE::Actor* a_this, float a_angle);
		static inline REL::Relocation<decltype(Actor_SetRotationX)> _Actor_SetRotationX;
		static inline REL::Relocation<decltype(Actor_SetRotationZ)> _Actor_SetRotationZ;
	};

	class EnemyHealthHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> EnemyHealthVtbl{ RE::VTABLE_EnemyHealth[0] };  // 16B2A50
			_ProcessMessage = EnemyHealthVtbl.write_vfunc(0x2, ProcessMessage);
		}

	private:
		static bool ProcessMessage(uintptr_t a_enemyHealth, RE::HUDData* a_hudData);
		static inline REL::Relocation<decltype(ProcessMessage)> _ProcessMessage;
	};

	class HeadtrackingHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();

#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(38009) };  // 635270
			REL::Relocation<uintptr_t> hook2{ REL::ID(37367) };  // 5FF320
			REL::Relocation<uintptr_t> hook3{ REL::ID(37200) };  // 5F4320
			REL::Relocation<uintptr_t> hook4{ REL::ID(37541) };  // 60E050

			_SetHeadtrackTarget0 = trampoline.write_call<5>(hook1.address() + 0x594, SetHeadtrackTarget0); // 635804
			_SetHeadtrackTarget4A = trampoline.write_call<5>(hook2.address() + 0xA3, SetHeadtrackTarget4A);  // 5FF3C3
			_SetHeadtrackTarget4B = trampoline.write_call<5>(hook3.address() + 0x470, SetHeadtrackTarget4B); // 5F4790
			_SetHeadtrackTarget4C = trampoline.write_call<5>(hook3.address() + 0x5D3, SetHeadtrackTarget4C); // 5F48F3
			_SetHeadtrackTarget4D = trampoline.write_call<5>(hook4.address() + 0x181, SetHeadtrackTarget4D); // 60E1D1

#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(36984) };  // 60D300
			REL::Relocation<uintptr_t> hook2{ REL::ID(36376) };  // 5D9BF0
			REL::Relocation<uintptr_t> hook3{ REL::ID(36220) };  // 5CFD60
			REL::Relocation<uintptr_t> hook4{ REL::ID(36540) };  // 5E8070

			_SetHeadtrackTarget0 = trampoline.write_call<5>(hook1.address() + 0x592, SetHeadtrackTarget0); // 60D892
			_SetHeadtrackTarget4A = trampoline.write_call<5>(hook2.address() + 0xA3, SetHeadtrackTarget4A);  // 5D9C93
			_SetHeadtrackTarget4B = trampoline.write_call<5>(hook3.address() + 0x45C, SetHeadtrackTarget4B);  // 5D01BC
			_SetHeadtrackTarget4C = trampoline.write_call<5>(hook3.address() + 0x5BF, SetHeadtrackTarget4C);  // 5D031F
			_SetHeadtrackTarget4D = trampoline.write_call<5>(hook4.address() + 0x17E, SetHeadtrackTarget4D);  // 5E81EE
#endif
		}

	private:
		static void SetHeadtrackTarget0(RE::AIProcess* a_this, RE::Actor* a_target);
		static void SetHeadtrackTarget4A(RE::AIProcess* a_this, RE::Actor* a_target);
		static void SetHeadtrackTarget4B(RE::AIProcess* a_this, RE::Actor* a_target);
		static void SetHeadtrackTarget4C(RE::AIProcess* a_this, RE::Actor* a_target);
		static void SetHeadtrackTarget4D(RE::AIProcess* a_this, RE::Actor* a_target);
		static inline REL::Relocation<decltype(SetHeadtrackTarget0)> _SetHeadtrackTarget0;
		static inline REL::Relocation<decltype(SetHeadtrackTarget4A)> _SetHeadtrackTarget4A;
		static inline REL::Relocation<decltype(SetHeadtrackTarget4B)> _SetHeadtrackTarget4B;
		static inline REL::Relocation<decltype(SetHeadtrackTarget4C)> _SetHeadtrackTarget4C;
		static inline REL::Relocation<decltype(SetHeadtrackTarget4D)> _SetHeadtrackTarget4D;
	};

	class NukeSetIsNPCHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();

#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(56225) };  // 9BF1F0, bool papyrus wrapper
			//REL::Relocation<uintptr_t> hook{ REL::ID(32885) };  // 5096B0
			REL::Relocation<uintptr_t> hook2{ REL::ID(56226) };  // 9BF2B0, int papyrus wrapper

			_SetBool = trampoline.write_call<5>(hook1.address() + 0x4C, SetBool);
			//_SetBool = trampoline.write_call<5>(hook.address() + 0xE, SetBool);
			_SetInt = trampoline.write_call<5>(hook2.address() + 0x4B, SetInt);
#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(55694) };  // 996FD0, bool papyrus wrapper
			//REL::Relocation<uintptr_t> hook{ REL::ID(32141) };  // 4F06E0
			REL::Relocation<uintptr_t> hook2{ REL::ID(55695) };  // 997090, int papyrus wrapper

			_SetBool = trampoline.write_call<5>(hook1.address() + 0x4C, SetBool);
			//_SetBool = trampoline.write_call<5>(hook.address() + 0xE, SetBool);
			_SetInt = trampoline.write_call<5>(hook2.address() + 0x4B, SetInt);
#endif
		}

	private:
		static void SetBool(RE::IAnimationGraphManagerHolder* a_this, RE::BSFixedString* a_variableName, bool a_value);
		static void SetInt(RE::IAnimationGraphManagerHolder* a_this, RE::BSFixedString* a_variableName, int32_t a_value);

		static inline REL::Relocation<decltype(SetBool)> _SetBool;
		static inline REL::Relocation<decltype(SetInt)> _SetInt;
	};

	class PlayerCameraHook // to fix Improved Camera breaking player pitch during target lock
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();

#ifdef IS_SKYRIM_AE
			REL::Relocation<std::uintptr_t> hook1{ REL::ID(50784) };  // 876700
			REL::Relocation<std::uintptr_t> hook2{ REL::ID(50816) };  // 877970, EnterTweenMenuState

			_Update = trampoline.write_call<5>(hook1.address() + 0x1A6, Update);
			_SetCameraState = trampoline.write_call<5>(hook2.address() + 0x83, SetCameraState);
#else
			REL::Relocation<std::uintptr_t> hook1{ REL::ID(49852) };  // 84AB90
			REL::Relocation<std::uintptr_t> hook2{ REL::ID(49883) };  // 84BCC0, EnterTweenMenuState

			_Update = trampoline.write_call<5>(hook1.address() + 0x1A6, Update);
			_SetCameraState = trampoline.write_call<5>(hook2.address() + 0x7C, SetCameraState);
#endif
		}

	private:
		static void Update(RE::TESCamera* a_this);
		static void SetCameraState(RE::TESCamera* a_this, RE::TESCameraState* a_newState);

		static inline REL::Relocation<decltype(Update)> _Update;
		static inline REL::Relocation<decltype(SetCameraState)> _SetCameraState;
	};

	class MainUpdateHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();

#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook{ REL::ID(36544) };  // 5D29F0, main loop

			_Update = trampoline.write_call<5>(hook.address() + 0x160, Update);
#else
			REL::Relocation<uintptr_t> hook{ REL::ID(35551) };  // 5AF3D0, main loop
			
			_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);
#endif
		}

	private:
		static void Update(RE::Main* a_this, float a2);
		static inline REL::Relocation<decltype(Update)> _Update;
		
	};

	class HorseAimHook
	{
	public:
		static void Hook()
		{
			auto& trampoline = SKSE::GetTrampoline();

#ifdef IS_SKYRIM_AE
			REL::Relocation<uintptr_t> hook1{ REL::ID(43657) };  // 75A890
			REL::Relocation<uintptr_t> hook2{ REL::ID(50896) };  // 87B570
			REL::Relocation<uintptr_t> hook3{ REL::ID(44200) };  // 77EFD0, replace horse aim yaw

			_GetHorseCameraFreeRotationYaw = trampoline.write_call<5>(hook1.address() + 0x1B5, GetHorseCameraFreeRotationYaw);
			//_GetMovementAgentPosition = trampoline.write_call<5>(hook1.address() + 0x22B, GetMovementAgentPosition); // 72FCEB - NPC Z offset after the location is set
			_Func = trampoline.write_call<5>(hook2.address() + 0x45, Func);
			_GetYaw = trampoline.write_call<5>(hook3.address() + 0x1C0, GetYaw);
#else
			REL::Relocation<uintptr_t> hook1{ REL::ID(42496) };  // 72FAC0
			REL::Relocation<uintptr_t> hook2{ REL::ID(49960) };  // 84F490
			REL::Relocation<uintptr_t> hook3{ REL::ID(43009) };  // 7516E0, replace horse aim yaw
			
			_GetHorseCameraFreeRotationYaw = trampoline.write_call<5>(hook1.address() + 0x17A, GetHorseCameraFreeRotationYaw);  // 72FC3A
			//_GetMovementAgentPosition = trampoline.write_call<5>(hook1.address() + 0x22B, GetMovementAgentPosition); // 72FCEB - NPC Z offset after the location is set
			_Func = trampoline.write_call<5>(hook2.address() + 0x45, Func);  // 84F4D5
			_GetYaw = trampoline.write_call<5>(hook3.address() + 0x1C0, GetYaw);  // 7518A0
#endif
		}

	private:
		static float* GetHorseCameraFreeRotationYaw(RE::PlayerCamera* a_this);
		//static void GetMovementAgentPosition(RE::Actor* a_this, RE::NiPoint3& a_pos);
		static void Func(RE::PlayerCamera* a_this);
		static float GetYaw(RE::Actor* a_this);
		
		static inline REL::Relocation<decltype(GetHorseCameraFreeRotationYaw)> _GetHorseCameraFreeRotationYaw;
		//static inline REL::Relocation<decltype(GetMovementAgentPosition)> _GetMovementAgentPosition;
		static inline REL::Relocation<decltype(Func)> _Func;
		static inline REL::Relocation<decltype(GetYaw)> _GetYaw;
	};

	//class CrosshairPickHook
	//{
	//public:
	//	static void Hook()
	//	{
	//		REL::Relocation<uintptr_t> hook{ REL::ID(39534) };  // 6B01E0, PickCrosshairReference

	//		auto& trampoline = SKSE::GetTrampoline();
	//		_Pick = trampoline.write_call<5>(hook.address() + 0x214, Pick);
	//	}

	//private:
	//	static void Pick(uintptr_t a_this, RE::bhkWorld* a_bhkWorld, RE::NiPoint3& a_sourcePoint, RE::NiPoint3& a_sourceRotation);

	//	static inline REL::Relocation<decltype(Pick)> _Pick;
	//};

	void Install();
}
