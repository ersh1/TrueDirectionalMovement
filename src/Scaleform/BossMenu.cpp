#include "Scaleform/BossMenu.h"

#include "Offsets.h"
#include "Utils.h"
#include "WidgetHandler.h"
#include "DirectionalMovementHandler.h"

namespace Scaleform
{
	void BossMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		if (ui) {
			ui->Register(MENU_NAME, Creator);
			logger::info("Registered {}"sv, MENU_NAME);
		}
	}

	void BossMenu::AddBoss(RE::ActorHandle a_boss)
	{
		_bossQueue.emplace_back(a_boss);
	}

	void BossMenu::RemoveBoss(RE::ActorHandle a_boss, bool a_bBossDied)
	{
		_bossQueue.remove(a_boss);
		auto bossBar = std::find(_bossBars.begin(), _bossBars.end(), a_boss);
		if (bossBar != _bossBars.end()) 
		{
			if (a_bBossDied)
			{
				KillBoss(*bossBar);
			} else {
				bossBar->Clear();
			}
		}
	}

	void BossMenu::Update()
	{
		bool bAtLeastOneBoss = false;
		auto widgetHandler = WidgetHandler::GetSingleton();

		for (auto& bossBar : _bossBars)
		{
			if (!bossBar.boss) {
				const RE::GFxValue bFalse{ false };
				// todo 
				continue;
			} else {
				const RE::GFxValue bTrue{ true };
			}

			bAtLeastOneBoss = true;

			if (bossBar.updateTimer > 0.f) {
				bossBar.updateTimer -= *g_deltaTime;
			}

			if (bossBar.hideTimer > 0.f) {
				bossBar.hideTimer -= *g_deltaTime;
			}

			float scale = 100.f * widgetHandler->_bossBarScale;

			// find correct position
			for (int i = 0; i < _occupiedBarSpots.size(); ++i) {
				if (!_occupiedBarSpots[i]) {
					if (bossBar.spotNumber == -1)
					{
						bossBar.spotNumber = i;
						_occupiedBarSpots[bossBar.spotNumber] = true;
					} else if (bossBar.spotNumber > i)
					{
						_occupiedBarSpots[bossBar.spotNumber] = false;
						bossBar.spotNumber = i;
						_occupiedBarSpots[bossBar.spotNumber] = true;
					}
					break;
				}
			}

			RE::NiPoint2 position;
			float multipleBarsOffsetY = bossBar.spotNumber * widgetHandler->_multipleBossBarsOffset;

			auto def = uiMovie->GetMovieDef();
			if (def)
			{
				position.x = def->GetWidth();
				position.y = def->GetHeight();
				multipleBarsOffsetY /= def->GetHeight();
			}

			if (widgetHandler->_multipleBossBarsStackUpwards) {
				multipleBarsOffsetY *= -1;
			}
			position.x *= widgetHandler->_bossBarX;
			position.y *= widgetHandler->_bossBarY + multipleBarsOffsetY;
			
			RE::GFxValue::DisplayInfo bossBarDisplayInfo;
			bossBarDisplayInfo.SetPosition(position.x, position.y);
			bossBarDisplayInfo.SetScale(scale, scale);
			bossBar.bar.SetDisplayInfo(bossBarDisplayInfo);

			if (widgetHandler->_bossBarOffsetSubtitles)
			{
				auto hud = RE::UI::GetSingleton()->GetMenu("HUD Menu");
				hud.get()->uiMovie->SetVariable("HUDMovieBaseInstance.SubtitleTextHolder._y", _savedSubtitleY.GetNumber() - GetSubtitleOffset());
			}

			if (bossBar.bossState == BossState::kUninitialized) {
				// fill boss name
				RE::GFxValue textField;
				bossBar.bar.GetMember("TargetName", &textField);
				if (textField.IsDisplayObject()) {
					const char* targetName = bossBar.boss.get()->GetDisplayFullName();
					textField.SetText(targetName);
				}

				// target level
				if (widgetHandler->_bossLevelMode != WidgetHandler::BossLevelMode::kDisable) {
					uint16_t playerLevel = RE::PlayerCharacter::GetSingleton()->GetLevel();
					uint16_t targetLevel = bossBar.boss.get()->GetLevel();
					uint32_t color;

					if (playerLevel - targetLevel > widgetHandler->_bossLevelThreshold) {
						color = _weakerColor;
					} else if (targetLevel - playerLevel > widgetHandler->_bossLevelThreshold) {
						color = _strongerColor;
					} else {
						color = _equalColor;
					}

					switch (widgetHandler->_bossLevelMode) {
					case WidgetHandler::BossLevelMode::kIcon:
						{
							RE::GFxValue levelIcon;
							bossBar.bar.GetMember("LevelIcon", &levelIcon);
							if (levelIcon.IsDisplayObject()) {
								RE::GFxValue args[1];
								args[0].SetNumber(color);
								bossBar.bar.Invoke("setLevelIconColor", nullptr, args, 1);
							}
							break;
						}
					case WidgetHandler::BossLevelMode::kText:
						{
							RE::GFxValue levelTextField;
							bossBar.bar.GetMember("LevelText", &levelTextField);
							if (levelTextField.IsDisplayObject()) {
								std::string targetLevelString = std::to_string(targetLevel);
								levelTextField.SetText(targetLevelString.c_str());

								RE::GFxValue args[1];
								args[0].SetNumber(color);
								bossBar.bar.Invoke("setLevelTextColor", nullptr, args, 1);
							}
							break;
						}
					}
				}

				bossBar.damage = 0.f;
				bossBar.healthPercent = -1;

				bossBar.bossState = kInitialized;

				bossBar.bar.Invoke("showBoss");
			}

			UpdateBossHealth(bossBar);

			if (bossBar.updateTimer <= 0.f) {
				bossBar.bar.Invoke("hideDamage");
				bossBar.damage = 0;
			}		

			if (bossBar.bossState == BossState::kDying && bossBar.hideTimer <= 0.f) {
				if (bossBar.spotNumber > -1 && bossBar.spotNumber < _occupiedBarSpots.size())
				{
					_occupiedBarSpots[bossBar.spotNumber] = false;
				}
				bossBar.Clear();
			} 
		}

		if (!bAtLeastOneBoss) {
			widgetHandler->CloseBossMenu();
		} else {
			if (widgetHandler->_bBossHideVanillaTargetBar) {
				bool bFound = false;
				auto enemyHealthTargetRef = widgetHandler->GetEnemyHealthTargetRef();
				for (auto& bossBar : _bossBars)
				{
					if (bossBar.boss.native_handle() == enemyHealthTargetRef)
					{
						bFound = true;
						break;
					}
				}

				if (bFound) 
				{
					if (!_bVanillaTargetBarHidden)
					{
						widgetHandler->HideVanillaTargetBar();
						_bVanillaTargetBarHidden = true;
					}
				} 
				else 
				{
					if (_bVanillaTargetBarHidden) 
					{
						widgetHandler->ShowVanillaTargetBar();
						_bVanillaTargetBarHidden = false;
					}
				}
			}
		}
	}

	void BossMenu::RefreshUI()
	{
		auto widgetHandler = WidgetHandler::GetSingleton();

		const RE::GFxValue bTrue{ true };
		const RE::GFxValue bFalse{ false };

		if (uiMovie) {
			for (auto& bossBar : _bossBars) {
				switch (widgetHandler->_bossLevelMode) {
				case WidgetHandler::BossLevelMode::kDisable:
					bossBar.bar.Invoke("levelDisplayNone");
					break;
				case WidgetHandler::BossLevelMode::kIcon:
					bossBar.bar.Invoke("levelDisplayIcon");
					break;
				case WidgetHandler::BossLevelMode::kText:
					bossBar.bar.Invoke("levelDisplayText");
					break;
				}

				if (widgetHandler->_bShowBossHealthPhantom) {
					RE::GFxValue arg[1];
					arg[0].SetBoolean(true);
					bossBar.bar.Invoke("phantomBarVisibility", nullptr, arg, 1);

					RE::GFxValue durArg[1];
					durArg[0].SetNumber(widgetHandler->_bossHealthPhantomDuration);
					bossBar.bar.Invoke("setPhantomDuration", nullptr, durArg, 1);
				} else {
					RE::GFxValue arg[1];
					arg[0].SetBoolean(false);
					bossBar.bar.Invoke("phantomBarVisibility", nullptr, arg, 1);
				}

				if (!widgetHandler->_bShowDamage) {
					bossBar.bar.Invoke("hideDamage");
				}

				switch (widgetHandler->_bossNameAlignment) {
				case WidgetHandler::TextAlignment::kCenter:
					{
						RE::GFxValue arg[1];
						arg[0].SetString("center");
						bossBar.bar.Invoke("bossNameAlignment", nullptr, arg, 1);
						break;
					}
				case WidgetHandler::TextAlignment::kLeft:
					{
						RE::GFxValue arg[1];
						arg[0].SetString("left");
						bossBar.bar.Invoke("bossNameAlignment", nullptr, arg, 1);
						break;
					}
				case WidgetHandler::TextAlignment::kRight:
					{
						RE::GFxValue arg[1];
						arg[0].SetString("right");
						bossBar.bar.Invoke("bossNameAlignment", nullptr, arg, 1);
						break;
					}
				}

				{
					float barAlpha = widgetHandler->_bUseHUDOpacityForBossBar ? *g_fHUDOpacity : widgetHandler->_bossBarOpacity;
					barAlpha *= 100.f;
					RE::GFxValue arg[1];
					arg[0].SetNumber(barAlpha);
					bossBar.bar.Invoke("setBarAlpha", nullptr, arg, 1);
				}

				if (bossBar.bossState > BossState::kNone)
				{
					bossBar.bar.Invoke("showBoss");
				}
			}

			//_bossList.Invoke("playTimeline");

			if (!_bSubtitleYSaved && widgetHandler->_bossBarOffsetSubtitles) {
				auto hud = RE::UI::GetSingleton()->GetMenu("HUD Menu");
				hud.get()->uiMovie->GetVariable(&_savedSubtitleY, "HUDMovieBaseInstance.SubtitleTextHolder._y");
				_bSubtitleYSaved = true;
			}
		}
	}

	bool BossMenu::IsDisplayingBoss(RE::ActorHandle a_boss) const
	{
		for (auto& bossBar : _bossBars)
		{
			if (bossBar.boss == a_boss)
			{
				return true;
			}
		}

		return false;
	}

	void BossMenu::UpdateBossHealth(BossBar& a_bossBar, bool bForceDead /*= false*/)
	{
		if (a_bossBar.bossState == BossState::kDying && !bForceDead)
		{
			return;
		}

		auto widgetHandler = WidgetHandler::GetSingleton();

		float currentHealth = 0.f;
		float maxHealth = 0.f;
		float currentHealthPercent = 0.f;

		if (!bForceDead)
		{
			currentHealth = fmax(a_bossBar.boss.get()->GetActorValue(RE::ActorValue::kHealth), 0.f);
			maxHealth = fmax(a_bossBar.boss.get()->GetPermanentActorValue(RE::ActorValue::kHealth), 0.f);
			currentHealthPercent;
			if (currentHealth > 0.f && maxHealth > 0.f) {
				currentHealthPercent = currentHealth / maxHealth;
			} else {
				currentHealthPercent = 0.f;
			}
		}
		
		if (currentHealth <= 0.f && a_bossBar.bossState < BossState::kDying)  // boss dead!
		{
			KillBoss(a_bossBar);
		}

		if (a_bossBar.healthPercent == -1)  // new target
		{
			RE::GFxValue args[1];
			args[0].SetNumber(currentHealthPercent);
			a_bossBar.bar.Invoke("setHealthPercent", nullptr, args, 1);
			a_bossBar.healthPercent = currentHealthPercent;

			a_bossBar.updateTimer = widgetHandler->_bossDamageDuration;

			a_bossBar.prevHealth = currentHealth;
		}

		if (!ApproximatelyEqual(a_bossBar.healthPercent, currentHealthPercent)) {
			bool bHealing = (a_bossBar.healthPercent < currentHealthPercent);

			RE::GFxValue args[2];
			args[0].SetNumber(a_bossBar.healthPercent);
			args[1].SetNumber(currentHealthPercent);
			a_bossBar.bar.Invoke("updateHealthPercent", nullptr, args, 2);
			a_bossBar.healthPercent = currentHealthPercent;

			if (!bHealing) {
				a_bossBar.updateTimer = widgetHandler->_bossDamageDuration;

				if (widgetHandler->_bShowBossDamage) {
					a_bossBar.damage += (a_bossBar.prevHealth - currentHealth);
					a_bossBar.prevHealth = currentHealth;

					RE::GFxValue arg[1];
					arg[0].SetNumber(static_cast<int32_t>(a_bossBar.damage));
					a_bossBar.bar.Invoke("updateDamage", nullptr, arg, 1);
				}
			}
		}
	}

	void BossMenu::KillBoss(BossBar& a_bossBar)
	{
		if (a_bossBar.bossState < BossState::kDying)
		{
			a_bossBar.bar.Invoke("removeBoss");
			a_bossBar.bossState = BossState::kDying;
			a_bossBar.hideTimer = 4.f;

			// Update health in case we weren't called by UpdateBossHealth
			UpdateBossHealth(a_bossBar, true);
		}
	}

	void BossMenu::AssignBossTargetsFromQueue()
	{
		for (auto& bossBar : _bossBars)
		{
			if (_bossQueue.empty()) {
				return;
			}

			if (!bossBar.boss) {
				bossBar.boss = *_bossQueue.begin();
				bossBar.bossState = BossState::kUninitialized;
				_bossQueue.pop_front();
			}
		}
	}

	void BossMenu::OnOpen()
	{
		RefreshUI();

		ProcessDelegate();
	}

	void BossMenu::OnClose()
	{
		/*if (uiMovie) {
		}*/

		auto widgetHandler = WidgetHandler::GetSingleton();

		if (widgetHandler->_bossBarOffsetSubtitles)
		{
			auto hud = RE::UI::GetSingleton()->GetMenu("HUD Menu");
			hud.get()->uiMovie->SetVariable("HUDMovieBaseInstance.SubtitleTextHolder._y", _savedSubtitleY);
		}

		if (widgetHandler->_bBossHideVanillaTargetBar) {
			if (_bVanillaTargetBarHidden) {
				widgetHandler->ShowVanillaTargetBar();
				_bVanillaTargetBarHidden = false;
			}
		}
		
		_occupiedBarSpots.clear();
		_bossQueue.clear();
	}

	void BossMenu::ProcessDelegate()
	{
		WidgetHandler::GetSingleton()->ProcessBossMenu(*this);
	}

	float BossMenu::GetSubtitleOffset() const
	{
		for (int i = (int)_occupiedBarSpots.size() - 1; i >= 0; --i)
		{
			if (_occupiedBarSpots[i])
			{
				return (i + 1) * WidgetHandler::GetSingleton()->_multipleBossBarsOffset;
			}
		}

		return 0;
	}

}
