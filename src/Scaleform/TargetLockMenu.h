#pragma once

#include "ViewHandler.h"

namespace Scaleform
{
	class TargetLockMenu : public RE::IMenu
	{
	private:
		using Super = RE::IMenu;

	public:
		static constexpr std::string_view MenuName() noexcept { return MENU_NAME; }
		static constexpr std::int8_t SortPriority() noexcept { return SORT_PRIORITY; }

		static void Register();

		void SetTarget(RE::ActorHandle a_target);
		void SetSoftTarget(RE::ActorHandle a_softTarget);

		void Update();

		void RefreshUI();

	protected:
		using UIResult = RE::UI_MESSAGE_RESULTS;

		TargetLockMenu()
		{
			auto menu = static_cast<Super*>(this);
			menu->depthPriority = SortPriority();
			auto scaleformManager = RE::BSScaleformManager::GetSingleton();
			[[maybe_unused]] const auto success =
				scaleformManager->LoadMovieEx(menu, FILE_NAME, [](RE::GFxMovieDef* a_def) -> void {
					a_def->SetState(
						RE::GFxState::StateType::kLog,
						RE::make_gptr<Logger>().get());
				});

			assert(success);
			menuFlags.set(RE::UI_MENU_FLAGS::kAllowSaving);

			_viewHandler.emplace(menu, ViewHandler::MenuType::kTargetLockMenu);
			_view = menu->uiMovie;
			_view->SetMouseCursorCount(0);	// disable input
			_view->GetVariable(&_root, "_root");
			_view->GetVariable(&_reticleHolder, "_root.ReticleHolder");
			_view->GetVariable(&_reticle, "_root.ReticleHolder.Reticle");
			_view->GetVariable(&_targetBar, "_root.TargetBar");
		}

		TargetLockMenu(const TargetLockMenu&) = default;
		TargetLockMenu(TargetLockMenu&&) = default;

		~TargetLockMenu() = default;

		TargetLockMenu& operator=(const TargetLockMenu&) = default;
		TargetLockMenu& operator=(TargetLockMenu&&) = default;

		static RE::stl::owner<RE::IMenu*> Creator() { return new TargetLockMenu(); }

		// IMenu
		void PostCreate() override { OnOpen(); }

		UIResult ProcessMessage(RE::UIMessage& a_message) override
		{
			using Type = RE::UI_MESSAGE_TYPE;

			switch (*a_message.type) {
			case Type::kShow:
				OnOpen();
				return Super::ProcessMessage(a_message);
			case Type::kHide:
				OnClose();
				return UIResult::kHandled;
			default:
				return Super::ProcessMessage(a_message);
			}
		}

		void AdvanceMovie(float a_interval, std::uint32_t a_currentTime) override
		{
			ProcessDelegate();
			Update();
			Super::AdvanceMovie(a_interval, a_currentTime);
		}

	private:
		class Logger : public RE::GFxLog
		{
		public:
			void LogMessageVarg(LogMessageType, const char* a_fmt, std::va_list a_argList) override
			{
				std::string fmt(a_fmt ? a_fmt : "");
				while (!fmt.empty() && fmt.back() == '\n') {
					fmt.pop_back();
				}

				std::va_list args;
				va_copy(args, a_argList);
				std::vector<char> buf(static_cast<std::size_t>(std::vsnprintf(0, 0, fmt.c_str(), a_argList) + 1));
				std::vsnprintf(buf.data(), buf.size(), fmt.c_str(), args);
				va_end(args);

				logger::info("{}: {}"sv, TargetLockMenu::MenuName(), buf.data());
			}
		};

		void OnOpen();

		void OnClose();

		void ProcessDelegate();

		void NewTarget(RE::ActorHandle a_newTarget);

		RE::GFxValue _root;
		RE::GFxValue _reticleHolder;
		RE::GFxValue _reticle;
		RE::GFxValue _targetBar;
		RE::ActorHandle _currentTarget;
		RE::ActorHandle _target;
		RE::ActorHandle _softTarget;
		bool _bIsLockOn;
		bool _bInitialized = false;
		float _barUpdateTimer = 0.f;
		float _healthPercent = -1.f;
		float _prevTargetHealth = -1.f;
		float _damage = 0.f;

		static constexpr std::string_view FILE_NAME{ "TrueDirectionalMovement/TDM_TargetLock" };
		static constexpr std::string_view MENU_NAME{ "TargetLockMenu" };
		static constexpr std::int8_t SORT_PRIORITY{ 0 };

		static constexpr uint32_t _equalColor = 0xCBCBCB;
		static constexpr uint32_t _weakerColor = 0x1E88E5;
		static constexpr uint32_t _weakerColorOutline = 0x666666;
		static constexpr uint32_t _strongerColor = 0xDF2020;

		RE::GPtr<RE::GFxMovieView> _view;
		std::optional<ViewHandler> _viewHandler;
	};
}
