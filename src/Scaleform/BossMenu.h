#pragma once

#include "ViewHandler.h"

namespace Scaleform
{
	class BossMenu : public RE::IMenu
	{
	private:
		using Super = RE::IMenu;

	public:
		enum BossState : std::uint8_t
		{
			kNone = 0,
			kUninitialized,
			kInitialized,
			kDying
		};

		struct BossBar
		{
			RE::ActorHandle boss;
			RE::GFxValue bar;
			BossState bossState = kNone;
			float updateTimer = 0.f;
			float hideTimer = 0.f;
			float healthPercent = -1.f;
			float prevHealth = -1.f;
			float damage = 0.f;
			int32_t spotNumber = -1;

			BossBar(RE::GFxValue a_bossBar) :
				boss(RE::ActorHandle()),
				bar(a_bossBar),
				bossState(kNone),
				updateTimer(0.f),
				hideTimer(0.f),
				healthPercent(-1.f),
				prevHealth(-1.f),
				damage(0.f),
				spotNumber(-1)
			{}

			~BossBar() noexcept = default;

			inline bool operator==(const RE::ActorHandle& a_boss) noexcept
			{
				return boss == a_boss;
			}

			void Clear()
			{
				boss = RE::ActorHandle();
				bossState = kNone;
				updateTimer = 0.f;
				hideTimer = 0.f;
				healthPercent = -1.f;
				prevHealth = -1.f;
				damage = 0.f;
				spotNumber = -1;
				bar.Invoke("hideDamage");
			}
		};

		static constexpr std::string_view MenuName() noexcept { return MENU_NAME; }
		static constexpr std::int8_t SortPriority() noexcept { return SORT_PRIORITY; }

		static void Register();

		void AddBoss(RE::ActorHandle a_boss);
		void RemoveBoss(RE::ActorHandle a_boss, bool a_bBossDied);

		void Update();

		void RefreshUI();

		bool IsDisplayingBoss(RE::ActorHandle a_boss) const;

	protected:
		using UIResult = RE::UI_MESSAGE_RESULTS;

		BossMenu()
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

			RE::GFxValue _bossBar0;
			RE::GFxValue _bossBar1;
			RE::GFxValue _bossBar2;
			_viewHandler.emplace(menu, ViewHandler::MenuType::kBossMenu);
			_view = menu->uiMovie;
			_view->SetMouseCursorCount(0);	// disable input
			_view->GetVariable(&_root, "_root");
			_view->GetVariable(&_bossList, "_root.BossList");
			_view->GetVariable(&_bossBar0, "_root.BossList.Boss0");
			_view->GetVariable(&_bossBar1, "_root.BossList.Boss1");
			_view->GetVariable(&_bossBar2, "_root.BossList.Boss2");

			_bossBars.emplace_back(_bossBar0);
			_bossBars.emplace_back(_bossBar1);
			_bossBars.emplace_back(_bossBar2);

			_occupiedBarSpots.resize(_bossBars.size(), false);
		}

		BossMenu(const BossMenu&) = default;
		BossMenu(BossMenu&&) = default;

		~BossMenu() = default;

		BossMenu& operator=(const BossMenu&) = default;
		BossMenu& operator=(BossMenu&&) = default;

		static RE::stl::owner<RE::IMenu*> Creator() { return new BossMenu(); }

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
			AssignBossTargetsFromQueue();
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

				logger::info("{}: {}"sv, BossMenu::MenuName(), buf.data());
			}
		};

		void UpdateBossHealth(BossBar& a_bossBar, bool bForceDead = false);

		void KillBoss(BossBar& a_bossBar);

		void AssignBossTargetsFromQueue();

		void OnOpen();

		void OnClose();

		void ProcessDelegate();

		float GetSubtitleOffset() const;

		RE::GFxValue _root;
		RE::GFxValue _bossList;

		std::vector<bool> _occupiedBarSpots;

		std::list<RE::ActorHandle> _bossQueue;
		std::vector<BossBar> _bossBars;

		bool _bSubtitleYSaved = false;
		RE::GFxValue _savedSubtitleY;

		bool _bVanillaTargetBarHidden = false;

		static constexpr std::string_view FILE_NAME{ "TrueDirectionalMovement/TDM_Boss" };
		static constexpr std::string_view MENU_NAME{ "BossMenu" };
		static constexpr std::int8_t SORT_PRIORITY{ 0 };

		static constexpr uint32_t _equalColor = 0xCBCBCB;
		static constexpr uint32_t _weakerColor = 0x1E88E5;
		static constexpr uint32_t _strongerColor = 0xDF2020;

		RE::GPtr<RE::GFxMovieView> _view;
		std::optional<ViewHandler> _viewHandler;
	};
}
