#include "Events.h"
#include "Settings.h"
#include "DirectionalMovementHandler.h"
#include "Offsets.h"

#include "WidgetHandler.h"

namespace Events
{
	InputEventHandler* InputEventHandler::GetSingleton()
	{
		static InputEventHandler singleton;
		return std::addressof(singleton);
	}

	void InputEventHandler::Register()
	{
		auto deviceManager = RE::BSInputDeviceManager::GetSingleton();
		deviceManager->AddEventSink(InputEventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::InputEvent).name());
	}

	auto InputEventHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
		-> EventResult
	{
		using EventType = RE::INPUT_EVENT_TYPE;
		using DeviceType = RE::INPUT_DEVICE;

		if (!a_event) {
			return EventResult::kContinue;
		}

		for (auto event = *a_event; event; event = event->next) {
			if (event->eventType != EventType::kButton) 
			{
				continue;
			}

			auto button = static_cast<RE::ButtonEvent*>(event);
			if (button->IsDown()) {
				auto key = button->idCode;
				switch (button->device.get()) {
				case DeviceType::kMouse:
					key += kMouseOffset;
					break;
				case DeviceType::kKeyboard:
					key += kKeyboardOffset;
					break;
				case DeviceType::kGamepad:
					key = GetGamepadIndex((RE::BSWin32GamepadDevice::Key)key);
					break;
				default:
					continue;
				}

				auto ui = RE::UI::GetSingleton();
				auto controlMap = RE::ControlMap::GetSingleton();
				if (ui->GameIsPaused() || !controlMap->IsMovementControlsEnabled()) {
					continue;
				}

				if (key == Settings::uTargetLockKey) {
					auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
					directionalMovementHandler->ToggleTargetLock(!directionalMovementHandler->HasTargetLocked());
					break;
				}

				if (key == Settings::uSwitchTargetLeftKey) {
					auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
					if (directionalMovementHandler->HasTargetLocked() && !directionalMovementHandler->ShouldFaceCrosshair()) {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kLeft);
					}
				}

				if (key == Settings::uSwitchTargetRightKey) {
					auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
					if (directionalMovementHandler->HasTargetLocked() && !directionalMovementHandler->ShouldFaceCrosshair()){
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Directions::kRight);
					}
				}
			}

			auto userEvent = event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();
			if (userEvent == userEvents->jump)
			{
				auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
				if (button->IsHeld()) {
					directionalMovementHandler->_pressedDirections.set(DirectionalMovementHandler::Directions::kUp);
				} else {
					directionalMovementHandler->_pressedDirections.reset(DirectionalMovementHandler::Directions::kUp);
				}
			} else if (userEvent == userEvents->sneak) {
				auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
				if (button->IsHeld()) {
					directionalMovementHandler->_pressedDirections.set(DirectionalMovementHandler::Directions::kDown);
				} else {
					directionalMovementHandler->_pressedDirections.reset(DirectionalMovementHandler::Directions::kDown);
				}
			}
		}

		return EventResult::kContinue;
	}

	std::uint32_t InputEventHandler::GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key)
	{
		using Key = RE::BSWin32GamepadDevice::Key;

		std::uint32_t index;
		switch (a_key) 
		{
		case Key::kUp:
			index = 0;
			break;
		case Key::kDown:
			index = 1;
			break;
		case Key::kLeft:
			index = 2;
			break;
		case Key::kRight:
			index = 3;
			break;
		case Key::kStart:
			index = 4;
			break;
		case Key::kBack:
			index = 5;
			break;
		case Key::kLeftThumb:
			index = 6;
			break;
		case Key::kRightThumb:
			index = 7;
			break;
		case Key::kLeftShoulder:
			index = 8;
			break;
		case Key::kRightShoulder:
			index = 9;
			break;
		case Key::kA:
			index = 10;
			break;
		case Key::kB:
			index = 11;
			break;
		case Key::kX:
			index = 12;
			break;
		case Key::kY:
			index = 13;
			break;
		case Key::kLeftTrigger:
			index = 14;
			break;
		case Key::kRightTrigger:
			index = 15;
			break;
		default:
			index = kInvalid;
			break;
		}

		return index != kInvalid ? index + kGamepadOffset : kInvalid;
	}

	CrosshairRefManager* CrosshairRefManager::GetSingleton()
	{
		static CrosshairRefManager singleton;
		return std::addressof(singleton);
	}

	void CrosshairRefManager::Register()
	{
		auto crosshair = SKSE::GetCrosshairRefEventSource();
		crosshair->AddEventSink(CrosshairRefManager::GetSingleton());
		logger::info("Registered {}"sv, typeid(SKSE::CrosshairRefEvent).name());
	}

	RE::ObjectRefHandle CrosshairRefManager::GetCachedRef()
	{
		return _cachedRef;
	}

	EventResult CrosshairRefManager::ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>*)
	{
		_cachedRef = a_event && a_event->crosshairRef ? a_event->crosshairRef->CreateRefHandle() : RE::ObjectRefHandle();

		return EventResult::kContinue;
	}

	EventHandler* EventHandler::GetSingleton()
	{
		static EventHandler singleton;
		return std::addressof(singleton);
	}

	void EventHandler::Register()
	{
		auto scriptEventSourceHolder = RE::ScriptEventSourceHolder::GetSingleton();
		scriptEventSourceHolder->GetEventSource<RE::TESCombatEvent>()->AddEventSink(EventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESCombatEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(EventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESDeathEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESEnterBleedoutEvent>()->AddEventSink(EventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESEnterBleedoutEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESHitEvent>()->AddEventSink(EventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESHitEvent).name());
	}

	bool CheckActorForBoss(RE::Actor* a_actor)
	{
		auto playerCharacter = RE::PlayerCharacter::GetSingleton();
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_actor && playerCharacter && (a_actor != playerCharacter))
		{
			// Check whether the target is even alive or hostile first
			if (a_actor->IsDead() || (a_actor->IsBleedingOut() && a_actor->IsEssential()) || !a_actor->IsHostileToActor(playerCharacter))
			{
				return false;
			}

			// Check blacklist
			if (directionalMovementHandler->GetBossNPCBlacklist().contains(a_actor->GetActorBase()))
			{
				return false;
			}

			// Check race
			if (directionalMovementHandler->GetBossRaces().contains(a_actor->race))
			{
				return true;
			}

			// Check NPC
			if (directionalMovementHandler->GetBossNPCs().contains(a_actor->GetActorBase()))
			{
				return true;
			}

			// Check current loc refs
			if (playerCharacter->currentLocation)
			{
				for (auto& ref : playerCharacter->currentLocation->specialRefs)
				{
					if (ref.type && directionalMovementHandler->GetBossLocRefTypes().contains(ref.type) && ref.refData.refID == a_actor->formID)
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}

	EventResult EventHandler::ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>*)
	{
		if (Settings::bShowBossBar)
		{
			using CombatState = RE::ACTOR_COMBAT_STATE;

			const auto isPlayerRef = [](auto&& a_ref) {
				return a_ref && a_ref->IsPlayerRef();
			};

			if (a_event && a_event->actor && a_event->targetActor && isPlayerRef(a_event->targetActor)) {
				auto actor = a_event->actor->As<RE::Actor>();
				RE::ActorHandle actorHandle = actor->GetHandle();
				if (a_event->newState == CombatState::kCombat) {
					if (CheckActorForBoss(actor)) {
						DirectionalMovementHandler::GetSingleton()->AddBoss(actorHandle);
					}
				} else if (a_event->newState == CombatState::kNone) {
					DirectionalMovementHandler::GetSingleton()->RemoveBoss(actorHandle, true);
				}
			}
		}
		
		return EventResult::kContinue;
	}

	EventResult EventHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && a_event->actorDying && directionalMovementHandler->GetTarget() == a_event->actorDying->GetHandle()) {
			if (directionalMovementHandler->HasTargetLocked()) {
				if (Settings::bAutoTargetNextOnDeath) {
					directionalMovementHandler->ToggleTargetLock(true);
				} else {
					directionalMovementHandler->ToggleTargetLock(false);
				}
			}
			
			if (Settings::bShowBossBar) {
				auto actor = a_event->actorDying->As<RE::Actor>();
				if (actor) {
					directionalMovementHandler->RemoveBoss(actor->GetHandle(), true);
				}
			}
		}

		return EventResult::kContinue;
	}

	EventResult EventHandler::ProcessEvent(const RE::TESEnterBleedoutEvent* a_event, RE::BSTEventSource<RE::TESEnterBleedoutEvent>*)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && a_event->actor && directionalMovementHandler->GetTarget() == a_event->actor->GetHandle()) {
			auto actor = a_event->actor->As<RE::Actor>();
			if (actor && actor->IsEssential())
			{
				if (directionalMovementHandler->HasTargetLocked()) {
					if (Settings::bAutoTargetNextOnDeath) {
						directionalMovementHandler->ToggleTargetLock(true);
					} else {
						directionalMovementHandler->ToggleTargetLock(false);
					}
				}

				if (Settings::bShowBossBar) {
					directionalMovementHandler->RemoveBoss(actor->GetHandle(), true);
				}
			}
		}

		return EventResult::kContinue;
	}

	EventResult EventHandler::ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>*)
	{
		if (Settings::bShowBossBar) {
			if (a_event && a_event->cause && a_event->target) {
				auto causeActor = a_event->cause->As<RE::Actor>();
				auto targetActor = a_event->target->As<RE::Actor>();

				if (causeActor && targetActor) {
					bool bCausePlayer = causeActor->IsPlayerRef();
					bool bTargetPlayer = a_event->target->IsPlayerRef();

					if (bCausePlayer && CheckActorForBoss(targetActor)) {
						DirectionalMovementHandler::GetSingleton()->AddBoss(targetActor->GetHandle());
					} else if (bTargetPlayer && CheckActorForBoss(causeActor)) {
						DirectionalMovementHandler::GetSingleton()->AddBoss(causeActor->GetHandle());
					}
				}
			}
		}	

		return EventResult::kContinue;
	}

	void SinkEventHandlers()
	{
		InputEventHandler::Register();
		CrosshairRefManager::Register();
		EventHandler::Register();
		logger::info("Registered all event handlers"sv);
	}
}
