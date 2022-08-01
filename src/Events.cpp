#include "Events.h"
#include "Settings.h"
#include "DirectionalMovementHandler.h"
#include "Offsets.h"

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

			auto& userEvent = event->QUserEvent();
			auto userEvents = RE::UserEvents::GetSingleton();

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
					bool bIgnore = false;

					if (userEvent == userEvents->togglePOV) {
						switch (button->device.get()) {
						case DeviceType::kKeyboard:
							bIgnore = Settings::bTargetLockUsePOVSwitchKeyboard;
							break;
						case DeviceType::kGamepad:
							bIgnore = Settings::bTargetLockUsePOVSwitchGamepad;
							break;
						}
					}

					if (bIgnore) {
						break;
					}

					auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
					directionalMovementHandler->ToggleTargetLock(!directionalMovementHandler->HasTargetLocked(), true);
				}

				if (key == Settings::uSwitchTargetLeftKey) {
					auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
					if (directionalMovementHandler->HasTargetLocked() && !directionalMovementHandler->ShouldFaceCrosshair()) {
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Direction::kLeft);
					}
				}

				if (key == Settings::uSwitchTargetRightKey) {
					auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
					if (directionalMovementHandler->HasTargetLocked() && !directionalMovementHandler->ShouldFaceCrosshair()){
						directionalMovementHandler->SwitchTarget(DirectionalMovementHandler::Direction::kRight);
					}
				}
			}
						
			if (userEvent == userEvents->jump)
			{
				auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
				if (button->IsHeld()) {
					directionalMovementHandler->_pressedDirections.set(DirectionalMovementHandler::Direction::kUp);
				} else {
					directionalMovementHandler->_pressedDirections.reset(DirectionalMovementHandler::Direction::kUp);
				}
			} else if (userEvent == userEvents->sneak) {
				auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
				if (button->IsHeld()) {
					directionalMovementHandler->_pressedDirections.set(DirectionalMovementHandler::Direction::kDown);
				} else {
					directionalMovementHandler->_pressedDirections.reset(DirectionalMovementHandler::Direction::kDown);
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
		scriptEventSourceHolder->GetEventSource<RE::TESDeathEvent>()->AddEventSink(EventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESDeathEvent).name());
		scriptEventSourceHolder->GetEventSource<RE::TESEnterBleedoutEvent>()->AddEventSink(EventHandler::GetSingleton());
		logger::info("Registered {}"sv, typeid(RE::TESEnterBleedoutEvent).name());
	}

	// On death - toggle target lock
	EventResult EventHandler::ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>*)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && a_event->actorDying) {
			if (directionalMovementHandler->HasTargetLocked() && directionalMovementHandler->GetTarget() == a_event->actorDying->GetHandle()) {
				if (Settings::bAutoTargetNextOnDeath) {
					directionalMovementHandler->ToggleTargetLock(true);
				} else {
					directionalMovementHandler->ToggleTargetLock(false);
				}
			}
		}

		return EventResult::kContinue;
	}

	// On bleedout - for essential targets
	EventResult EventHandler::ProcessEvent(const RE::TESEnterBleedoutEvent* a_event, RE::BSTEventSource<RE::TESEnterBleedoutEvent>*)
	{
		auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
		if (a_event && a_event->actor) {
			auto actor = a_event->actor->As<RE::Actor>();
			if (actor && actor->IsEssential())
			{
				if (directionalMovementHandler->HasTargetLocked() && directionalMovementHandler->GetTarget() == a_event->actor->GetHandle()) {
					if (Settings::bAutoTargetNextOnDeath) {
						directionalMovementHandler->ToggleTargetLock(true);
					} else {
						directionalMovementHandler->ToggleTargetLock(false);
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
