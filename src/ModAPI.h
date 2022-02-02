#pragma once
#include "TrueDirectionalMovementAPI.h"

class DirectionalMovementHandler;

namespace Messaging
{
	using APIResult = ::TDM_API::APIResult;
	using InterfaceVersion1 = ::TDM_API::IVTDM1;
	using InterfaceContainer = ::TDM_API::InterfaceContainer;

	using InterfaceLoaderCallback = std::function<void(void* interfaceInstance, uint8_t interfaceVersion)>;

	class TDMInterface : public InterfaceVersion1
	{
	private:
		TDMInterface() noexcept;
		virtual ~TDMInterface() noexcept;

	public:
		static TDMInterface* GetInstance() noexcept
		{
			static TDMInterface instance;
			return &instance;
		}

		// InterfaceVersion1
		virtual unsigned long GetTDMThreadId() const noexcept override;
		virtual bool GetDirectionalMovementState() noexcept override;
		virtual bool GetTargetLockState() noexcept override;
		virtual RE::ActorHandle GetCurrentTarget() noexcept override;
		virtual APIResult RequestDisableDirectionalMovement(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult RequestDisableHeadtracking(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual SKSE::PluginHandle GetDisableDirectionalMovementOwner() const noexcept override;
		virtual SKSE::PluginHandle GetDisableHeadtrackingOwner() const noexcept override;
		virtual APIResult ReleaseDisableDirectionalMovement(SKSE::PluginHandle a_modHandle) noexcept override;
		virtual APIResult ReleaseDisableHeadtracking(SKSE::PluginHandle a_modHandle) noexcept override;

		// Internal
		// Provide the directional movement handler for API requests
		void SetDirectionalMovementHandler(DirectionalMovementHandler* a_directionalMovementHandler);

		// Mark directional movement control as required by True Directional Movement for API requests
		void SetNeedsDirectionalMovementControl(bool a_needsControl) noexcept;
		// Mark headtracking control as required by True Directional Movement for API requests
		void SetNeedsHeadtrackingControl(bool a_needsControl) noexcept;

		// Does a mod have control over the directional movement?
		bool IsDirectionalMovementControlTaken() const noexcept;
		// Does a mod have control over the headtracking?
		bool IsHeadtrackingControlTaken() const noexcept;

	public:
		using Consumers = std::vector<std::string>;

		void RegisterConsumer(const char* modName) noexcept;
		const Consumers& GetConsumers() const noexcept;

	private:
		DirectionalMovementHandler* directionalMovementHandler = nullptr;
		Consumers consumers = {};
		unsigned long apiTID = 0;

		bool needsDirectionalMovementControl = false;
		std::atomic<SKSE::PluginHandle> directionalMovementOwner = SKSE::kInvalidPluginHandle;

		bool needsHeadtrackingControl = false;
		std::atomic<SKSE::PluginHandle> headtrackingOwner = SKSE::kInvalidPluginHandle;
	};

	void HandleInterfaceRequest(SKSE::MessagingInterface::Message* a_msg) noexcept;
	bool RegisterInterfaceListenerCallback(const SKSE::MessagingInterface* skseMessaging, const char* sender, InterfaceLoaderCallback&& callback) noexcept;
}
