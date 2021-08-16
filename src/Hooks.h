#pragma once

namespace Hooks
{
	class MovementHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> MovementHandlerVtbl { RE::Offset::MovementHandler::Vtbl };
			_ProcessThumbstick = MovementHandlerVtbl.write_vfunc(0x2, ProcessThumbstick);
			_ProcessButton = MovementHandlerVtbl.write_vfunc(0x4, ProcessButton);
		}

	private:
		static void ProcessThumbstick(RE::MovementHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data);
		static void ProcessButton(RE::MovementHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(ProcessThumbstick)> _ProcessThumbstick;
		static inline REL::Relocation<decltype(ProcessButton)> _ProcessButton;
	};

	class LookHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> LookHandlerVtbl{ RE::Offset::LookHandler::Vtbl };
			_ProcessThumbstick = LookHandlerVtbl.write_vfunc(0x2, ProcessThumbstick);
			_ProcessMouseMove = LookHandlerVtbl.write_vfunc(0x3, ProcessMouseMove);
		}

	private:
		static void ProcessThumbstick(RE::LookHandler* a_this, RE::ThumbstickEvent* a_event, RE::PlayerControlsData* a_data);
		static void ProcessMouseMove(RE::LookHandler* a_this, RE::MouseMoveEvent* a_event, RE::PlayerControlsData* a_data);

		static inline REL::Relocation<decltype(ProcessThumbstick)> _ProcessThumbstick;
		static inline REL::Relocation<decltype(ProcessMouseMove)> _ProcessMouseMove;
	};

	class FirstPersonStateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> FirstPersonStateVtbl{ RE::Offset::FirstPersonState::Vtbl };
			_OnEnterState = FirstPersonStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = FirstPersonStateVtbl.write_vfunc(0x2, OnExitState);
			REL::Relocation<std::uintptr_t> PlayerInputHandlerVtbl{ REL::ID(267811) };
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
			REL::Relocation<std::uintptr_t> ThirdPersonStateVtbl{ RE::Offset::ThirdPersonState::Vtbl };
			_OnEnterState = ThirdPersonStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = ThirdPersonStateVtbl.write_vfunc(0x2, OnExitState);
			_SetFreeRotationMode = ThirdPersonStateVtbl.write_vfunc(0xD, SetFreeRotationMode);
			REL::Relocation<std::uintptr_t> PlayerInputHandlerVtbl{ REL::ID(256648) };
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
			REL::Relocation<std::uintptr_t> HorseCameraStateVtbl{ REL::ID(267749) };
			_OnEnterState = HorseCameraStateVtbl.write_vfunc(0x1, OnEnterState);
			_OnExitState = HorseCameraStateVtbl.write_vfunc(0x2, OnExitState);
			_UpdateRotation = HorseCameraStateVtbl.write_vfunc(0xE, UpdateRotation);
			_HandleLookInput = HorseCameraStateVtbl.write_vfunc(0xF, HandleLookInput);
			REL::Relocation<std::uintptr_t> PlayerInputHandlerVtbl{ REL::ID(267750) };
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
			REL::Relocation<std::uintptr_t> TweenMenuCameraStateVtbl{ REL::ID(267922) };
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
			REL::Relocation<std::uintptr_t> VATSCameraStateVtbl{ REL::ID(267929) };
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
			REL::Relocation<std::uintptr_t> PlayerCameraTransitionStateVtbl{ REL::ID(267818) };
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
			REL::Relocation<std::uintptr_t> MovementHandlerAgentPlayerControlsVtbl{ REL::ID(262625) }; // 166ABA0
			_Func1 = MovementHandlerAgentPlayerControlsVtbl.write_vfunc(0x2, Func1);
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
			REL::Relocation<std::uintptr_t> ProjectileVtbl{ REL::ID(264058) };				// 167C888
			REL::Relocation<std::uintptr_t> ArrowProjectileVtbl{ REL::ID(263776) };			// 1676318
			REL::Relocation<std::uintptr_t> MissileProjectileVtbl{ REL::ID(263942) };		// 167AE78
			REL::Relocation<std::uintptr_t> BeamProjectileVtbl{ REL::ID(263808) };          // 1677660
			_GetLinearVelocity = ProjectileVtbl.write_vfunc(0x86, GetLinearVelocity);
			ArrowProjectileVtbl.write_vfunc(0x86, GetLinearVelocity);
			MissileProjectileVtbl.write_vfunc(0x86, GetLinearVelocity);

			REL::Relocation<uintptr_t> hook{ REL::ID(static_cast<std::uint64_t>(43030)) };  // 754820
			auto& trampoline = SKSE::GetTrampoline();
			_Func183 = trampoline.write_call<5>(hook.address() + 0x304, Func183);
		}
	private:
		static void GetLinearVelocity(RE::Projectile* a_this, RE::NiPoint3& a_outVelocity);
		static void UpdateImpl(RE::Projectile* a_this);

		static void Func183(RE::Projectile* a_this);

		static inline REL::Relocation<decltype(GetLinearVelocity)> _GetLinearVelocity;

		static inline REL::Relocation<decltype(Func183)> _Func183;
	};

	class PlayerCharacterHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> PlayerCharacterVtbl{ RE::Offset::PlayerCharacter::Vtbl };				// 1665E0
			_ProcessTracking = PlayerCharacterVtbl.write_vfunc(0x122, ProcessTracking);
			REL::Relocation<std::uintptr_t> PlayerCharacter_IAnimationGraphManagerHolderVtbl{ REL::ID(261918) };	// 1663F78
			_ProcessEvent = PlayerCharacter_IAnimationGraphManagerHolderVtbl.write_vfunc(0x1, ProcessEvent);
			REL::Relocation<std::uintptr_t> PlayerCharacter_ActorStateVtbl{ REL::ID(261922) };						// 16640E8
			_GetAngle = PlayerCharacter_ActorStateVtbl.write_vfunc(0x4, GetAngle);

			REL::Relocation<uintptr_t> SprintHook{ REL::ID(static_cast<std::uint64_t>(39673)) };					// 6B8F20
			auto& trampoline = SKSE::GetTrampoline();
			trampoline.write_branch<6>(SprintHook.address(), Sprint);
		}

	private:
		static void ProcessTracking(RE::Actor* a_this, float a_delta, RE::NiAVObject* a_obj3D);
		static void ProcessEvent(RE::BSTEventSink<RE::BSAnimationGraphEvent>* a_this, const RE::BSAnimationGraphEvent* a_event, RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_dispatcher);
		static void GetAngle(RE::ActorState* a_this, RE::NiPoint3 &a_angle);
		static void Sprint(RE::PlayerCharacter* a_this);

		static inline REL::Relocation<decltype(ProcessTracking)> _ProcessTracking;
		static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;
		static inline REL::Relocation<decltype(GetAngle)> _GetAngle;
	};

	class PlayerControlsHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<std::uintptr_t> PlayerControlsVtbl{ REL::ID(262983) };  // 166E838
			_Handle = PlayerControlsVtbl.write_vfunc(0x1, Handle);

			auto& trampoline = SKSE::GetTrampoline();
			REL::Relocation<uintptr_t> hook{ REL::ID(static_cast<std::uint64_t>(41259)) }; // 704DE0
			_CanProcessControls = trampoline.write_call<5>(hook.address() + 0x2E, CanProcessControls);
		}

	private:
		static bool Handle(RE::PlayerControls* a_this, uintptr_t a2);
		static bool CanProcessControls(RE::PlayerControls* a_this, RE::InputEvent** a_eventPtr);
		static inline REL::Relocation<decltype(Handle)> _Handle;
		static inline REL::Relocation<decltype(CanProcessControls)> _CanProcessControls;

	};

	class AIProcess_SetRotationSpeedZHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook{ REL::ID(static_cast<std::uint64_t>(38848)) }; // 67D0C0

			auto& trampoline = SKSE::GetTrampoline();
			trampoline.write_branch<6>(hook.address(), AIProcess_SetRotationSpeedZ);
		}

	private:
		static void AIProcess_SetRotationSpeedZ(RE::AIProcess* a_this, float a_rotationSpeed);
	};

	class Actor_SetRotationHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook1{ REL::ID(static_cast<std::uint64_t>(32042)) }; // 4EC300  // synchronized anims
			REL::Relocation<uintptr_t> hook2{ REL::ID(static_cast<std::uint64_t>(36365)) }; // 5D87F0
			
			auto& trampoline = SKSE::GetTrampoline();
			_Actor_SetRotationX = trampoline.write_call<5>(hook1.address() + 0x4DC, Actor_SetRotationX);
			_Actor_SetRotationZ = trampoline.write_call<5>(hook2.address() + 0x9C7, Actor_SetRotationZ);
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
			REL::Relocation<std::uintptr_t> EnemyHealthVtbl{ REL::ID(268826) };  // 16B2A50
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
			REL::Relocation<uintptr_t> hook1{ REL::ID(static_cast<std::uint64_t>(36984)) };  // 60D300
			REL::Relocation<uintptr_t> hook2{ REL::ID(static_cast<std::uint64_t>(36376)) };  // 5D9BF0
			REL::Relocation<uintptr_t> hook3{ REL::ID(static_cast<std::uint64_t>(36220)) };  // 5CFD60
			REL::Relocation<uintptr_t> hook4{ REL::ID(static_cast<std::uint64_t>(36540)) };  // 5E8070

			auto& trampoline = SKSE::GetTrampoline();
			_SetHeadtrackTarget0 = trampoline.write_call<5>(hook1.address() + 0x592, SetHeadtrackTarget0);
			_SetHeadtrackTarget4 = trampoline.write_call<5>(hook2.address() + 0xA3, SetHeadtrackTarget4);
			trampoline.write_call<5>(hook3.address() + 0x45C, SetHeadtrackTarget4);
			trampoline.write_call<5>(hook3.address() + 0x5BF, SetHeadtrackTarget4);
			trampoline.write_call<5>(hook4.address() + 0x17E, SetHeadtrackTarget4);
		}

	private:
		static void SetHeadtrackTarget0(RE::AIProcess* a_this, RE::Actor* a_target);
		static void SetHeadtrackTarget4(RE::AIProcess* a_this, RE::Actor* a_target);
		static inline REL::Relocation<decltype(SetHeadtrackTarget0)> _SetHeadtrackTarget0;
		static inline REL::Relocation<decltype(SetHeadtrackTarget4)> _SetHeadtrackTarget4;
	};

	class NukeSetIsNPCHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook1{ REL::ID(55694) };  // 996FD0, bool papyrus wrapper
			//REL::Relocation<uintptr_t> hook{ REL::ID(32141) };  // 4F06E0
			REL::Relocation<uintptr_t> hook2{ REL::ID(55695) };  // 997090, int papyrus wrapper

			auto& trampoline = SKSE::GetTrampoline();
			_SetBool = trampoline.write_call<5>(hook1.address() + 0x4C, SetBool);
			//_SetBool = trampoline.write_call<5>(hook.address() + 0xE, SetBool);
			_SetInt = trampoline.write_call<5>(hook2.address() + 0x4B, SetInt);
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
			REL::Relocation<std::uintptr_t> hook{ REL::ID(49852) };  // 84AB90

			auto& trampoline = SKSE::GetTrampoline();
			_Update = trampoline.write_call<5>(hook.address() + 0x1A6, Update);
		}

	private:
		static void Update(RE::TESCamera* a_this);
		static inline REL::Relocation<decltype(Update)> _Update;
	};

	class MainUpdateHook
	{
	public:
		static void Hook()
		{
			REL::Relocation<uintptr_t> hook{ REL::ID(35551) };  // 5AF3D0, main loop

			auto& trampoline = SKSE::GetTrampoline();
			_Update = trampoline.write_call<5>(hook.address() + 0x11F, Update);
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
			REL::Relocation<uintptr_t> hook1{ REL::ID(42496) };  // 72FAC0
			REL::Relocation<uintptr_t> hook2{ REL::ID(49960) };  // 84F490

			auto& trampoline = SKSE::GetTrampoline();
			_GetHorseCameraFreeRotationYaw = trampoline.write_call<5>(hook1.address() + 0x17A, GetHorseCameraFreeRotationYaw);
			_Func = trampoline.write_call<5>(hook2.address() + 0x45, Func);
		}

	private:
		static float* GetHorseCameraFreeRotationYaw(RE::PlayerCamera* a_this);
		static void Func(RE::PlayerCamera* a_this);
		
		static inline REL::Relocation<decltype(GetHorseCameraFreeRotationYaw)> _GetHorseCameraFreeRotationYaw;
		static inline REL::Relocation<decltype(Func)> _Func;

	};

	void Install();
}
