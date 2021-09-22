#include "Scaleform/TargetLockMenu.h"
#include "Scaleform/BossMenu.h"

#include "Settings.h"
#include "Offsets.h"
#include "Utils.h"
#include "WidgetHandler.h"
#include "DirectionalMovementHandler.h"

namespace Scaleform
{
	void TargetLockMenu::Register()
	{
		auto ui = RE::UI::GetSingleton();
		if (ui) {
			ui->Register(MENU_NAME, Creator);
			logger::info("Registered {}"sv, MENU_NAME);
		}
	}

	void TargetLockMenu::SetTarget(RE::ActorHandle a_target)
	{
		_target = a_target;
		if (a_target)
		{
			_reticle.Invoke("playTimeline");
		}
	}

	void TargetLockMenu::SetSoftTarget(RE::ActorHandle a_softTarget)
	{
		_softTarget = a_softTarget;
	}

	void TargetLockMenu::Update()
	{
		if (_barUpdateTimer > 0.f)
		{
			_barUpdateTimer -= *g_deltaTime;
		}

		RE::NiPoint3 reticlePos;

		auto newTarget = _target ? _target : _softTarget ? _softTarget : RE::ActorHandle();

		if (_currentTarget != newTarget) 
		{
			NewTarget(newTarget);
		}

		if (_currentTarget && GetTargetPos(_currentTarget, reticlePos, Settings::uReticleAnchor == WidgetAnchor::kBody))
		{
			const RE::GFxValue bTrue{ true };
			const RE::GFxValue bFalse{ false };

			if (_target && Settings::bShowReticle) {
				uiMovie->SetVariable("_root.ReticleHolder._visible", bTrue);
			} else {
				uiMovie->SetVariable("_root.ReticleHolder._visible", bFalse);
			}

			RE::GRectF rect = _view->GetVisibleFrameRect();

			float reticlePosX;
			float reticlePosY;
			float reticlePosZ;
			
			RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, reticlePos, reticlePosX, reticlePosY, reticlePosZ, 1e-5f);

			reticlePosY = 1.0f - reticlePosY;  // Flip y for Flash coordinate system
			reticlePosY = rect.top + (rect.bottom - rect.top) * reticlePosY;
			reticlePosX = rect.left + (rect.right - rect.left) * reticlePosX;

			auto widgetHandler = WidgetHandler::GetSingleton();
			float reticleScale = 100.f * Settings::fReticleScale;

			RE::GFxValue::DisplayInfo reticleHolderDisplayInfo;
			reticleHolderDisplayInfo.SetPosition(reticlePosX, reticlePosY);
			reticleHolderDisplayInfo.SetScale(reticleScale, reticleScale);
			_reticleHolder.SetDisplayInfo(reticleHolderDisplayInfo);

			if (!Settings::bShowTargetBar) {
				return;
			}

			if (Settings::bHideVanillaTargetBar && !_bVanillaTargetBarHidden) {
				widgetHandler->HideVanillaTargetBar();
				_bVanillaTargetBarHidden = true;
			}

			auto bossMenu = WidgetHandler::GetSingleton()->GetBossMenu();
			if (bossMenu && bossMenu->IsDisplayingBoss(_currentTarget))
			{
				uiMovie->SetVariable("_root.TargetBar._visible", bFalse);
				return;
			} else {
				uiMovie->SetVariable("_root.TargetBar._visible", bTrue);
			}
			
			RE::NiPoint3 barPos;
			GetTargetPos(_currentTarget, barPos, Settings::uTargetBarAnchor == WidgetAnchor::kBody);
			barPos.z += Settings::fTargetBarZOffset;
			float barPosX;
			float barPosY;
			float barPosZ;

			RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, barPos, barPosX, barPosY, barPosZ, 1e-5f);

			barPosY = 1.0f - barPosY;  // Flip y for Flash coordinate system
			barPosY = rect.top + (rect.bottom - rect.top) * barPosY;
			barPosX = rect.left + (rect.right - rect.left) * barPosX;

			float barScale = 100.f * Settings::fTargetBarScale;

			RE::GFxValue::DisplayInfo barDisplayInfo;
			barDisplayInfo.SetPosition(barPosX, barPosY);
			barDisplayInfo.SetScale(barScale, barScale);
			_targetBar.SetDisplayInfo(barDisplayInfo);

			if (!_bInitialized)
			{
				// fill target name
				if (Settings::bShowTargetName) {
					RE::GFxValue textField;
					_targetBar.GetMember("TargetName", &textField);
					if (textField.IsDisplayObject()) {
						const char* targetName = _currentTarget.get()->GetDisplayFullName();
						textField.SetText(targetName);
					}
				}

				// target level
				if (Settings::uTargetLevelMode != TargetLockLevelMode::kDisable) {
					uint16_t playerLevel = RE::PlayerCharacter::GetSingleton()->GetLevel();
					uint16_t targetLevel = _currentTarget.get()->GetLevel();
					uint32_t color;

					if (playerLevel - targetLevel > Settings::uTargetLevelThreshold) {
						color = Settings::uTargetLevelMode == TargetLockLevelMode::kOutline ? _weakerColorOutline : _weakerColor;
					} else if (targetLevel - playerLevel > Settings::uTargetLevelThreshold) {
						color = _strongerColor;
					} else {
						color = _equalColor;
					}

					switch (Settings::uTargetLevelMode) {
					case TargetLockLevelMode::kIcon:
						{
							RE::GFxValue levelIcon;
							_targetBar.GetMember("LevelIcon", &levelIcon);
							if (levelIcon.IsDisplayObject()) {
								RE::GFxValue args[1];
								args[0].SetNumber(color);
								_targetBar.Invoke("setLevelIconColor", nullptr, args, 1);
							}
							break;
						}
					case TargetLockLevelMode::kText:
						{
							RE::GFxValue textField;
							_targetBar.GetMember("LevelText", &textField);
							if (textField.IsDisplayObject()) {
								std::string targetLevelString = std::to_string(targetLevel);
								textField.SetText(targetLevelString.c_str());

								RE::GFxValue args[1];
								args[0].SetNumber(color);
								_targetBar.Invoke("setLevelTextColor", nullptr, args, 1);
							}
							break;
						}
					case TargetLockLevelMode::kOutline:
						{
							RE::GFxValue frameOutline;
							_targetBar.GetMember("FrameOutline", &frameOutline);
							if (frameOutline.IsDisplayObject()) {
								RE::GFxValue args[1];
								args[0].SetNumber(color);
								_targetBar.Invoke("setFrameColor", nullptr, args, 1);
							}
							break;
						}
					}
				}

				_damage = 0.f;
				_healthPercent = -1;

				_bInitialized = true;
			}

			// set healthbar percentage
			float currentHealth = _currentTarget.get()->GetActorValue(RE::ActorValue::kHealth);
			float maxHealth = _currentTarget.get()->GetPermanentActorValue(RE::ActorValue::kHealth);
			float currentHealthPercent;
			if (currentHealth > 0.f && maxHealth > 0.f) {
				currentHealthPercent = currentHealth / maxHealth;
			} else {
				currentHealthPercent = 0.f;
			}

			if (_healthPercent == -1) // new target
			{
				RE::GFxValue args[1];
				args[0].SetNumber(currentHealthPercent);
				_targetBar.Invoke("setHealthPercent", nullptr, args, 1);
				_healthPercent = currentHealthPercent;

				_barUpdateTimer = Settings::fDamageDuration;

				_prevTargetHealth = currentHealth;
			}

			if (!ApproximatelyEqual(_healthPercent, currentHealthPercent))
			{
				bool bHealing = (_healthPercent < currentHealthPercent);

				RE::GFxValue args[2];
				args[0].SetNumber(_healthPercent);
				args[1].SetNumber(currentHealthPercent);
				_targetBar.Invoke("updateHealthPercent", nullptr, args, 2);
				_healthPercent = currentHealthPercent;

				if (!bHealing)
				{
					_barUpdateTimer = Settings::fDamageDuration;

					if (Settings::bShowDamage)
					{
						_damage += (_prevTargetHealth - currentHealth);
						_prevTargetHealth = currentHealth;

						RE::GFxValue arg[1];
						arg[0].SetNumber(static_cast<int32_t>(_damage));
						_targetBar.Invoke("updateDamage", nullptr, arg, 1);
					}
				}
			}

			if (_barUpdateTimer <= 0.f)
			{
				_targetBar.Invoke("hideDamage");
				_damage = 0;
			}
		}
		else {
			WidgetHandler::GetSingleton()->CloseTargetLockMenu();
		}
	}

	void TargetLockMenu::RefreshUI()
	{
		auto widgetHandler = WidgetHandler::GetSingleton();

		_bInitialized = false;

		const RE::GFxValue bTrue{ true };
		const RE::GFxValue bFalse{ false };

		if (uiMovie) {
			if (Settings::bShowReticle) {
				uiMovie->SetVariable("_root.ReticleHolder._visible", bTrue);

				{
					RE::GFxValue arg[1];
					arg[0].SetNumber(static_cast<uint32_t>(Settings::uReticleStyle));
					_reticle.Invoke("setReticleType", nullptr, arg, 1);
				}

				{
					float reticleAlpha = Settings::bUseHUDOpacityForReticle ? *g_fHUDOpacity : Settings::fReticleOpacity;
					reticleAlpha *= 100.f;
					RE::GFxValue arg[1];
					arg[0].SetNumber(reticleAlpha);
					_reticle.Invoke("setReticleAlpha", nullptr, arg, 1);
				}
			} else {
				uiMovie->SetVariable("_root.ReticleHolder._visible", bFalse);
			}

			if (Settings::bShowTargetBar) {
				uiMovie->SetVariable("_root.TargetBar._visible", bTrue);

				if (Settings::bShowTargetName) {
					uiMovie->SetVariable("_root.TargetBar.TargetName._visible", bTrue);
				} else {
					uiMovie->SetVariable("_root.TargetBar.TargetName._visible", bFalse);
				}

				switch (Settings::uTargetLevelMode) {
				case TargetLockLevelMode::kDisable:
					_targetBar.Invoke("levelDisplayNone");
					break;
				case TargetLockLevelMode::kIcon:
					_targetBar.Invoke("levelDisplayIcon");
					break;
				case TargetLockLevelMode::kText:
					_targetBar.Invoke("levelDisplayText");
					break;
				case TargetLockLevelMode::kOutline:
					_targetBar.Invoke("levelDisplayOutline");
					break;
				}

				{
					float barAlpha = Settings::bUseHUDOpacityForTargetBar ? *g_fHUDOpacity : Settings::fTargetBarOpacity;
					barAlpha *= 100.f;
					RE::GFxValue arg[1];
					arg[0].SetNumber(barAlpha);
					_targetBar.Invoke("setBarAlpha", nullptr, arg, 1);
				}

				if (Settings::bShowHealthPhantom) {
					RE::GFxValue arg[1];
					arg[0].SetBoolean(true);
					_targetBar.Invoke("phantomBarVisibility", nullptr, arg, 1);

					RE::GFxValue durArg[1];
					durArg[0].SetNumber(Settings::fHealthPhantomDuration);
					_targetBar.Invoke("setPhantomDuration", nullptr, durArg, 1);
				} else {
					RE::GFxValue arg[1];
					arg[0].SetBoolean(false);
					_targetBar.Invoke("phantomBarVisibility", nullptr, arg, 1);
				}

				if (!Settings::bShowDamage) {
					_targetBar.Invoke("hideDamage");
				}

				_healthPercent = -1;
				_barUpdateTimer = 0.f;
				_targetBar.Invoke("showBar");
			} else {
				uiMovie->SetVariable("_root.TargetBar._visible", bFalse);
			}
		}

		if (Settings::bHideVanillaTargetBar && Settings::bShowTargetBar) {
			if (!_bVanillaTargetBarHidden)
			{
				widgetHandler->HideVanillaTargetBar();
				_bVanillaTargetBarHidden = true;
			}
		}
	}

	void TargetLockMenu::Hide()
	{
		_hideCount++;
		if (_hideCount > 0) {
			depthPriority = -1;
			_view->SetVisible(false);
		}
	}

	void TargetLockMenu::Show()
	{
		if (_hideCount > 0) {
			_hideCount--;
			if (_hideCount == 0) {
				depthPriority = SORT_PRIORITY;
				_view->SetVisible(true);
			}
		}
	}

	/*void TargetLockMenu::Open(RE::TESObjectREFR* a_target)
	{
		m_lookHandler->SetInputEventHandlingEnabled(false);
		m_refTarget = a_target;
		m_isLockOn = true;

		RE::UIMessageQueue::GetSingleton()->AddMessage(m_menuName, RE::UI_MESSAGE_TYPE::kShow, nullptr);
	}*/

	void TargetLockMenu::OnOpen()
	{
		/*if (RE::UI::GetSingleton()->IsMenuOpen(RE::TweenMenu::MENU_NAME)) {
			Hide();
		}*/

		RefreshUI();

		ProcessDelegate();
	}

	void TargetLockMenu::OnClose()
	{
		/*if (uiMovie) {
		}*/

		auto widgetHandler = WidgetHandler::GetSingleton();

		if (Settings::bHideVanillaTargetBar && Settings::bShowTargetBar) {
			if (_bVanillaTargetBarHidden)
			{
				widgetHandler->ShowVanillaTargetBar();
				_bVanillaTargetBarHidden = false;
			}			
		}
	}

	void TargetLockMenu::ProcessDelegate()
	{
		WidgetHandler::GetSingleton()->ProcessTargetLockMenu(*this);
	}

	void TargetLockMenu::NewTarget(RE::ActorHandle a_newTarget)
	{
		_bInitialized = false;
		_reticle.Invoke("playTimeline");
		_targetBar.Invoke("hideDamage");
		_targetBar.Invoke("showBar");
		_currentTarget = a_newTarget;
	}
}
