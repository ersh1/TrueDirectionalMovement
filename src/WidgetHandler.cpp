#include "WidgetHandler.h"

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
	if (!_bShowSoftTargetBar)
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

bool WidgetHandler::GetShowReticle() const
{
	Locker locker(_lock);
	return _bShowReticle;
}

void WidgetHandler::SetShowReticle(bool a_show)
{
	Locker locker(_lock);
	_bShowReticle = a_show;

	RefreshTargetLockMenu();
}

WidgetHandler::ReticleStyle WidgetHandler::GetReticleStyle() const
{
	Locker locker(_lock);
	return _reticleStyle;
}

void WidgetHandler::SetReticleStyle(ReticleStyle a_style)
{
	Locker locker(_lock);
	_reticleStyle = a_style;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetShowTargetBar() const
{
	Locker locker(_lock);
	return _bShowTargetBar;
}

void WidgetHandler::SetShowTargetBar(bool a_show)
{
	Locker locker(_lock);
	_bShowTargetBar = a_show;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetShowSoftTargetBar() const
{
	Locker locker(_lock);
	return _bShowSoftTargetBar;
}

void WidgetHandler::SetShowSoftTargetBar(bool a_show)
{
	Locker locker(_lock);
	_bShowSoftTargetBar = a_show;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetShowTargetName() const
{
	Locker locker(_lock);
	return _bShowTargetName;
}

void WidgetHandler::SetShowTargetName(bool a_show)
{
	Locker locker(_lock);
	_bShowTargetName = a_show;

	RefreshTargetLockMenu();
}

WidgetHandler::TargetLockLevelMode WidgetHandler::GetTargetLevelMode() const
{
	Locker locker(_lock);
	return _targetLevelMode;
}

void WidgetHandler::SetTargetLevelMode(TargetLockLevelMode a_mode)
{
	Locker locker(_lock);
	_targetLevelMode = a_mode;

	RefreshTargetLockMenu();
}

uint16_t WidgetHandler::GetTargetLevelThreshold() const
{
	Locker locker(_lock);
	return _targetLevelThreshold;
}

void WidgetHandler::SetTargetLevelThreshold(uint16_t a_threshold)
{
	Locker locker(_lock);
	_targetLevelThreshold = a_threshold;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetShowDamage() const
{
	Locker locker(_lock);
	return _bShowDamage;
}

void WidgetHandler::SetShowDamage(bool a_show)
{
	Locker locker(_lock);
	_bShowDamage = a_show;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetShowHealthPhantom() const
{
	Locker locker(_lock);
	return _bShowHealthPhantom;
}

void WidgetHandler::SetShowHealthPhantom(bool a_show)
{
	Locker locker(_lock);
	_bShowHealthPhantom = a_show;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetHideVanillaTargetBar() const
{
	Locker locker(_lock);
	return _bHideVanillaTargetBar;
}

void WidgetHandler::SetHideVanillaTargetBar(bool a_hide)
{
	Locker locker(_lock);
	_bHideVanillaTargetBar = a_hide;

	RefreshTargetLockMenu();
}

float WidgetHandler::GetHealthPhantomDuration() const
{
	Locker locker(_lock);
	return _healthPhantomDuration;
}

void WidgetHandler::SetHealthPhantomDuration(float a_duration)
{
	Locker locker(_lock);
	_healthPhantomDuration = a_duration;

	RefreshTargetLockMenu();
}

float WidgetHandler::GetDamageDuration() const
{
	Locker locker(_lock);
	return _damageDuration;
}

void WidgetHandler::SetDamageDuration(float a_duration)
{
	Locker locker(_lock);
	_damageDuration = a_duration;

	RefreshTargetLockMenu();
}

float WidgetHandler::GetReticleScale() const
{
	Locker locker(_lock);
	return _reticleScale;
}

void WidgetHandler::SetReticleScale(float a_scale)
{
	Locker locker(_lock);
	_reticleScale = a_scale;

	RefreshTargetLockMenu();
}

float WidgetHandler::GetTargetBarScale() const
{
	Locker locker(_lock);
	return _targetBarScale;
}

void WidgetHandler::SetTargetBarScale(float a_scale)
{
	Locker locker(_lock);
	_targetBarScale = a_scale;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetUseHUDOpacityForReticle() const
{
	Locker locker(_lock);
	return _bUseHUDOpacityForReticle;
}

void WidgetHandler::SetUseHUDOpacityForReticle(bool a_enable)
{
	Locker locker(_lock);
	_bUseHUDOpacityForReticle = a_enable;

	RefreshTargetLockMenu();
}

float WidgetHandler::GetReticleOpacity() const
{
	Locker locker(_lock);
	return _reticleOpacity;
}

void WidgetHandler::SetReticleOpacity(float a_opacity)
{
	Locker locker(_lock);
	_reticleOpacity = a_opacity;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetUseHUDOpacityForTargetBar() const
{
	Locker locker(_lock);
	return _bUseHUDOpacityForTargetBar;
}

void WidgetHandler::SetUseHUDOpacityForTargetBar(bool a_enable)
{
	Locker locker(_lock);
	_bUseHUDOpacityForTargetBar = a_enable;

	RefreshTargetLockMenu();
}

float WidgetHandler::GetTargetBarOpacity() const
{
	Locker locker(_lock);
	return _targetBarOpacity;
}

void WidgetHandler::SetTargetBarOpacity(float a_opacity)
{
	Locker locker(_lock);
	_targetBarOpacity = a_opacity;

	RefreshTargetLockMenu();
}

bool WidgetHandler::GetShowBossBar() const
{
	Locker locker(_lock);
	return _bShowBossBar;
}

void WidgetHandler::SetShowBossBar(bool a_show)
{
	Locker locker(_lock);
	_bShowBossBar = a_show;

	RefreshBossMenu();
}

WidgetHandler::TextAlignment WidgetHandler::GetBossNameAlignment() const
{
	Locker locker(_lock);
	return _bossNameAlignment;
}

void WidgetHandler::SetBossNameAlignment(TextAlignment a_alignment)
{
	Locker locker(_lock);
	_bossNameAlignment = a_alignment;

	RefreshBossMenu();
}

WidgetHandler::BossLevelMode WidgetHandler::GetBossLevelMode() const
{
	Locker locker(_lock);
	return _bossLevelMode;
}

void WidgetHandler::SetBossLevelMode(BossLevelMode a_mode)
{
	Locker locker(_lock);
	_bossLevelMode = a_mode;

	RefreshBossMenu();
}

uint16_t WidgetHandler::GetBossLevelThreshold() const
{
	Locker locker(_lock);
	return _bossLevelThreshold;
}

void WidgetHandler::SetBossLevelThreshold(uint16_t a_threshold)
{
	Locker locker(_lock);
	_bossLevelThreshold = a_threshold;

	RefreshBossMenu();
}

bool WidgetHandler::GetShowBossDamage() const
{
	Locker locker(_lock);
	return _bShowBossDamage;
}

void WidgetHandler::SetShowBossDamage(bool a_show)
{
	Locker locker(_lock);
	_bShowBossDamage = a_show;

	RefreshBossMenu();
}

bool WidgetHandler::GetShowBossHealthPhantom() const
{
	Locker locker(_lock);
	return _bShowBossHealthPhantom;
}

void WidgetHandler::SetShowBossHealthPhantom(bool a_show)
{
	Locker locker(_lock);
	_bShowBossHealthPhantom = a_show;

	RefreshBossMenu();
}

float WidgetHandler::GetBossHealthPhantomDuration() const
{
	Locker locker(_lock);
	return _bossHealthPhantomDuration;
}

void WidgetHandler::SetBossHealthPhantomDuration(float a_duration)
{
	Locker locker(_lock);
	_bossHealthPhantomDuration = a_duration;

	RefreshBossMenu();
}

float WidgetHandler::GetBossDamageDuration() const
{
	Locker locker(_lock);
	return _bossDamageDuration;
}

void WidgetHandler::SetBossDamageDuration(float a_duration)
{
	Locker locker(_lock);
	_bossDamageDuration = a_duration;

	RefreshBossMenu();
}

float WidgetHandler::GetBossBarScale() const
{
	Locker locker(_lock);
	return _bossBarScale;
}

void WidgetHandler::SetBossBarScale(float a_scale)
{
	Locker locker(_lock);
	_bossBarScale = a_scale;

	RefreshBossMenu();
}

bool WidgetHandler::GetUseHUDOpacityForBossBar() const
{
	Locker locker(_lock);
	return _bUseHUDOpacityForBossBar;
}

void WidgetHandler::SetUseHUDOpacityForBossBar(bool a_enable)
{
	Locker locker(_lock);
	_bUseHUDOpacityForBossBar = a_enable;

	RefreshBossMenu();
}

float WidgetHandler::GetBossBarOpacity() const
{
	Locker locker(_lock);
	return _bossBarOpacity;
}

void WidgetHandler::SetBossBarOpacity(float a_opacity)
{
	Locker locker(_lock);
	_bossBarOpacity = a_opacity;

	RefreshBossMenu();
}

float WidgetHandler::GetBossBarX() const
{
	Locker locker(_lock);
	return _bossBarX;
}

void WidgetHandler::SetBossBarX(float a_position)
{
	Locker locker(_lock);
	_bossBarX = a_position;

	RefreshBossMenu();
}

float WidgetHandler::GetBossBarY() const
{
	Locker locker(_lock);
	return _bossBarY;
}

void WidgetHandler::SetBossBarY(float a_position)
{
	Locker locker(_lock);
	_bossBarY = a_position;

	RefreshBossMenu();
}

bool WidgetHandler::GetBossBarOffsetSubtitles() const
{
	Locker locker(_lock);
	return _bossBarOffsetSubtitles;
}

void WidgetHandler::SetBossBarOffsetSubtitles(bool a_enable)
{
	Locker locker(_lock);
	_bossBarOffsetSubtitles = a_enable;

	RefreshBossMenu();
}

float WidgetHandler::GetMultipleBossBarsOffset() const
{
	Locker locker(_lock);
	return _multipleBossBarsOffset;
}

void WidgetHandler::SetMultipleBossBarsOffset(float a_offset)
{
	Locker locker(_lock);
	_multipleBossBarsOffset = a_offset;

	RefreshBossMenu();
}

bool WidgetHandler::GetMultipleBossBarsStackUpwards() const
{
	Locker locker(_lock);
	return _multipleBossBarsStackUpwards;
}

void WidgetHandler::SetMultipleBossBarsStackUpwards(bool a_enable)
{
	Locker locker(_lock);
	_multipleBossBarsStackUpwards = a_enable;

	RefreshBossMenu();
}

bool WidgetHandler::ShowSoftTargetBar()
{
	return GetSingleton()->_bShowSoftTargetBar;
}

bool WidgetHandler::Save(const SKSE::SerializationInterface* a_intfc, std::uint32_t a_typeCode, std::uint32_t a_version)
{
	Locker locker(_lock);

	if (!a_intfc->OpenRecord(a_typeCode, a_version)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowReticle)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_reticleStyle)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowTargetBar)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowSoftTargetBar)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowTargetName)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLevelMode)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetLevelThreshold)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowDamage)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowHealthPhantom)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bHideVanillaTargetBar)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_healthPhantomDuration)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_damageDuration)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_reticleScale)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetBarScale)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bUseHUDOpacityForReticle)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_reticleOpacity)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bUseHUDOpacityForTargetBar)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_targetBarOpacity)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowBossBar)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossNameAlignment)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossLevelMode)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossLevelThreshold)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowBossDamage)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bShowBossHealthPhantom)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossHealthPhantomDuration)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossDamageDuration)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossBarScale)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bUseHUDOpacityForBossBar)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossBarOpacity)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossBarX)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossBarY)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_bossBarOffsetSubtitles)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_multipleBossBarsOffset)) {
		return false;
	}

	if (!a_intfc->WriteRecordData(_multipleBossBarsStackUpwards)) {
		return false;
	}

	return true;
}

bool WidgetHandler::Load(const SKSE::SerializationInterface* a_intfc)
{
	Locker locker(_lock);

	if (!a_intfc->ReadRecordData(_bShowReticle)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_reticleStyle)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowTargetBar)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowSoftTargetBar)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowTargetName)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLevelMode)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetLevelThreshold)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowDamage)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowHealthPhantom)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bHideVanillaTargetBar)) {
		return false;
	}
		
	if (!a_intfc->ReadRecordData(_healthPhantomDuration)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_damageDuration)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_reticleScale)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetBarScale)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bUseHUDOpacityForReticle)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_reticleOpacity)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bUseHUDOpacityForTargetBar)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_targetBarOpacity)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowBossBar)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossNameAlignment)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossLevelMode)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossLevelThreshold)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowBossDamage)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bShowBossHealthPhantom)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossHealthPhantomDuration)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossDamageDuration)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossBarScale)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bUseHUDOpacityForBossBar)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossBarOpacity)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossBarX)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossBarY)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_bossBarOffsetSubtitles)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_multipleBossBarsOffset)) {
		return false;
	}

	if (!a_intfc->ReadRecordData(_multipleBossBarsStackUpwards)) {
		return false;
	}

	return true;
}

void WidgetHandler::Clear()
{
	Locker locker(_lock);
	_bShowReticle = DF_SHOWRETICLE;
	_reticleStyle = DF_RETICLESTYLE;
	_bShowTargetBar = DF_SHOWTARGETBAR;
	_bShowSoftTargetBar = DF_SHOWSOFTTARGETBAR;
	_bShowTargetName = DF_SHOWTARGETNAME;
	_targetLevelMode = DF_TARGETLEVELMODE;
	_targetLevelThreshold = DF_TARGETLEVELTHRESHOLD;
	_bShowDamage = DF_SHOWDAMAGE;
	_bShowHealthPhantom = DF_SHOWHEALTHPHANTOM;
	_bHideVanillaTargetBar = DF_HIDEVANILLATARGETBAR;
	_healthPhantomDuration = DF_HEALTHPHANTOMDURATION;
	_damageDuration = DF_DAMAGEDURATION;
	_reticleScale = DF_RETICLESCALE;
	_targetBarScale = DF_TARGETBARSCALE;
	_bUseHUDOpacityForReticle = DF_USEHUDOPACITYFORRETICLE;
	_reticleOpacity = DF_RETICLEOPACITY;
	_bUseHUDOpacityForTargetBar = DF_USEHUDOPACITYFORTARGETBAR;
	_targetBarOpacity = DF_TARGETBAROPACITY;
	_bShowBossBar = DF_SHOWBOSSBAR;
	_bossNameAlignment = DF_BOSSNAMEALIGNMENT;
	_bossLevelMode = DF_BOSSLEVELMODE;
	_bossLevelThreshold = DF_BOSSLEVELTHRESHOLD;
	_bShowBossDamage = DF_SHOWBOSSDAMAGE;
	_bShowBossHealthPhantom = DF_SHOWBOSSHEALTHPHANTOM;
	_bossHealthPhantomDuration = DF_BOSSHEALTHPHANTOMDURATION;
	_bossDamageDuration = DF_BOSSDAMAGEDURATION;
	_bossBarScale = DF_BOSSBARSCALE;
	_bUseHUDOpacityForBossBar = DF_USEHUDOPACITYFORBOSSBAR;
	_bossBarOpacity = DF_BOSSBAROPACITY;
	_bossBarX = DF_BOSSBARX;
	_bossBarY = DF_BOSSBARY;
	_bossBarOffsetSubtitles = DF_BOSSBAROFFSETSUBTITLES;
	_multipleBossBarsOffset = DF_MULTIPLEBOSSBARSOFFSET;
	_multipleBossBarsStackUpwards = DF_MULTIPLEBOSSBARSSTACKUPWARDS;
}

void WidgetHandler::OnLoad()
{
	CloseTargetLockMenu();
	CloseBossMenu();
}

WidgetHandler::WidgetHandler() :
	_lock(),
	_bShowReticle(DF_SHOWRETICLE),
	_reticleStyle(DF_RETICLESTYLE),
	_bShowTargetBar(DF_SHOWTARGETBAR),
	_bShowSoftTargetBar(DF_SHOWSOFTTARGETBAR),
	_bShowTargetName(DF_SHOWTARGETNAME),
	_targetLevelMode(DF_TARGETLEVELMODE),
	_targetLevelThreshold(DF_TARGETLEVELTHRESHOLD),
	_bShowDamage(DF_SHOWDAMAGE),
	_bShowHealthPhantom(DF_SHOWHEALTHPHANTOM),
	_bHideVanillaTargetBar(DF_HIDEVANILLATARGETBAR),
	_healthPhantomDuration(DF_HEALTHPHANTOMDURATION),
	_damageDuration(DF_DAMAGEDURATION),
	_reticleScale(DF_RETICLESCALE),
	_targetBarScale(DF_TARGETBARSCALE),
	_bUseHUDOpacityForReticle(DF_USEHUDOPACITYFORRETICLE),
	_reticleOpacity(DF_RETICLEOPACITY),
	_bUseHUDOpacityForTargetBar(DF_USEHUDOPACITYFORTARGETBAR),
	_targetBarOpacity(DF_TARGETBAROPACITY),
	_bShowBossBar(DF_SHOWBOSSBAR),
	_bossNameAlignment(DF_BOSSNAMEALIGNMENT),
	_bossLevelMode(DF_BOSSLEVELMODE),
	_bossLevelThreshold(DF_BOSSLEVELTHRESHOLD),
	_bShowBossDamage(DF_SHOWBOSSDAMAGE),
	_bShowBossHealthPhantom(DF_SHOWBOSSHEALTHPHANTOM),
	_bossHealthPhantomDuration(DF_BOSSHEALTHPHANTOMDURATION),
	_bossDamageDuration(DF_BOSSDAMAGEDURATION),
	_bossBarScale(DF_BOSSBARSCALE),
	_bUseHUDOpacityForBossBar(DF_USEHUDOPACITYFORBOSSBAR),
	_bossBarOpacity(DF_BOSSBAROPACITY),
	_bossBarX(DF_BOSSBARX),
	_bossBarY(DF_BOSSBARY),
	_bossBarOffsetSubtitles(DF_BOSSBAROFFSETSUBTITLES),
	_multipleBossBarsOffset(DF_MULTIPLEBOSSBARSOFFSET),
	_multipleBossBarsStackUpwards(DF_MULTIPLEBOSSBARSSTACKUPWARDS)
{}
