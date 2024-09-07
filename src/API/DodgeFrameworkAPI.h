#pragma once

/*
* For modders: Copy this file into your own project if you wish to use this API
*/
namespace DODGEFRAMEWORK_API
{
	constexpr const auto DodgeFrameworkPluginName = "DodgeFramework";

	// Available Dodge Framework interface versions
	enum class InterfaceVersion : uint8_t
	{
		V1
	};

	// Dodge Framework's modder interface
	class IVDodgeFramework1
	{
	public:
		// just a stub, API is WIP
	};

	typedef void* (*_RequestPluginAPI)(const InterfaceVersion interfaceVersion);

	/// <summary>
	/// Request the Dodge Framework API interface.
	/// Recommended: Send your request when you need to use the API and cache the pointer. SKSEMessagingInterface::kMessage_PostLoad seems to be unreliable for some users for unknown reasons.
	/// </summary>
	/// <param name="a_interfaceVersion">The interface version to request</param>
	/// <returns>The pointer to the API singleton, or nullptr if request failed</returns>
	[[nodiscard]] inline void* RequestPluginAPI(const InterfaceVersion a_interfaceVersion = InterfaceVersion::V1)
	{
		auto pluginHandle = GetModuleHandleA("DodgeFramework.dll");
		_RequestPluginAPI requestAPIFunction = (_RequestPluginAPI)GetProcAddress(pluginHandle, "RequestPluginAPI");
		if (requestAPIFunction) {
			return requestAPIFunction(a_interfaceVersion);
		}
		return nullptr;
	}
}
