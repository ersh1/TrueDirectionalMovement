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

		bool						Save(const SKSE::SerializationInterface* a_intfc, std::uint32_t a_typeCode, std::uint32_t a_version);
		bool						Load(const SKSE::SerializationInterface* a_intfc);
		void						Clear();
		std::uint32_t				GetTargetLockKey() const;
		void						SetTargetLockKey(std::uint32_t a_key);
		std::uint32_t				GetSwitchTargetLeftKey() const;
		void						SetSwitchTargetLeftKey(std::uint32_t a_key);
		std::uint32_t				GetSwitchTargetRightKey() const;
		void						SetSwitchTargetRightKey(std::uint32_t a_key);

	private:
		using Lock = std::recursive_mutex;
		using Locker = std::lock_guard<Lock>;

		enum : std::uint32_t
		{
			kInvalid = static_cast<std::uint32_t>(-1),
			kKeyboardOffset = 0,
			kMouseOffset = 256,
			kGamepadOffset = 266
		};

		InputEventHandler();
		InputEventHandler(const InputEventHandler&) = delete;
		InputEventHandler(InputEventHandler&&) = delete;
		virtual ~InputEventHandler() = default;

		InputEventHandler& operator=(const InputEventHandler&) = delete;
		InputEventHandler& operator=(InputEventHandler&&) = delete;

		mutable Lock _lock;
		std::uint32_t _targetLockKey;
		std::uint32_t _switchTargetLeftKey;
		std::uint32_t _switchTargetRightKey;

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
		public RE::BSTEventSink<RE::TESCombatEvent>,
		public RE::BSTEventSink<RE::TESDeathEvent>,
		public RE::BSTEventSink<RE::TESEnterBleedoutEvent>,
		public RE::BSTEventSink<RE::TESHitEvent>
	{
	public:
		static EventHandler* GetSingleton();
		static void Register();

		virtual EventResult ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_eventSource) override;
		virtual EventResult ProcessEvent(const RE::TESDeathEvent* a_event, RE::BSTEventSource<RE::TESDeathEvent>* a_eventSource) override;
		virtual EventResult ProcessEvent(const RE::TESEnterBleedoutEvent* a_event, RE::BSTEventSource<RE::TESEnterBleedoutEvent>* a_eventSource) override;
		virtual EventResult ProcessEvent(const RE::TESHitEvent* a_event, RE::BSTEventSource<RE::TESHitEvent>* a_eventSource) override;

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
