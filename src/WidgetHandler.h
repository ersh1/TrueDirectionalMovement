#pragma once

namespace Scaleform
{
	class TargetLockMenu;
	class BossMenu;
}

namespace Events
{
	class EventHandler;
}

// Interface for interacting with the menus outside of UI threads
class WidgetHandler
{
private:
	using TargetLockMenu = Scaleform::TargetLockMenu;
	using BossMenu = Scaleform::BossMenu;

public:
	static WidgetHandler* GetSingleton()
	{
		static WidgetHandler singleton;
		return std::addressof(singleton);
	}

	void RefreshTargetLockMenu()
	{
		auto task = SKSE::GetTaskInterface();
		task->AddTask([this]() {
			_refreshTargetLockMenu = true;
		});
	}

	void RefreshBossMenu()
	{
		auto task = SKSE::GetTaskInterface();
		task->AddTask([this]() {
			_refreshBossMenu = true;
		});
	}

	void CloseTargetLockMenu();
	void OpenTargetLockMenu();

	void CloseBossMenu();
	void OpenBossMenu();

	void UpdateVanillaTargetBarState();
	void HideVanillaTargetBar();
	void ShowVanillaTargetBar();

	RE::RefHandle GetEnemyHealthTargetRef() const;
	void SetEnemyHealthTargetRef(RE::RefHandle a_refHandle);

	void SetTarget(RE::ActorHandle a_target);
	void SetSoftTarget(RE::ActorHandle a_softTarget);

	void AddBoss(RE::ActorHandle a_boss);
	void RemoveBoss(RE::ActorHandle a_boss, bool a_bBossDied);

	void Update();

	void OnPreLoadGame();

	void OnSettingsUpdated();

protected:
	friend class TargetLockMenu;
	friend class BossMenu;

	void ProcessTargetLockMenu(TargetLockMenu& a_menu);
	void ProcessBossMenu(BossMenu& a_menu);

private:
	using TargetLockTasklet = std::function<void(TargetLockMenu&)>;
	using BossTasklet = std::function<void(BossMenu&)>;
	using Lock = std::recursive_mutex;
	using Locker = std::lock_guard<Lock>;

	WidgetHandler();
	WidgetHandler(const WidgetHandler&) = delete;
	WidgetHandler(WidgetHandler&&) = delete;

	~WidgetHandler() = default;

	WidgetHandler& operator=(const WidgetHandler&) = delete;
	WidgetHandler& operator=(WidgetHandler&&) = delete;

	friend class DirectionalMovementHandler;
	friend class Events::EventHandler;

	void AddTargetLockMenuTask(TargetLockTasklet a_task);
	void AddBossMenuTask(BossTasklet a_task);

	[[nodiscard]] RE::GPtr<TargetLockMenu> GetTargetLockMenu() const;
	[[nodiscard]] RE::GPtr<BossMenu> GetBossMenu() const;
	[[nodiscard]] bool IsTargetLockMenuOpen() const;
	[[nodiscard]] bool IsBossMenuOpen() const;

	[[nodiscard]] bool ShouldTargetLockMenuOpen() const
	{
		//if (!_enabled /* || IsOpen() */) {
		//	return false;
		//}

		/*if (RE::UI::GetSingleton()->IsMenuOpen(RE::TweenMenu::MENU_NAME)) {
			return false;
		}*/

		return true;
	}

	[[nodiscard]] bool ShouldBossMenuOpen() const
	{
		if (IsBossMenuOpen()) {
			return false;
		}

		/*if (RE::UI::GetSingleton()->IsMenuOpen(RE::TweenMenu::MENU_NAME)) {
			return false;
		}*/

		return true;
	}

	mutable Lock _lock;
	std::vector<TargetLockTasklet> _targetLockMenuTaskQueue;
	std::vector<BossTasklet> _bossMenuTaskQueue;
	uint8_t _hideVanillaTargetBar = 0;
	bool _refreshTargetLockMenu { false };
	bool _refreshBossMenu { false };
	RE::RefHandle _enemyHealthTargetRef;

	bool _targetLockMenuHidden { false };
	bool _bossMenuHidden { false };
};
