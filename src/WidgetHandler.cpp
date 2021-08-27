#include "WidgetHandler.h"

#include "Settings.h"
#include "Scaleform/TargetLockMenu.h"
#include "Scaleform/BossMenu.h"

void WidgetHandler::CloseTargetLockMenu()
{
	if (IsTargetLockMenuOpen()) {
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(TargetLockMenu::MenuName(), RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
	}
}

void WidgetHandler::OpenTargetLockMenu()
{
	if (ShouldTargetLockMenuOpen()) {
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(TargetLockMenu::MenuName(), RE::UI_MESSAGE_TYPE::kShow, nullptr);
		}
	}
}

void WidgetHandler::CloseBossMenu()
{
	if (IsBossMenuOpen()) {
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(BossMenu::MenuName(), RE::UI_MESSAGE_TYPE::kHide, nullptr);
		}
	}
}

void WidgetHandler::OpenBossMenu()
{
	if (ShouldBossMenuOpen()) {
		auto msgQ = RE::UIMessageQueue::GetSingleton();
		if (msgQ) {
			msgQ->AddMessage(BossMenu::MenuName(), RE::UI_MESSAGE_TYPE::kShow, nullptr);
		}
	}
}

void WidgetHandler::UpdateVanillaTargetBarState()
{
	bool bVisible;
	if (_hideVanillaTargetBar > 0) {
		bVisible = false;
	} else {
		bVisible = true;
	}

	auto hud = RE::UI::GetSingleton()->GetMenu("HUD Menu");
	if (hud && hud->uiMovie) {
		hud->uiMovie->SetVariable("HUDMovieBaseInstance.EnemyHealth_mc._visible", bVisible);
	}
}

void WidgetHandler::HideVanillaTargetBar()
{
	_hideVanillaTargetBar++;
	UpdateVanillaTargetBarState();
}

void WidgetHandler::ShowVanillaTargetBar()
{
	if (_hideVanillaTargetBar > 0)
	{
		_hideVanillaTargetBar--;
	}
	UpdateVanillaTargetBarState();
}

RE::RefHandle WidgetHandler::GetEnemyHealthTargetRef() const
{
	Locker locker(_lock);
	return _enemyHealthTargetRef;
}

void WidgetHandler::SetEnemyHealthTargetRef(RE::RefHandle a_refHandle)
{
	Locker locker(_lock);
	_enemyHealthTargetRef = a_refHandle;
}

void WidgetHandler::SetTarget(RE::ActorHandle a_target)
{
	if (a_target)
	{
		OpenTargetLockMenu();
	}

	AddTargetLockMenuTask([a_target](TargetLockMenu& a_menu) {
		a_menu.SetTarget(a_target);
	});
}

void WidgetHandler::SetSoftTarget(RE::ActorHandle a_softTarget)
{
	if (!Settings::bShowSoftTargetBar)
	{
		return;
	}

	if (a_softTarget)
	{
		OpenTargetLockMenu();
	}

	AddTargetLockMenuTask([a_softTarget](TargetLockMenu& a_menu) {
		a_menu.SetSoftTarget(a_softTarget);
	});
}

void WidgetHandler::AddBoss(RE::ActorHandle a_boss)
{
	if (!a_boss)
	{
		return;
	}

	OpenBossMenu();

	AddBossMenuTask([a_boss](BossMenu& a_menu) {
		a_menu.AddBoss(a_boss);
	});
}

void WidgetHandler::RemoveBoss(RE::ActorHandle a_boss, bool a_bBossDied)
{
	if (!a_boss) {
		return;
	}

	AddBossMenuTask([a_boss, a_bBossDied](BossMenu& a_menu) {
		a_menu.RemoveBoss(a_boss, a_bBossDied);
	});
}

void WidgetHandler::ProcessTargetLockMenu(TargetLockMenu& a_menu)
{
	if (!_targetLockMenuTaskQueue.empty()) {
		for (auto& task : _targetLockMenuTaskQueue) {
			task(a_menu);
		}
		_targetLockMenuTaskQueue.clear();
	}

	if (_refreshTargetLockMenu) {
		a_menu.RefreshUI();
	}

	_refreshTargetLockMenu = false;
}

void WidgetHandler::ProcessBossMenu(BossMenu& a_menu)
{
	if (!_bossMenuTaskQueue.empty()) {
		for (auto& task : _bossMenuTaskQueue) {
			task(a_menu);
		}
		_bossMenuTaskQueue.clear();
	}

	if (_refreshBossMenu) {
		a_menu.RefreshUI();
	}

	_refreshBossMenu = false;
}

void WidgetHandler::AddTargetLockMenuTask(TargetLockTasklet a_task)
{
	OpenTargetLockMenu();
	Locker locker(_lock);
	_targetLockMenuTaskQueue.push_back(std::move(a_task));
}

void WidgetHandler::AddBossMenuTask(BossTasklet a_task)
{
	OpenBossMenu();
	Locker locker(_lock);
	_bossMenuTaskQueue.push_back(std::move(a_task));
}

auto WidgetHandler::GetTargetLockMenu() const -> RE::GPtr<TargetLockMenu>
{
	auto ui = RE::UI::GetSingleton();
	return ui ? ui->GetMenu<TargetLockMenu>(TargetLockMenu::MenuName()) : nullptr;
}

auto WidgetHandler::GetBossMenu() const->RE::GPtr<BossMenu>
{
	auto ui = RE::UI::GetSingleton();
	return ui ? ui->GetMenu<BossMenu>(BossMenu::MenuName()) : nullptr;
}

bool WidgetHandler::IsTargetLockMenuOpen() const
{
	return static_cast<bool>(GetTargetLockMenu());
}

bool WidgetHandler::IsBossMenuOpen() const
{
	return static_cast<bool>(GetBossMenu());
}

void WidgetHandler::Update()
{
	UpdateVanillaTargetBarState();
}

void WidgetHandler::OnPreLoadGame()
{
	CloseTargetLockMenu();
	CloseBossMenu();
}

void WidgetHandler::OnSettingsUpdated()
{
	RefreshTargetLockMenu();
	RefreshBossMenu();
}

WidgetHandler::WidgetHandler() :
	_lock()
{}
