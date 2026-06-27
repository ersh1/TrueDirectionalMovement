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

bool Messaging::TDMInterface::GetDirectionalMovementState() const noexcept
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		return directionalMovementHandler->IsFreeCamera();
	}
	return false;
}

bool Messaging::TDMInterface::GetTargetLockState() const noexcept
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		return directionalMovementHandler->HasTargetLocked();
	}
	return false;
}

bool Messaging::TDMInterface::IsTargetLockBehindTarget() const noexcept
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		return directionalMovementHandler->IsTargetLockBehindTarget();
	}
	return false;
}

RE::ActorHandle Messaging::TDMInterface::GetCurrentTarget() const noexcept
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
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

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableDirectionalMovement(true);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::RequestDisableTargetLock(SKSE::PluginHandle a_modHandle) noexcept
{
	const auto owner = targetLockReticleOwner.load(std::memory_order::memory_order_acquire);
	if (owner != SKSE::kInvalidPluginHandle)
		if (owner == a_modHandle)
			return APIResult::AlreadyGiven;
		else
			return APIResult::AlreadyTaken;

	if (needsTargetLockControl)
		return APIResult::MustKeep;
	auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
	if (!targetLockReticleOwner.compare_exchange_strong(expected, a_modHandle, std::memory_order::memory_order_acq_rel))
		return APIResult::AlreadyTaken;

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableTargetLock(true);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::RequestDisableHeadtracking(SKSE::PluginHandle a_modHandle) noexcept
{
	const auto owner = headtrackingOwner.load(std::memory_order::memory_order_acquire);
	if (owner != SKSE::kInvalidPluginHandle)
		if (owner == a_modHandle)
			return APIResult::AlreadyGiven;
		else
			return APIResult::AlreadyTaken;

	if (needsHeadtrackingControl)
		return APIResult::MustKeep;
	auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
	if (!headtrackingOwner.compare_exchange_strong(expected, a_modHandle, std::memory_order::memory_order_acq_rel))
		return APIResult::AlreadyTaken;

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableHeadtracking(true);
	}

	return APIResult::OK;
}

SKSE::PluginHandle Messaging::TDMInterface::GetDisableDirectionalMovementOwner() const noexcept
{
	return directionalMovementOwner;
}

SKSE::PluginHandle Messaging::TDMInterface::GetDisableTargetLockOwner() const noexcept
{
	return targetLockReticleOwner;
}

SKSE::PluginHandle Messaging::TDMInterface::GetDisableHeadtrackingOwner() const noexcept
{
	return headtrackingOwner;
}

Messaging::APIResult Messaging::TDMInterface::ReleaseDisableDirectionalMovement(SKSE::PluginHandle a_modHandle) noexcept
{
	if (directionalMovementOwner != a_modHandle)
		return APIResult::NotOwner;
	directionalMovementOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableDirectionalMovement(false);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::ReleaseDisableTargetLock(SKSE::PluginHandle a_modHandle) noexcept
{
	if (targetLockReticleOwner != a_modHandle)
		return APIResult::NotOwner;
	targetLockReticleOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableTargetLock(false);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::ReleaseDisableHeadtracking(SKSE::PluginHandle a_modHandle) noexcept
{
	if (headtrackingOwner != a_modHandle)
		return APIResult::NotOwner;
	headtrackingOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetForceDisableHeadtracking(false);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::RequestYawControl(SKSE::PluginHandle a_modHandle, float a_yawRotationSpeedMultiplier) noexcept
{
	const auto owner = yawOwner.load(std::memory_order::memory_order_acquire);
	if (owner != SKSE::kInvalidPluginHandle)
		if (owner == a_modHandle)
			return APIResult::AlreadyGiven;
		else
			return APIResult::AlreadyTaken;

	if (needsYawControl)
		return APIResult::MustKeep;
	auto expected = static_cast<SKSE::PluginHandle>(SKSE::kInvalidPluginHandle);
	if (!yawOwner.compare_exchange_strong(expected, a_modHandle, std::memory_order::memory_order_acq_rel))
		return APIResult::AlreadyTaken;

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetYawControl(true, a_yawRotationSpeedMultiplier);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::SetPlayerYaw(SKSE::PluginHandle a_modHandle, float a_desiredYaw) noexcept
{
	if (yawOwner != a_modHandle)
		return APIResult::NotOwner;

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetPlayerYaw(a_desiredYaw);
	}

	return APIResult::OK;
}

Messaging::APIResult Messaging::TDMInterface::ReleaseYawControl(SKSE::PluginHandle a_modHandle) noexcept
{
	if (yawOwner != a_modHandle)
		return APIResult::NotOwner;
	yawOwner.store(SKSE::kInvalidPluginHandle, std::memory_order::memory_order_release);

	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	if (directionalMovementHandler) {
		directionalMovementHandler->SetYawControl(false);
	}

	return APIResult::OK;
}

TDM_API::DirectionalMovementMode Messaging::TDMInterface::GetDirectionalMovementMode() const noexcept
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	return static_cast<TDM_API::DirectionalMovementMode>(directionalMovementHandler->GetDirectionalMovementMode());
}

RE::NiPoint2 Messaging::TDMInterface::GetActualMovementInput() const noexcept
{
	auto directionalMovementHandler = DirectionalMovementHandler::GetSingleton();
	return directionalMovementHandler->GetActualInputDirection();
}

void Messaging::TDMInterface::SetNeedsDirectionalMovementControl(bool a_needsControl) noexcept
{
	needsDirectionalMovementControl = a_needsControl;
}

void Messaging::TDMInterface::SetNeedsTargetLockControl(bool a_needsControl) noexcept
{
	needsTargetLockControl = a_needsControl;
}

void Messaging::TDMInterface::SetNeedsHeadtrackingControl(bool a_needsControl) noexcept
{
	needsHeadtrackingControl = a_needsControl;
}

void Messaging::TDMInterface::SetNeedsYawControl(bool a_needsControl) noexcept
{
	needsYawControl = a_needsControl;
}

bool Messaging::TDMInterface::IsDirectionalMovementControlTaken() const noexcept
{
	return directionalMovementOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
}

bool Messaging::TDMInterface::IsTargetLockControlTaken() const noexcept
{
	return targetLockReticleOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
}

bool Messaging::TDMInterface::IsHeadtrackingControlTaken() const noexcept
{
	return headtrackingOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
}

bool Messaging::TDMInterface::IsYawControlTaken() const noexcept
{
	return yawOwner.load(std::memory_order::memory_order_acquire) != SKSE::kInvalidPluginHandle;
}
