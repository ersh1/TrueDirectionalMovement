#include "ModAPI.h"
#include "DirectionalMovementHandler.h"

Messaging::TDMInterface::TDMInterface() noexcept
{
	apiTID = GetCurrentThreadId();
}

Messaging::TDMInterface::~TDMInterface() noexcept {}

unsigned long Messaging::TDMInterface::GetTDMThreadId() const noexcept
{
	return apiTID;
}

bool Messaging::TDMInterface::GetDirectionalMovementState() noexcept
{
	if (directionalMovementHandler) {
		return directionalMovementHandler->IsFreeCamera();
	}
	return false;
}

bool Messaging::TDMInterface::GetTargetLockState() noexcept
{
	if (directionalMovementHandler) {
		return directionalMovementHandler->HasTargetLocked();
	}
	return false;
}

RE::ActorHandle Messaging::TDMInterface::GetCurrentTarget() noexcept
{
	if (directionalMovementHandler) {
		return directionalMovementHandler->GetTarget();
	}
	return RE::ActorHandle();
}

Messaging::APIResult Messaging::TDMInterface::RequestDisableDirectionalMovement(SKSE::PluginHandle a_modHandle) noexcept
{
	const auto owner = directionalMovementOwner.load(std::memory_order::memory_order_acquire);
	if (owner != SKSE::kInvalidPluginHandle)
		if (owner == a_modHandle)
			return APIResult::AlreadyGiven;
		else
			return APIResult::AlreadyTaken;

	if (needsDirectionalMovementControl)
		return APIResult::MustKeep;
	auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
	if (!directionalMovementOwner.compare_exchange_strong(expected, a_modHandle, std::memory_order::memory_order_acq_rel))
		return APIResult::AlreadyTaken;

	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableDirectionalMovement(true);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::RequestDisableHeadtracking(SKSE::PluginHandle modHandle) noexcept
{
	const auto owner = headtrackingOwner.load(std::memory_order::memory_order_acquire);
	if (owner != SKSE::kInvalidPluginHandle)
		if (owner == modHandle)
			return APIResult::AlreadyGiven;
		else
			return APIResult::AlreadyTaken;

	if (needsHeadtrackingControl)
		return APIResult::MustKeep;
	auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
	if (!headtrackingOwner.compare_exchange_strong(expected, modHandle, std::memory_order::memory_order_acq_rel))
		return APIResult::AlreadyTaken;

	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableHeadtracking(true);
	}

	return APIResult::OK;
}

SKSE::PluginHandle Messaging::TDMInterface::GetDisableDirectionalMovementOwner() const noexcept
{
	return directionalMovementOwner;
}

SKSE::PluginHandle Messaging::TDMInterface::GetDisableHeadtrackingOwner() const noexcept
{
	return headtrackingOwner;
}

Messaging::APIResult Messaging::TDMInterface::ReleaseDisableDirectionalMovement(SKSE::PluginHandle modHandle) noexcept
{
	if (directionalMovementOwner != modHandle)
		return APIResult::NotOwner;
	directionalMovementOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableDirectionalMovement(false);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::ReleaseDisableHeadtracking(SKSE::PluginHandle modHandle) noexcept
{
	if (headtrackingOwner != modHandle)
		return APIResult::NotOwner;
	headtrackingOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableHeadtracking(false);
	}

	return APIResult::OK;
}

void Messaging::TDMInterface::SetDirectionalMovementHandler(DirectionalMovementHandler* a_directionalMovementHandler)
{
	directionalMovementHandler = a_directionalMovementHandler;
}

void Messaging::TDMInterface::SetNeedsDirectionalMovementControl(bool a_needsControl) noexcept
{
	needsDirectionalMovementControl = a_needsControl;
}

void Messaging::TDMInterface::SetNeedsHeadtrackingControl(bool a_needsControl) noexcept
{
	needsHeadtrackingControl = a_needsControl;
}

bool Messaging::TDMInterface::IsDirectionalMovementControlTaken() const noexcept
{
	return directionalMovementOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
}

bool Messaging::TDMInterface::IsHeadtrackingControlTaken() const noexcept
{
	return headtrackingOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
}

void Messaging::TDMInterface::RegisterConsumer(const char* a_modName) noexcept
{
	consumers.push_back(std::move(std::string(a_modName)));
	logger::info(FMT_STRING("Added API consumer '{}'"), a_modName);
}

const Messaging::TDMInterface::Consumers& Messaging::TDMInterface::GetConsumers() const noexcept
{
	return consumers;
}

void Messaging::HandleInterfaceRequest(SKSE::MessagingInterface::Message* a_msg) noexcept
{
	if (a_msg->type != 0)
		return;
	constexpr const auto DispatchToPlugin = [](TDM_API::PluginMessage* packet, const char* to) noexcept {
		if (!SKSE::GetMessagingInterface()->Dispatch(0, packet, sizeof(TDM_API::PluginMessage), to))
			logger::warn(FMT_STRING("Failed to dispatch API message to '{}'"), to ? to : "unnamed");
	};

	TDM_API::PluginMessage packet = {};
	packet.type = TDM_API::PluginMessage::Type::Error;

	if (a_msg->dataLen != sizeof(TDM_API::PluginMessage)) {
		DispatchToPlugin(&packet, a_msg->sender);
		return;
	}

	const auto cmd = reinterpret_cast<const TDM_API::PluginMessage*>(a_msg->data);

	if (cmd->header != 'TDMV' || cmd->type != TDM_API::PluginMessage::Type::RequestInterface) {
		//DispatchToPlugin(&packet, a_msg->sender);
		return;
	}

	const auto request = reinterpret_cast<const TDM_API::InterfaceRequest*>(cmd->messageData);
	if (!(request->interfaceVersion == TDM_API::InterfaceVersion::V1)) {
		DispatchToPlugin(&packet, a_msg->sender);
		return;
	}

	auto api = Messaging::TDMInterface::GetInstance();
	if (a_msg->sender)
		api->RegisterConsumer(a_msg->sender);
	else
		logger::info("Added unnamed API consumer");

	TDM_API::InterfaceContainer container = {};
	container.interfaceVersion = request->interfaceVersion;

	switch (request->interfaceVersion) {
	case TDM_API::InterfaceVersion::V1:
		container.interfaceInstance = static_cast<void*>(api);
		break;
	default:
		api->RegisterConsumer(a_msg->sender);
		return;
	}

	packet.type = TDM_API::PluginMessage::Type::InterfaceProvider;
	packet.messageData = &container;
	DispatchToPlugin(&packet, a_msg->sender);
}

bool Messaging::RegisterInterfaceListenerCallback(const SKSE::MessagingInterface* skseMessaging, const char* sender, InterfaceLoaderCallback&& callback) noexcept
{
	static InterfaceLoaderCallback storedCallback = callback;

	return skseMessaging->RegisterListener(sender, [](SKSE::MessagingInterface::Message* msg) {
		if (msg->type != 0) {
			return;
		}

		if (msg->dataLen == sizeof(TDM_API::PluginMessage)) {
			const auto resp = reinterpret_cast<TDM_API::PluginMessage*>(msg->data);
			if (resp->type == TDM_API::PluginMessage::Type::InterfaceProvider) {
				auto interfaceContainer = reinterpret_cast<InterfaceContainer*>(resp->messageData);
				storedCallback(
					interfaceContainer->interfaceInstance,
					static_cast<uint8_t>(interfaceContainer->interfaceVersion));
				return;
			}
		}

		HandleInterfaceRequest(msg);
	});
}
