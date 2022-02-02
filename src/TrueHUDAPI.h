#pragma once
#include <functional>
#include <queue>
#include <stdint.h>

/*
* For modders: Copy this file into your own project if you wish to use this API
*/
namespace TRUEHUD_API
{
	constexpr const auto TrueHUDPluginName = "TrueHUD";

	// Available True HUD interface versions
	enum class InterfaceVersion : uint8_t
	{
		V1
	};

	// Error types that may be returned by the True HUD
	enum class APIResult : uint8_t
	{
		// Your API call was successful
		OK,

		// You tried to release a resource that was not allocated to you
		// Do not attempt to manipulate the requested resource if you receive this response
		NotOwner,

		// True HUD currently must keep control of this resource for proper functionality
		// Do not attempt to manipulate the requested resource if you receive this response
		MustKeep,

		// You have already been given control of this resource
		AlreadyGiven,

		// Another mod has been given control of this resource at the present time
		// Do not attempt to manipulate the requested resource if you receive this response
		AlreadyTaken,

		// The widget clip failed to load
		WidgetFailedToLoad,

		// You sent a command on a thread that could cause a data race were it to be processed
		// Do not attempt to manipulate the requested resource if you receive this response
		BadThread,
	};

	// Removal modes of the widget when requesting an info bar to be removed
	enum class WidgetRemovalMode : std::uint8_t
	{
		// The widget will be removed instantly
		Immediate,

		// A short fade to zero opacity will start instantly, after which the widget will be removed
		Normal,

		// The fade to zero opacity will start after a short delay (normally used on dead targets)
		Delayed
	};

	// Player widget bar types
	enum class PlayerWidgetBarType : std::uint8_t
	{
		HealthBar,
		MagickaBar,
		StaminaBar,
		SpecialBar
	};

	// Player widget bar color types
	enum class PlayerWidgetBarColorType : std::uint8_t
	{
		BarColor,
		PhantomColor,
		BackgroundColor,
		PenaltyColor,
		FlashColor
	};

	using SpecialResourceCallback = std::function<float(RE::Actor* a_actor)>;
	using APIResultCallback = std::function<void(APIResult)>;

	// Widget base class
	class WidgetBase
	{
	public:
		using WidgetTask = std::function<void()>;
		using Lock = std::recursive_mutex;
		using Locker = std::lock_guard<Lock>;

		enum WidgetState : std::uint8_t
		{
			kActive = 0,
			kPendingHide = 1,
			kHidden = 2,
			kDelayedRemoval = 3,
			kPendingRemoval = 4,
			kRemoved = 5
		};

		WidgetBase() = default;
		virtual ~WidgetBase() = default;

		WidgetBase(RE::GPtr<RE::GFxMovieView> a_view) :
			_view(a_view),
			_widgetID(0)
		{}

		WidgetBase(uint32_t a_widgetID) :
			_widgetID(a_widgetID)
		{}

		WidgetBase(RE::GPtr<RE::GFxMovieView> a_view, uint32_t a_widgetID) :
			_view(a_view),
			_widgetID(a_widgetID)
		{}

		virtual void Update(float a_deltaTime) = 0;
		virtual void Initialize() = 0;
		virtual void Dispose() = 0;
		virtual void SetWidgetState(WidgetState a_newWidgetState)
		{
			_widgetState = a_newWidgetState;
		}

		void AddWidgetTask(WidgetTask a_task)
		{
			Locker locker(_lock);
			_taskQueue.push(std::move(a_task));
		}

		void ProcessDelegates()
		{
			while (!_taskQueue.empty()) {
				auto& task = _taskQueue.front();
				task();
				_taskQueue.pop();
			}
		}

		RE::GPtr<RE::GFxMovieView> _view;
		RE::GFxValue _object;
		uint32_t _widgetID;

		mutable Lock _lock;
		std::queue<WidgetTask> _taskQueue;

		WidgetState _widgetState = WidgetState::kHidden;

		float _depth = 0;
	};

	// True HUD's modder interface
	class IVTrueHUD1
	{
	public:
		/// <summary>
		/// Get the thread ID True HUD Movement is running in.
		/// You may compare this with the result of GetCurrentThreadId() to help determine
		/// if you are using the correct thread.
		/// </summary>
		/// <returns>TID</returns>
		[[nodiscard]] virtual unsigned long GetTrueHUDThreadId() const noexcept = 0;

		/// <summary>
		/// Request control of the current target (affects actor info bar mode)
		/// If granted, you may manipulate the current target and soft target in whatever ways you wish for the duration of your control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, AlreadyGiven, AlreadyTaken</returns>
		[[nodiscard]] virtual APIResult RequestTargetControl(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Request control over the player widget's bar colors.
		/// If granted, you may manipulate the widget's bar colors for the duration of your control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, MustKeep, AlreadyGiven, AlreadyTaken</returns>
		[[nodiscard]] virtual APIResult RequestPlayerWidgetBarColorsControl(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Request control over the special resource bars.
		/// If granted, you may provide the functions that will be used to get the special bars' fill percent.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, AlreadyGiven, AlreadyTaken</returns>
		[[nodiscard]] virtual APIResult RequestSpecialResourceBarsControl(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Tries to set the current target to the given actor handle. Will only do so if granted target control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult SetTarget(SKSE::PluginHandle a_myPluginHandle, RE::ActorHandle a_ActorHandle) noexcept = 0;

		/// <summary>
		/// Tries to set the current soft target to the given actor handle. Will only do so if granted target control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult SetSoftTarget(SKSE::PluginHandle a_myPluginHandle, RE::ActorHandle a_ActorHandle) noexcept = 0;

		/// <summary>
		/// Tries to create an info bar widget for the given actor handle.
		/// </summary>
		/// <param name="a_actorHandle">Actor handle</param>
		virtual void AddActorInfoBar(RE::ActorHandle a_ActorHandle) noexcept = 0;

		/// <summary>
		/// Tries to remove an info bar widget for the given actor handle.
		/// </summary>
		/// <param name="a_actorHandle">Actor handle</param>
		/// <param name="a_removalMode">How quickly should the widget be removed</param>
		virtual void RemoveActorInfoBar(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept = 0;

		/// <summary>
		/// Adds an actor handle to the boss queue.
		/// </summary>
		/// <param name="a_actorHandle">Actor handle</param>
		virtual void AddBoss(RE::ActorHandle a_actorHandle) noexcept = 0;

		/// <summary>
		/// Tries to remove an actor handle from the boss queue / a boss bar widget, if exists.
		/// </summary>
		/// <param name="a_actorHandle">Actor handle</param>
		/// <param name="a_removalMode">How quickly should the widget be removed</param>
		virtual void RemoveBoss(RE::ActorHandle a_actorHandle, WidgetRemovalMode a_removalMode) noexcept = 0;

		/// <summary>
		/// Tries to override the player widget bar color for the given bar type. Will only do so if granted control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_playerWidgetBarType">Bar type</param>
		/// <param name="a_colorType">Which color you want to override</param>
		/// <param name="a_color">The color in hex</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult OverridePlayerWidgetBarColor(SKSE::PluginHandle a_myPluginHandle, PlayerWidgetBarType a_playerWidgetBarType, PlayerWidgetBarColorType a_colorType, uint32_t a_color) noexcept = 0;

		/// <summary>
		/// Tries to revert the player widget bar color for the given bar type. Will only do so if granted control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_playerWidgetBarType">Bar type</param>
		/// <param name="a_colorType">Which color you're overriding</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult RevertPlayerWidgetBarColor(SKSE::PluginHandle a_myPluginHandle, PlayerWidgetBarType a_playerWidgetBarType, PlayerWidgetBarColorType a_colorType) noexcept = 0;

		/// <summary>
		/// Tries to send a visual flash event related to the given actor value on a widget related to the given actor handle (similar to vanilla stamina bar flashing when trying to sprint while it's empty). Will only succeed if such a target exists and is supported by the widget.
		/// </summary>
		/// <param name="a_actorHandle">Actor handle</param>
		/// <param name="a_actorValue">Actor value represented on the bar you want to flash</param>
		/// <param name="a_bLong">Play longer flash animation</param>
		virtual void FlashActorValue(RE::ActorHandle a_actorHandle, RE::ActorValue a_actorValue, bool a_bLong) noexcept = 0;

		/// <summary>
		/// Tries to send a visual flash event on a special bar related to the given actor handle. Will only do so if granted control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_actorHandle">Actor handle</param>
		/// <param name="a_bLong">Play longer flash animation</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult FlashActorSpecialBar(SKSE::PluginHandle a_myPluginHandle, RE::ActorHandle a_actorHandle, bool a_bLong) noexcept = 0;

		/// <summary>
		/// Registers the special resource functions
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_getCurrentSpecialResource">Function that will return current special resource value</param>
		/// <param name="a_getMaxSpecialResource">Function that will return max special resource value</param>
		/// <param name="a_bSpecialMode">Whether the max value is default and 0 is the flash threshold (true), or the other way around (false)</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult RegisterSpecialResourceFunctions(SKSE::PluginHandle a_myPluginHandle, SpecialResourceCallback&& a_getCurrentSpecialResource, SpecialResourceCallback&& a_getMaxSpecialResource, bool a_bSpecialMode) noexcept = 0;

		/// <summary>
		/// Loads a custom widget swf. First step in registering a custom widget.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_filePath">File path to the .swf file, relative to the Data/Interface folder</param>
		/// <param name="a_successCallback">Function that will be called back when done. Check the result before proceeding.</param>
		virtual void LoadCustomWidgets(SKSE::PluginHandle a_myPluginHandle, std::string_view a_filePath, APIResultCallback&& a_successCallback) noexcept = 0;

		/// <summary>
		/// Registers a new widget type. Second step in registering a custom widget.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_widgetType">A pluginwide unique ID of the widget type.</param>
		virtual void RegisterNewWidgetType(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType) noexcept = 0;

		/// <summary>
		/// Adds a custom widget. Will succeed only if prepared correctly by calling the previous two functions.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_widgetType">A pluginwide unique ID of the widget type.</param>
		/// <param name="a_widgetID">An unique ID of the widget.</param>
		/// <param name="a_symbolIdentifier">The ActionScript linkage name of the movieclip</param>
		/// <param name="a_widget">The shared pointer to the widget (based on the WidgetBase class) created in your plugin</param>
		virtual void AddWidget(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, std::string_view a_symbolIdentifier, std::shared_ptr<WidgetBase> a_widget) noexcept = 0;

		/// <summary>
		/// Removes a custom widget.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <param name="a_widgetType">A pluginwide unique ID of the widget type.</param>
		/// <param name="a_widgetID">An unique ID of the widget.</param>
		/// <param name="a_removalMode">Indicates whether the widget should be removed instantly. Modes other than Immediate have to be handled inside the widget</param>
		virtual void RemoveWidget(SKSE::PluginHandle a_myPluginHandle, uint32_t a_widgetType, uint32_t a_widgetID, WidgetRemovalMode a_removalMode) noexcept = 0;

		/// <summary>
		/// Returns the plugin handle of the plugin controlling the current target resource.
		/// </summary>
		/// <returns>Handle or kSKSE::PluginHandle_Invalid if no one currently owns the resource</returns>
		virtual SKSE::PluginHandle GetTargetControlOwner() const noexcept = 0;

		/// <summary>
		/// Returns the plugin handle of the plugin controlling player widget bar colors.
		/// </summary>
		/// <returns>Handle or kSKSE::PluginHandle_Invalid if no one currently owns the resource</returns>
		virtual SKSE::PluginHandle GetPlayerWidgetBarColorsControlOwner() const noexcept = 0;

		/// <summary>
		/// Returns the plugin handle of the plugin controlling special resource bars.
		/// </summary>
		/// <returns>Handle or kSKSE::PluginHandle_Invalid if no one currently owns the resource</returns>
		virtual SKSE::PluginHandle GetSpecialResourceBarControlOwner() const noexcept = 0;

		/// <summary>
		/// Release your control of the target resource.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult ReleaseTargetControl(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Release your control of the player widget's bar colors.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult ReleasePlayerWidgetBarColorsControl(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Release your control of the special resource bars.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult ReleaseSpecialResourceBarControl(SKSE::PluginHandle a_myPluginHandle) noexcept = 0;
	};

	struct InterfaceRequest
	{
		// Version to request
		InterfaceVersion interfaceVersion;
	};

	struct PluginMessage
	{
		// Command types available
		enum class Type : uint8_t
		{
			Error,
			RequestInterface,
			InterfaceProvider
		};

		// Packet header
		uint32_t header = 'THUD';

		// Command type to invoke
		Type type;

		// Pointer to data for the given message
		void* messageData = nullptr;
	};

	struct InterfaceContainer
	{
		// Pointer to interface
		void* interfaceInstance = nullptr;
		// Contained version
		InterfaceVersion interfaceVersion;
	};

	using InterfaceLoaderCallback = std::function<void(
		void* interfaceInstance, InterfaceVersion interfaceVersion)>;

	/// <summary>
	/// Initiate a request for the True HUD API interface via SKSE's messaging system.
	/// You must register a callback to obtain the response to this request.
	/// Recommended: Send your request during SKSEMessagingInterface::kMessage_PostPostLoad
	/// </summary>
	/// <param name="skseMessaging">SKSE's messaging interface</param>
	/// <param name="version">The interface version to request</param>
	/// <returns>If any plugin was listening for this request, true. See skse/PluginAPI.h</returns>
	[[nodiscard]] inline bool RequestInterface(const SKSE::MessagingInterface* a_skseMessaging,
		InterfaceVersion a_version = InterfaceVersion::V1) noexcept
	{
		InterfaceRequest req = {};
		req.interfaceVersion = a_version;

		PluginMessage cmd = {};
		cmd.type = PluginMessage::Type::RequestInterface;
		cmd.messageData = &req;

		return a_skseMessaging->Dispatch(
			0,
			&cmd, sizeof(PluginMessage),
			TrueHUDPluginName);
	}

	/// <summary>
	/// Register the callback for obtaining the True HUD API interface. Call only once.
	/// Recommended: Register your callback during SKSEMessagingInterface::kMessage_PostLoad
	/// </summary>
	/// <param name="skseMessaging">SKSE's messaging interface</param>
	/// <param name="callback">A callback function receiving both the interface pointer and interface version</param>
	/// <returns></returns>
	[[nodiscard]] inline bool RegisterInterfaceLoaderCallback(const SKSE::MessagingInterface* a_skseMessaging,
		InterfaceLoaderCallback&& a_callback) noexcept
	{
		static InterfaceLoaderCallback storedCallback = a_callback;

		return a_skseMessaging->RegisterListener(
			TrueHUDPluginName,
			[](SKSE::MessagingInterface::Message* msg) {
				if (msg->sender && strcmp(msg->sender, TrueHUDPluginName) != 0)
					return;
				if (msg->type != 0)
					return;
				if (msg->dataLen != sizeof(PluginMessage))
					return;

				const auto resp = reinterpret_cast<PluginMessage*>(msg->data);
				switch (resp->type) {
				case PluginMessage::Type::InterfaceProvider:
					{
						auto interfaceContainer = reinterpret_cast<InterfaceContainer*>(resp->messageData);
						storedCallback(
							interfaceContainer->interfaceInstance,
							interfaceContainer->interfaceVersion);
						break;
					}
				case PluginMessage::Type::Error:
					{
						SKSE::log::info("TrueHUDAPI: Error obtaining interface");
						break;
					}
				default:
					return;
				}
			});
	}
}
