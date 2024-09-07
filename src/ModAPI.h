#pragma once
#include "TrueDirectionalMovementAPI.h"

class DirectionalMovementHandler;

namespace Messaging
{
	using APIResult = ::TDM_API::APIResult;
	using InterfaceVersion1 = ::TDM_API::IVTDM1;
	using InterfaceVersion2 = ::TDM_API::IVTDM2;
	using InterfaceVersion3 = ::TDM_API::IVTDM3;

	class TDMInterface : public InterfaceVersion3
	{
	private:
		TDMInterface() noexcept;
		virtual ~TDMInterface() noexcept;

	public:
		static TDMInterface* GetSingleton() noexcept
		{
			static TDMInterface singleton;
			return std::addressof(singleton);
		}

		// InterfaceVersion1
		virtual unsigned long GetTDMThreadId() const noexcept override;
		virtual bool GetDirectionalMovementState() const noexcept override;
		virtual bool GetTargetLockState() const noexcept override;
		virtual RE::ActorHandle GetCurrentTarget() const noexcept override;
		virtual APIResult RequestDisableDirectionalMovement(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult RequestDisableHeadtracking(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual SKSE::PluginHandle GetDisableDirectionalMovementOwner() const noexcept override;
		virtual SKSE::PluginHandle GetDisableHeadtrackingOwner() const noexcept override;
		virtual APIResult ReleaseDisableDirectionalMovement(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult ReleaseDisableHeadtracking(SKSE::PluginHandle a_modHandle) noexcept override;

		// InterfaceVersion2
		virtual APIResult RequestYawControl(SKSE::PluginHandle a_modHandle, float a_yawRotationSpeedMultiplier) noexcept override;
		virtual APIResult SetPlayerYaw(SKSE::PluginHandle a_modHandle, float a_desiredYaw) noexcept override;
		virtual APIResult ReleaseYawControl(SKSE::PluginHandle a_modHandle) noexcept override;

		// InterfaceVersion3
		virtual TDM_API::DirectionalMovementMode GetDirectionalMovementMode() const noexcept override;
		virtual RE::NiPoint2 GetActualMovementInput() const noexcept override;

		// Internal
		// Mark directional movement control as required by True Directional Movement for API requests
		void SetNeedsDirectionalMovementControl(bool a_needsControl) noexcept;
		// Mark headtracking control as required by True Directional Movement for API requests
		void SetNeedsHeadtrackingControl(bool a_needsControl) noexcept;
		// Mark player yaw control as required by True Directional Movement for API requests
		void SetNeedsYawControl(bool a_needsControl) noexcept;

		// Does a mod have control over the directional movement?
		bool IsDirectionalMovementControlTaken() const noexcept;
		// Does a mod have control over the headtracking?
		bool IsHeadtrackingControlTaken() const noexcept;
		// Does a mod have control over the player character's yaw?
		bool IsYawControlTaken() const noexcept;

	private:
		unsigned long apiTID = 0;

		bool needsDirectionalMovementControl = false;
		std::atomic<SKSE::PluginHandle> directionalMovementOwner = SKSE::kInvalidPluginHandle;

		bool needsHeadtrackingControl = false;
		std::atomic<SKSE::PluginHandle> headtrackingOwner = SKSE::kInvalidPluginHandle;

		bool needsYawControl = false;
		std::atomic<SKSE::PluginHandle> yawOwner = SKSE::kInvalidPluginHandle;
	};
}
