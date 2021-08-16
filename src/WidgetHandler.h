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
	enum class ReticleStyle : std::uint32_t
	{
		kDefault = 0,
		kSimpleDot = 1,
		kSimpleGlow = 2
	};

	enum class TargetLockLevelMode : std::uint32_t
	{
		kDisable = 0,
		kIcon = 1,
		kText = 2,
		kOutline = 3
	};

	enum class BossLevelMode : std::uint32_t
	{
		kDisable = 0,
		kIcon = 1,
		kText = 2
	};

	enum class TextAlignment : std::uint32_t
	{
		kCenter = 0,
		kLeft = 1,
		kRight = 2
	};

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

	bool GetShowReticle() const;
	void SetShowReticle(bool a_show);
	ReticleStyle GetReticleStyle() const;
	void SetReticleStyle(ReticleStyle a_style);
	bool GetShowTargetBar() const;
	void SetShowTargetBar(bool a_show);
	bool GetShowSoftTargetBar() const;
	void SetShowSoftTargetBar(bool a_show);
	bool GetShowTargetName() const;
	void SetShowTargetName(bool a_show);
	TargetLockLevelMode GetTargetLevelMode() const;
	void SetTargetLevelMode(TargetLockLevelMode a_mode);
	uint16_t GetTargetLevelThreshold() const;
	void SetTargetLevelThreshold(uint16_t a_threshold);
	bool GetShowDamage() const;
	void SetShowDamage(bool a_show);
	bool GetShowHealthPhantom() const;
	void SetShowHealthPhantom(bool a_show);
	bool GetHideVanillaTargetBar() const;
	void SetHideVanillaTargetBar(bool a_hide);
	float GetHealthPhantomDuration() const;
	void SetHealthPhantomDuration(float a_duration);
	float GetDamageDuration() const;
	void SetDamageDuration(float a_duration);
	float GetReticleScale() const;
	void SetReticleScale(float a_scale);
	float GetTargetBarScale() const;
	void SetTargetBarScale(float a_scale);
	bool GetUseHUDOpacityForReticle() const;
	void SetUseHUDOpacityForReticle(bool a_enable);
	float GetReticleOpacity() const;
	void SetReticleOpacity(float a_opacity);
	bool GetUseHUDOpacityForTargetBar() const;
	void SetUseHUDOpacityForTargetBar(bool a_enable);
	float GetTargetBarOpacity() const;
	void SetTargetBarOpacity(float a_opacity);

	bool GetShowBossBar() const;
	void SetShowBossBar(bool a_show);
	TextAlignment GetBossNameAlignment() const;
	void SetBossNameAlignment(TextAlignment a_alignment);
	BossLevelMode GetBossLevelMode() const;
	void SetBossLevelMode(BossLevelMode a_mode);
	uint16_t GetBossLevelThreshold() const;
	void SetBossLevelThreshold(uint16_t a_threshold);
	bool GetShowBossDamage() const;
	void SetShowBossDamage(bool a_show);
	bool GetShowBossHealthPhantom() const;
	void SetShowBossHealthPhantom(bool a_show);
	bool GetBossHideVanillaTargetBar() const;
	void SetBossHideVanillaTargetBar(bool a_hide);
	float GetBossHealthPhantomDuration() const;
	void SetBossHealthPhantomDuration(float a_duration);
	float GetBossDamageDuration() const;
	void SetBossDamageDuration(float a_duration);
	float GetBossBarScale() const;
	void SetBossBarScale(float a_scale);
	bool GetUseHUDOpacityForBossBar() const;
	void SetUseHUDOpacityForBossBar(bool a_enable);
	float GetBossBarOpacity() const;
	void SetBossBarOpacity(float a_opacity);
	float GetBossBarX() const;
	void SetBossBarX(float a_position);
	float GetBossBarY() const;
	void SetBossBarY(float a_position);
	bool GetBossBarOffsetSubtitles() const;
	void SetBossBarOffsetSubtitles(bool a_enable);
	float GetMultipleBossBarsOffset() const;
	void SetMultipleBossBarsOffset(float a_offset);
	bool GetMultipleBossBarsStackUpwards() const;
	void SetMultipleBossBarsStackUpwards(bool a_enable);

	static bool ShowSoftTargetBar();

	void Update();

	bool Save(const SKSE::SerializationInterface* a_intfc, std::uint32_t a_typeCode, std::uint32_t a_version);
	bool Load(const SKSE::SerializationInterface* a_intfc);
	void Clear();
	void OnLoad();

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

		return true;
	}

	[[nodiscard]] bool ShouldBossMenuOpen() const
	{
		if (IsBossMenuOpen()) {
			return false;
		}

		return true;
	}

	mutable Lock _lock;
	std::vector<TargetLockTasklet> _targetLockMenuTaskQueue;
	std::vector<BossTasklet> _bossMenuTaskQueue;
	uint8_t _hideVanillaTargetBar = 0;
	bool _refreshTargetLockMenu{ false };
	bool _refreshBossMenu{ false };
	RE::RefHandle _enemyHealthTargetRef;

	static constexpr bool DF_SHOWRETICLE = true;
	static constexpr ReticleStyle DF_RETICLESTYLE = ReticleStyle::kDefault;
	static constexpr bool DF_SHOWTARGETBAR = true;
	static constexpr bool DF_SHOWSOFTTARGETBAR = true;
	static constexpr bool DF_SHOWTARGETNAME = true;
	static constexpr TargetLockLevelMode DF_TARGETLEVELMODE = TargetLockLevelMode::kOutline;
	static constexpr uint16_t DF_TARGETLEVELTHRESHOLD = 10;
	static constexpr bool DF_SHOWDAMAGE = true;
	static constexpr bool DF_SHOWHEALTHPHANTOM = true;
	static constexpr bool DF_HIDEVANILLATARGETBAR = true;
	static constexpr float DF_HEALTHPHANTOMDURATION = 0.75f;
	static constexpr float DF_DAMAGEDURATION = 2.f;
	static constexpr float DF_RETICLESCALE = 1.f;
	static constexpr float DF_TARGETBARSCALE = 1.f;
	static constexpr bool DF_USEHUDOPACITYFORRETICLE = true;
	static constexpr float DF_RETICLEOPACITY = 1.f;
	static constexpr bool DF_USEHUDOPACITYFORTARGETBAR = true;
	static constexpr float DF_TARGETBAROPACITY = 1.f;

	static constexpr bool DF_SHOWBOSSBAR = true;
	static constexpr TextAlignment DF_BOSSNAMEALIGNMENT = TextAlignment::kCenter;
	static constexpr BossLevelMode DF_BOSSLEVELMODE = BossLevelMode::kText;
	static constexpr uint16_t DF_BOSSLEVELTHRESHOLD = 10;
	static constexpr bool DF_SHOWBOSSDAMAGE = true;
	static constexpr bool DF_SHOWBOSSHEALTHPHANTOM = true;
	static constexpr bool DF_BOSSHIDEVANILLATARGETBAR = true;
	static constexpr float DF_BOSSHEALTHPHANTOMDURATION = 0.75f;
	static constexpr float DF_BOSSDAMAGEDURATION = 2.f;
	static constexpr float DF_BOSSBARSCALE = 1.f;
	static constexpr float DF_BOSSBARX = 0.5f;
	static constexpr float DF_BOSSBARY = 0.87f;
	static constexpr bool DF_BOSSBAROFFSETSUBTITLES = true;
	static constexpr float DF_MULTIPLEBOSSBARSOFFSET = 45.f;
	static constexpr bool DF_MULTIPLEBOSSBARSSTACKUPWARDS = true;
	static constexpr bool DF_USEHUDOPACITYFORBOSSBAR = true;
	static constexpr float DF_BOSSBAROPACITY = 1.f;

	bool _bShowReticle = true;
	ReticleStyle _reticleStyle = ReticleStyle::kDefault;
	bool _bShowTargetBar = true;
	bool _bShowSoftTargetBar = true;
	bool _bShowTargetName = true;
	TargetLockLevelMode _targetLevelMode = TargetLockLevelMode::kOutline;
	uint16_t _targetLevelThreshold = 10;
	bool _bShowDamage = true;
	bool _bShowHealthPhantom = true;
	bool _bHideVanillaTargetBar = true;
	float _healthPhantomDuration = 0.75f;
	float _damageDuration = 2.f;
	float _reticleScale = 1.f;
	float _targetBarScale = 1.f;
	bool _bUseHUDOpacityForReticle = true;
	float _reticleOpacity = 1.f;
	bool _bUseHUDOpacityForTargetBar = true;
	float _targetBarOpacity = 1.f;

	bool _bShowBossBar = true;
	TextAlignment _bossNameAlignment = TextAlignment::kCenter;
	BossLevelMode _bossLevelMode = BossLevelMode::kText;
	uint16_t _bossLevelThreshold = 10;
	bool _bShowBossDamage = true;
	bool _bShowBossHealthPhantom = true;
	bool _bBossHideVanillaTargetBar = true;
	float _bossHealthPhantomDuration = 0.75f;
	float _bossDamageDuration = 2.f;
	float _bossBarScale = 1.f;
	bool _bUseHUDOpacityForBossBar = true;
	float _bossBarOpacity = 1.f;
	float _bossBarX = 0.5f;
	float _bossBarY = 0.87f;
	bool _bossBarOffsetSubtitles = true;
	float _multipleBossBarsOffset = 45.f;
	bool _multipleBossBarsStackUpwards = true;
};
