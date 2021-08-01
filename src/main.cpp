#include "Hooks.h"
#include "Events.h"
#include "DirectionalMovementHandler.h"
#include "Papyrus.h"
#include "ViewHandler.h"
#include "WidgetHandler.h"
#include "Scaleform/Scaleform.h"

enum : std::uint32_t
{
	kSerializationVersion = 1,

	kTrueDirectionalMovement = 'TDMV',
	kDirectionalMovementHandler = 'DMVH',
	kInputEventHandler = 'INPT',
	kTargetLock = 'TLCK',
};

std::string DecodeTypeCode(std::uint32_t a_typeCode)
{
	constexpr std::size_t SIZE = sizeof(std::uint32_t);

	std::string sig;
	sig.resize(SIZE);
	char* iter = reinterpret_cast<char*>(&a_typeCode);
	for (std::size_t i = 0, j = SIZE - 2; i < SIZE - 1; ++i, --j) {
		sig[j] = iter[i];
	}
	return sig;
}

void SaveCallback(SKSE::SerializationInterface* a_intfc)
{
	if (!Events::InputEventHandler::GetSingleton()->Save(a_intfc, kInputEventHandler, kSerializationVersion)) {
		logger::error("Failed to save InputEventHandler!");
	}

	if (!DirectionalMovementHandler::GetSingleton()->Save(a_intfc, kDirectionalMovementHandler, kSerializationVersion)) {
		logger::error("Failed to save DirectionalMovementHandler!");
	}

	if (!WidgetHandler::GetSingleton()->Save(a_intfc, kTargetLock, kSerializationVersion)) {
		logger::error("Failed to save TargetLock!");
	}

	logger::info("Finished saving data");
}

void LoadCallback(SKSE::SerializationInterface* a_intfc)
{
	auto inputEventHandler = Events::InputEventHandler::GetSingleton();
	inputEventHandler->Clear();

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	directionalMovementHandler->Clear();

	auto widgetHandler = WidgetHandler::GetSingleton();
	widgetHandler->Clear();

	std::uint32_t type;
	std::uint32_t version;
	std::uint32_t length;
	while (a_intfc->GetNextRecordInfo(type, version, length)) {
		if (version != kSerializationVersion) {
			logger::error("Loaded data is out of date! Read {}, expected {} for type code {}", version, kSerializationVersion, DecodeTypeCode(type).c_str());
			continue;
		}

		switch (type) {
		case kInputEventHandler:
			logger::error("Reading type {}!", DecodeTypeCode(type).c_str());
			if (!inputEventHandler->Load(a_intfc)) {
				inputEventHandler->Clear();
				logger::error("Failed to load InputEventHandler!");
			}
			break;
		case kDirectionalMovementHandler:
			logger::error("Reading type {}!", DecodeTypeCode(type).c_str());
			if (!directionalMovementHandler->Load(a_intfc)) {
				directionalMovementHandler->Clear();
				logger::error("Failed to load DirectionalMovementHandler!");
			}
			break;
		case kTargetLock:
			logger::error("Reading type {}!", DecodeTypeCode(type).c_str());
			if (!widgetHandler->Load(a_intfc)) {
				widgetHandler->Clear();
				logger::error("Failed to load TargetLock!");
			}
			break;
		default:
			logger::error("Unrecognized record type {}!", DecodeTypeCode(type).c_str());
			break;
		}
	}

	DirectionalMovementHandler::GetSingleton()->Initialize();

	DirectionalMovementHandler::GetSingleton()->OnLoad();
	WidgetHandler::GetSingleton()->OnLoad();

	logger::info("Finished loading data");
}

void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
{
	switch (a_msg->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Events::SinkEventHandlers();
		Scaleform::Register();
		DirectionalMovementHandler::GetSingleton()->LoadIniSettings();
		DirectionalMovementHandler::GetSingleton()->SaveDefaultNearClip();
		break;
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
	SKSE::AllocTrampoline(1 << 7);

	auto messaging = SKSE::GetMessagingInterface();
	if (!messaging->RegisterListener("SKSE", MessageHandler)) {
		return false;
	}

	Hooks::Install();
	Papyrus::Register();

	auto serialization = SKSE::GetSerializationInterface();
	serialization->SetUniqueID(kTrueDirectionalMovement);
	serialization->SetSaveCallback(SaveCallback);
	serialization->SetLoadCallback(LoadCallback);

	return true;
}
