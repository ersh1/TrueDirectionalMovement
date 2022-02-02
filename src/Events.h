#pragma once

namespace Events
{
	using EventResult = RE::BSEventNotifyControl;

	class InputEventHandler : public RE::BSTEventSink<RE::InputEvent*>
	{
	public:
		static InputEventHandler*	GetSingleton();
		static void					Register();

		virtual EventResult			ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>* a_eventSource) override;

	private:
		enum : std::uint32_t
		{
			kInvalid = static_cast<std::uint32_t>(-1),
			kKeyboardOffset = 0,
			kMouseOffset = 256,
			kGamepadOffset = 266
		};

		InputEventHandler() = default;
		InputEventHandler(const InputEventHandler&) = delete;
		InputEventHandler(InputEventHandler&&) = delete;
		virtual ~InputEventHandler() = default;

		InputEventHandler& operator=(const InputEventHandler&) = delete;
		InputEventHandler& operator=(InputEventHandler&&) = delete;

		std::uint32_t GetGamepadIndex(RE::BSWin32GamepadDevice::Key a_key);
	};

	class CrosshairRefManager : public RE::BSTEventSink<SKSE::CrosshairRefEvent>
	{
	public:
		static CrosshairRefManager*			GetSingleton();
		static void							Register();
		RE::ObjectRefHandle					GetCachedRef();

		virtual EventResult					ProcessEvent(const SKSE::CrosshairRefEvent* a_event, RE::BSTEventSource<SKSE::CrosshairRefEvent>* a_eventSource) override;

	private:
		CrosshairRefManager() = default;
		CrosshairRefManager(const CrosshairRefManager&) = delete;
		CrosshairRefManager(CrosshairRefManager&&) = delete;
		virtual ~CrosshairRefManager() = default;

		CrosshairRefManager& operator=(const CrosshairRefManager&) = delete;
		CrosshairRefManager& operator=(CrosshairRefManager&&) = delete;

		RE::ObjectRefHandle _cachedRef;

	};

	class EventHandler : 
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<RE::TESEnterBleedoutEvent>
	{
	public:
		static EventHandler* GetSingleton();
		static void Register();

		virtual EventResult ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) override;
		virtual EventResult ProcessEvent(const RE::TESEnterBleedoutEvent* a_event, RE::BSTEventSource<RE::TESEnterBleedoutEvent>* a_eventSource) override;

	private:
		EventHandler() = default;
		EventHandler(const EventHandler&) = delete;
		EventHandler(EventHandler&&) = delete;
		virtual ~EventHandler() = default;

		EventHandler& operator=(const EventHandler&) = delete;
		EventHandler& operator=(EventHandler&&) = delete;
	};

	void SinkEventHandlers();
}
