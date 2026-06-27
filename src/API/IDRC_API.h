#pragma once
#include <functional>
#include <stdint.h>

/*
* For modders: Copy this file into your own project if you wish to use this API
*/
namespace IDRC_API {
	constexpr const auto IDRCPluginName = "IntuitiveDragonRideControl";

	// Available IDRC interface versions
	enum class InterfaceVersion : uint8_t {
		V1
	};

	// IDRC's modder interface
	class IVIDRC1 {
	public:
		/// <summary>
		/// Get the thread ID IDRC is running in.
		/// You may compare this with the result of GetCurrentThreadId() to help determine
		/// if you are using the correct thread.
		/// </summary>
		/// <returns>TID</returns>
		[[nodiscard]] virtual unsigned long GetIDRCThreadId() const noexcept = 0;

		/// <summary>
		/// Get the actor handle of the the dragon's current target. If case no dragon is currently being ridden, this will return an empty handle.
		/// </summary>
		/// <returns>The actor handle of the dragon's current target, or an empty handle if the dragon is not in combat, or no dragon is ridden.</returns>
		[[nodiscard]] virtual RE::ActorHandle GetCurrentTarget() const noexcept = 0;

		/// <summary>
		/// Propagates IDRC setting which defines if the dragon's current target should be used by other mods. Used in TrueDirectionalMovement
		/// </summary>
		/// <returns>True if current target (obtained via GetCurrentTarget() should be used according to IDRC's settings.</returns>
		[[nodiscard]] virtual bool UseTarget() const noexcept = 0;

		/// <summary>
		/// Get the actor handle of the the currently ridden dragon. If case no dragon is currently being ridden, this will return an empty handle.
		/// </summary>
		/// <returns>The actor handle of the currently ridden dragon, or an empty handle if no dragon is ridden.</returns>
		[[nodiscard]] virtual RE::ActorHandle GetDragon() const noexcept = 0;
	};

	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interfaceVersion);

	/// <summary>
	/// Request the IDRC API interface.
	/// Recommended: Send your request during or after SKSEMessagingInterface::kMessage_PostLoad to make sure the dll has already been loaded
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or nullptr if request failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::V1) {
		auto pluginHandle = GetModuleHandle("IntuitiveDragonRideControl.dll");
		_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
		if (requestAPIFunction) {
			return requestAPIFunction(a_interfaceVersion);
		}
		return nullptr;
	}
}
