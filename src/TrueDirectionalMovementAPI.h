#pragma once
#include <functional>
#include <stdint.h>

/*
* For modders: Copy this file into your own project if you wish to use this API
* 
* #define TDM_API_SKSE if using SKSE
* #define TDM_API_COMMONLIB if using CommonLibSSE
*/
	namespace TDM_API
{
	constexpr const auto TDMPluginName = "TrueDirectionalMovement";

	using PluginHandle = SKSE::PluginHandle;
	using ActorHandle = RE::ActorHandle;

	// Available True Directional Movement interface versions
	enum class InterfaceVersion : uint8_t
	{
		V1
	};

	// Error types that may be returned by the True Directional Movement API
	enum class APIResult : uint8_t
	{
		// Your API call was successful
		OK,

		// You tried to release a resource that was not allocated to you
		// Do not attempt to manipulate the requested resource if you receive this response
		NotOwner,

		// True Directional Movement currently must keep control of this resource for proper functionality
		// Do not attempt to manipulate the requested resource if you receive this response
		MustKeep,

		// You have already been given control of this resource
		AlreadyGiven,

		// Another mod has been given control of this resource at the present time
		// Do not attempt to manipulate the requested resource if you receive this response
		AlreadyTaken,

		// You sent a command on a thread that could cause a data race were it to be processed
		// Do not attempt to manipulate the requested resource if you receive this response
		BadThread,
	};

	// True Directional Movement's modder interface
	class IVTDM1
	{
	public:
		/// <summary>
		/// Get the thread ID True Directional Movement is running in.
		/// You may compare this with the result of GetCurrentThreadId() to help determine
		/// if you are using the correct thread.
		/// </summary>
		/// <returns>TID</returns>
		[[nodiscard]] virtual unsigned long GetTDMThreadId() const noexcept = 0;

		/// <summary>
		/// Get the current state (enabled / disabled) of directional movement.
		/// </summary>
		/// <returns>The current state (true / false) of directional movement</returns>
		[[nodiscard]] virtual bool GetDirectionalMovementState() noexcept = 0;

		/// <summary>
		/// Get the current state (enabled / disabled) of target lock.
		/// </summary>
		/// <returns>The current state (true / false) of target lock</returns>
		[[nodiscard]] virtual bool GetTargetLockState() noexcept = 0;

		/// <summary>
		/// Get the actor handle of the currently locked target.
		/// </summary>
		/// <returns>The actor handle of the currently locked target</returns>
		[[nodiscard]] virtual ActorHandle GetCurrentTarget() noexcept = 0;

		/// <summary>
		/// Request the plugin to forcibly disable directional movement.
		/// If granted, directional movement will be disabled for the duration of your control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, MustKeep, AlreadyGiven, AlreadyTaken</returns>
		[[nodiscard]] virtual APIResult RequestDisableDirectionalMovement(PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Request the plugin to forcibly disable headtracking.
		/// If granted, this mod's headtracking will be disabled for the duration of your control.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, MustKeep, AlreadyGiven, AlreadyTaken</returns>
		[[nodiscard]] virtual APIResult RequestDisableHeadtracking(PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Returns the current owner of the forced disable of directional movement.
		/// </summary>
		/// <returns>Handle or kPluginHandle_Invalid if no one currently owns the resource</returns>
		virtual PluginHandle GetDisableDirectionalMovementOwner() const noexcept = 0;

		/// <summary>
		/// Returns the current owner of the forced disable of headtracking.
		/// </summary>
		/// <returns>Handle or kPluginHandle_Invalid if no one currently owns the resource</returns>
		virtual PluginHandle GetDisableHeadtrackingOwner() const noexcept = 0;

		/// <summary>
		/// Release your forced disable of directional movement.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult ReleaseDisableDirectionalMovement(PluginHandle a_myPluginHandle) noexcept = 0;

		/// <summary>
		/// Release your forced disable of headtracking.
		/// </summary>
		/// <param name="a_myPluginHandle">Your assigned plugin handle</param>
		/// <returns>OK, NotOwner</returns>
		virtual APIResult ReleaseDisableHeadtracking(PluginHandle a_myPluginHandle) noexcept = 0;
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
		uint32_t header = 'TDMV';

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
	/// Initiate a request for the True Directional Movement API interface via SKSE's messaging system.
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

		PluginMessage msg = {};
		msg.type = PluginMessage::Type::RequestInterface;
		msg.messageData = &req;

		return a_skseMessaging->Dispatch(
			0,
			&msg, sizeof(PluginMessage),
			TDMPluginName);
	}

	/// <summary>
	/// Register the callback for obtaining the True Directional Movement API interface. Call only once.
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
			TDMPluginName,
			[](SKSE::MessagingInterface::Message* msg) {
				if (msg->sender && strcmp(msg->sender, TDMPluginName) != 0)
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
						SKSE::log::info("TrueDirectionalMovementAPI: Error obtaining interface");
						break;
					}
				default:
					return;
				}
			});
	}
}
