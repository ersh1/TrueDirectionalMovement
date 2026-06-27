#include "APIManager.h"

#include "OARFunctions.h"

template <typename T>
void RegisterFunction()
{
	switch (OAR_API::Functions::AddCustomFunction<T>())
	{
		using enum OAR_API::Functions::APIResult;
	case OK:
		logger::info("Registered {} function!", T::FUNCTION_NAME);
		break;
	case AlreadyRegistered:
		logger::warn("Function {} is already registered!",
			T::FUNCTION_NAME);
		break;
	case Invalid:
		logger::error("Function {} is invalid!", T::FUNCTION_NAME);
		break;
	case Failed:
		logger::error("Failed to register function {}!", T::FUNCTION_NAME);
		break;
	}
}

void APIs::RequestAPIs()
{
	if (!DodgeFramework) {
		DodgeFramework = reinterpret_cast<DODGEFRAMEWORK_API::IVDodgeFramework1*>(DODGEFRAMEWORK_API::RequestPluginAPI(DODGEFRAMEWORK_API::InterfaceVersion::V1));
		if (DodgeFramework) {
			logger::info("Obtained Dodge Framework API - {0:x}", reinterpret_cast<uintptr_t>(DodgeFramework));
		} else {
			logger::warn("Failed to obtain Dodge Framework API");
		}
	}

	if (!SmoothCam) {
		if (!SmoothCamAPI::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(),
				[](void* interfaceInstance, SmoothCamAPI::InterfaceVersion interfaceVersion) {
					if (interfaceVersion == SmoothCamAPI::InterfaceVersion::V3) {
						SmoothCam = reinterpret_cast<SmoothCamAPI::IVSmoothCam3*>(interfaceInstance);
						logger::info("Obtained SmoothCamAPI");
					} else {
						logger::error("Unable to acquire requested SmoothCamAPI interface version");
					}
				})) {
			logger::warn("SmoothCamAPI::RegisterInterfaceLoaderCallback reported an error");
		}

		if (!SmoothCamAPI::RequestInterface(
				SKSE::GetMessagingInterface(),
				SmoothCamAPI::InterfaceVersion::V3)) {
			logger::warn("SmoothCamAPI::RequestInterface reported an error");
		}
	}

	if (!TrueHUD) {
		TrueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
		if (TrueHUD) {
			logger::info("Obtained TrueHUD API - {0:x}", reinterpret_cast<uintptr_t>(TrueHUD));
		} else {
			logger::warn("Failed to obtain TrueHUD API");
		}
	}	

	if (!IDRC) {
		IDRC = reinterpret_cast<IDRC_API::IVIDRC1*>(IDRC_API::RequestPluginAPI(IDRC_API::InterfaceVersion::V1));
		if (IDRC) {
			logger::info("Obtained IDRC API - {0:x}", reinterpret_cast<uintptr_t>(IDRC));
		} else {
			logger::warn("Failed to obtain IDRC API");
		}
	}	

	if (!OAR_Functions) {
		OAR_Functions = GetAPI(OAR_API::Functions::InterfaceVersion::V1);
		if (OAR_Functions) {
			logger::info("Obtained OAR Functions API - {0:x}", reinterpret_cast<uintptr_t>(OAR_Functions));

			RegisterFunction<Functions::ToggleHeadtrackingFunction>();
			RegisterFunction<Functions::ReleaseTargetLockFunction>();
		} else {
			logger::warn("Failed to obtain OAR Functions API");
		}
	}
}
