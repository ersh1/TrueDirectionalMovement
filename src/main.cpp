#include "DirectionalMovementHandler.h"
#include "Events.h"
#include "Hooks.h"
#include "ModAPI.h"
#include "Papyrus.h"
#include "Settings.h"
#include "SmoothCamAPI.h"
#include "TrueHUDAPI.h"
#include "Raycast.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Events::SinkEventHandlers();
		Settings::Initialize();
		Settings::ReadSettings();		
		DirectionalMovementHandler::GetSingleton()->InitCameraModsCompatibility();
		DirectionalMovementHandler::GetSingleton()->Initialize();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		if (!SmoothCamAPI::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(),
				[](void* interfaceInstance, SmoothCamAPI::InterfaceVersion interfaceVersion) {
					if (interfaceVersion == SmoothCamAPI::InterfaceVersion::V3) {
						DirectionalMovementHandler::GetSingleton()->g_SmoothCam = reinterpret_cast<SmoothCamAPI::IVSmoothCam3*>(interfaceInstance);
						logger::info("Obtained SmoothCamAPI");
					} else {
						logger::error("Unable to acquire requested SmoothCamAPI interface version");
					}
				})) {
			logger::warn("SmoothCamAPI::RegisterInterfaceLoaderCallback reported an error");
		}

		if (!TRUEHUD_API::RegisterInterfaceLoaderCallback(SKSE::GetMessagingInterface(),
				[](void* interfaceInstance, TRUEHUD_API::InterfaceVersion interfaceVersion) {
					if (interfaceVersion >= TRUEHUD_API::InterfaceVersion::V1) {
						DirectionalMovementHandler::GetSingleton()->g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD3*>(interfaceInstance);
						logger::info("Obtained TrueHUD API");
					} else {
						logger::error("Unable to acquire requested TrueHUD API interface version");
					}
				})) {
			logger::warn("TRUEHUD_API::RegisterInterfaceLoaderCallback reported an error");
		}

		//// Register the special callback that will handle both types of messages
		//if (!Messaging::RegisterInterfaceListenerCallback(SKSE::GetMessagingInterface(), TRUEHUD_API::TrueHUDPluginName,
		//		[](void* interfaceInstance, uint8_t interfaceVersion) {
		//			if (static_cast<TRUEHUD_API::InterfaceVersion>(interfaceVersion) == TRUEHUD_API::InterfaceVersion::V1) {
		//				DirectionalMovementHandler::GetSingleton()->g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD1*>(interfaceInstance);
		//				logger::info("Obtained TrueHUD V1 API");
		//			} else {
		//				logger::error("Unable to acquire requested TrueHUD API interface version");
		//			}
		//		})) {
		//	logger::warn("Failed to register a callback for TrueHUD");
		//}

		// Register interface request listener for other plugins
		SKSE::GetMessagingInterface()->RegisterListener(nullptr, Messaging::HandleInterfaceRequest);

		break;

	case SKSE::MessagingInterface::kPostPostLoad:
		if (!SmoothCamAPI::RequestInterface(
				SKSE::GetMessagingInterface(),
				SmoothCamAPI::InterfaceVersion::V3))
			logger::warn("SmoothCamAPI::RequestInterface reported an error");
		if (!TRUEHUD_API::RequestInterface(
				SKSE::GetMessagingInterface(),
				TRUEHUD_API::InterfaceVersion::V1))
			logger::warn("TrueHUD API::RequestInterface reported an error");
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		DirectionalMovementHandler::GetSingleton()->OnPreLoadGame();
		break;
	case SKSE::MessagingInterface::kPostLoadGame:
	case SKSE::MessagingInterface::kNewGame:
		Settings::OnPostLoadGame();
	}
}

namespace
{
	void InitializeLog()
	{
#ifndef NDEBUG
		auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
		auto path = logger::log_directory();
		if (!path) {
			util::report_and_fail("Failed to find standard logging directory"sv);
		}

		*path /= fmt::format("{}.log"sv, Plugin::NAME);
		auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

#ifndef NDEBUG
		const auto level = spdlog::level::trace;
#else
		const auto level = spdlog::level::info;
#endif

		auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
		log->set_level(level);
		log->flush_on(level);

		spdlog::set_default_logger(std::move(log));
		spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Plugin::NAME.data();
	a_info->version = Plugin::VERSION[0];

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

#ifdef IS_SKYRIM_AE
extern "C" DLLEXPORT constinit auto SKSEPlugin_Version = []() {
	SKSE::PluginVersionData v;

	v.PluginVersion(Plugin::VERSION);
	v.PluginName(Plugin::NAME);

	v.UsesAddressLibrary(true);
	v.CompatibleVersions({ SKSE::RUNTIME_LATEST });

	return v;
}();
#endif

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
#ifndef NDEBUG
	while (!IsDebuggerPresent()) {}
#endif

	InitializeLog();
	logger::info("{} v{}"sv, Plugin::NAME, Plugin::VERSION.string());

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 9);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	Hooks::Install();
	Papyrus::Register();

	return true;
}
