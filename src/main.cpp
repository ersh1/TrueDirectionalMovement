#include "DirectionalMovementHandler.h"
#include "Events.h"
#include "Hooks.h"
#include "Papyrus.h"
#include "Settings.h"
#include "SmoothCamAPI.h"
#include "ViewHandler.h"
#include "WidgetHandler.h"
#include "Scaleform/Scaleform.h"

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Events::SinkEventHandlers();
		Scaleform::Register();
		Settings::ReadSettings();
		DirectionalMovementHandler::GetSingleton()->LoadIniSettings();
		DirectionalMovementHandler::GetSingleton()->InitCameraModsCompatibility();
		DirectionalMovementHandler::GetSingleton()->Initialize();
		break;
	case SKSE::MessagingInterface::kPostLoad:
		if (!SmoothCamAPI::RegisterInterfaceLoaderCallback(
				SKSE::GetMessagingInterface(),
				[](void* interfaceInstance, SmoothCamAPI::InterfaceVersion interfaceVersion) {
					if (interfaceVersion == SmoothCamAPI::InterfaceVersion::V1) {
						DirectionalMovementHandler::GetSingleton()->g_SmoothCam = reinterpret_cast<SmoothCamAPI::IVSmoothCam1*>(interfaceInstance);
						logger::info("Obtained SmoothCamAPI");
					} else
						logger::error("Unable to acquire requested SmoothCamAPI interface version");
				}))
			logger::warn("SmoothCamAPI::RegisterInterfaceLoaderCallback reported an error");
		break;

	case SKSE::MessagingInterface::kPostPostLoad:
		if (!SmoothCamAPI::RequestInterface(
				SKSE::GetMessagingInterface(),
				SmoothCamAPI::InterfaceVersion::V1))
			logger::warn("SmoothCamAPI::RequestInterface reported an error");
		break;
	case SKSE::MessagingInterface::kPreLoadGame:
		DirectionalMovementHandler::GetSingleton()->OnPreLoadGame();
		WidgetHandler::GetSingleton()->OnPreLoadGame();
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef NDEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef NDEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

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

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	logger::info("TrueDirectionalMovement loaded");

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 8);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	Hooks::Install();
	Papyrus::Register();

	return true;
}
