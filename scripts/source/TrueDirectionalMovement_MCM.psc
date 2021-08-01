ScriptName TrueDirectionalMovement_MCM Extends SKI_ConfigBase

Int Function GetVersion()
	return 1
EndFunction

string[] _dialogueModeNames
string[] _targetLockProjectileAimTypeNames
string[] _reticleStyleNames
string[] _targetBarLevelModeNames
string[] _bossBarLevelModeNames
string[] _nameAlignmentNames
int _directionalMovementFlags
int _headtrackingFlags
int _cameraHeadtrackingFlags
int _widgetReticleFlags
int _widgetReticleOpacityFlags
int _widgetTargetBarFlags
int _widgetTargetBarOpacityFlags
int _widgetTargetLevelFlags
int _widgetHealthPhantomFlags
int _widgetDamageFlags
int _bossBarFlags
int _bossBarOpacityFlags
int _bossLevelFlags
int _bossHealthPhantomFlags
int _bossDamageFlags

; Called when the config menu is initialized.
Event OnConfigInit()
	ModName = "$TrueDirectionalMovement"
	Pages = new string[4]
	Pages[0] = "$TrueDirectionalMovement_GeneralPage"
	Pages[1] = "$TrueDirectionalMovement_TargetLockPage"
	Pages[2] = "$TrueDirectionalMovement_TargetLockWidgetPage"
	Pages[3] = "$TrueDirectionalMovement_BossBarWidgetPage"

	_dialogueModeNames = new string[3]
	_dialogueModeNames[0] = "$TrueDirectionalMovement_DialogueMode_Disable"
	_dialogueModeNames[1] = "$TrueDirectionalMovement_DialogueMode_Normal"
	_dialogueModeNames[2] = "$TrueDirectionalMovement_DialogueMode_FaceSpeaker"

	_reticleStyleNames = new string[3]
	_reticleStyleNames[0] = "$TrueDirectionalMovement_ReticleStyle_Default"
	_reticleStyleNames[1] = "$TrueDirectionalMovement_ReticleStyle_SimpleDot"
	_reticleStyleNames[2] = "$TrueDirectionalMovement_ReticleStyle_SimpleGlow"

	_targetLockProjectileAimTypeNames = new string[3]
	_targetLockProjectileAimTypeNames[0] = "$TrueDirectionalMovement_TargetLockProjectileAimType_FreeAim"
	_targetLockProjectileAimTypeNames[1] = "$TrueDirectionalMovement_TargetLockProjectileAimType_Predict"
	_targetLockProjectileAimTypeNames[2] = "$TrueDirectionalMovement_TargetLockProjectileAimType_Homing"

	_targetBarLevelModeNames = new string[4]
	_targetBarLevelModeNames[0] = "$TrueDirectionalMovement_LevelMode_Disable"
	_targetBarLevelModeNames[1] = "$TrueDirectionalMovement_LevelMode_Icon"
	_targetBarLevelModeNames[2] = "$TrueDirectionalMovement_LevelMode_Text"
	_targetBarLevelModeNames[3] = "$TrueDirectionalMovement_LevelMode_Outline"

	_bossBarLevelModeNames = new string[3]
	_bossBarLevelModeNames[0] = "$TrueDirectionalMovement_LevelMode_Disable"
	_bossBarLevelModeNames[1] = "$TrueDirectionalMovement_LevelMode_Icon"
	_bossBarLevelModeNames[2] = "$TrueDirectionalMovement_LevelMode_Text"

	_nameAlignmentNames = new string[3]
	_nameAlignmentNames[0] = "$TrueDirectionalMovement_NameAlignment_Center"
	_nameAlignmentNames[1] = "$TrueDirectionalMovement_NameAlignment_Left"
	_nameAlignmentNames[2] = "$TrueDirectionalMovement_NameAlignment_Right"
EndEvent


; Called when the config menu is closed.
Event OnConfigClose()
EndEvent


; Called when a version update of this script has been detected.
; a_version - The new version.
Event OnVersionUpdate(Int a_version)
EndEvent


; Called when a new page is selected, including the initial empty page.
; a_page - The name of the the current page, or "" if no page is selected.
Event OnPageReset(String a_page)
	If (a_page == "")
		LoadCustomContent("TrueDirectionalMovement/TDM_Splash.swf")
		return
	Else
		UnloadCustomContent()
	EndIf

	If (a_page == "$TrueDirectionalMovement_GeneralPage")
		SetCursorFillMode(TOP_TO_BOTTOM)

		AddHeaderOption("$TrueDirectionalMovement_GeneralSettings_HeaderText")
		AddToggleOptionST("DirectionalMovement_S", "$TrueDirectionalMovement_DirectionalMovement_OptionText", TrueDirectionalMovement.GetFreeCamera())
		AddMenuOptionST("DialogueMode_S", "$TrueDirectionalMovement_DialogueMode_OptionText", _dialogueModeNames[TrueDirectionalMovement.GetDialogueMode()], _directionalMovementFlags)
		AddToggleOptionST("FaceCrosshairWhileBlocking_S", "$TrueDirectionalMovement_FaceCrosshairWhileBlocking_OptionText", TrueDirectionalMovement.GetFaceCrosshairWhileBlocking(), _directionalMovementFlags)
		AddToggleOptionST("StopOnDirectionChange_S", "$TrueDirectionalMovement_StopOnDirectionChange_OptionText", TrueDirectionalMovement.GetStopOnDirectionChange(), _directionalMovementFlags)

		AddHeaderOption("$TrueDirectionalMovement_Headtracking_HeaderText")
		AddToggleOptionST("Headtracking_S", "$TrueDirectionalMovement_Headtracking_OptionText", TrueDirectionalMovement.GetHeadtracking())
		AddSliderOptionST("DialogueHeadtrackingDuration_S", "$TrueDirectionalMovement_DialogueHeadtrackingDuration_OptionText", TrueDirectionalMovement.GetDialogueHeadtrackingDuration(), "{1}", _headtrackingFlags)
		AddToggleOptionST("CameraHeadtracking_S", "$TrueDirectionalMovement_CameraHeadtracking_OptionText", TrueDirectionalMovement.GetCameraHeadtracking(), _headtrackingFlags)
		AddSliderOptionST("CameraHeadtrackingStrength_S", "$TrueDirectionalMovement_CameraHeadtrackingStrength_OptionText", TrueDirectionalMovement.GetCameraHeadtrackingStrength(), "{1}", _cameraHeadtrackingFlags)
		AddToggleOptionST("StopCameraHeadtrackingBehindPlayer_S", "$TrueDirectionalMovement_StopCameraHeadtrackingBehindPlayer_OptionText", TrueDirectionalMovement.GetStopCameraHeadtrackingBehindPlayer(), _cameraHeadtrackingFlags)

		SetCursorPosition(1)
		AddHeaderOption("$TrueDirectionalMovement_RotationSpeed_HeaderText")
		AddSliderOptionST("RunningRotationSpeedMult_S", "$TrueDirectionalMovement_RunningRotationSpeedMult_OptionText", TrueDirectionalMovement.GetRunningRotationSpeedMult(), "{2}", _directionalMovementFlags)
		AddSliderOptionST("SprintingRotationSpeedMult_S", "$TrueDirectionalMovement_SprintingRotationSpeedMult_OptionText", TrueDirectionalMovement.GetSprintingRotationSpeedMult(), "{2}", _directionalMovementFlags)
		AddSliderOptionST("AttackStartRotationSpeedMult_S", "$TrueDirectionalMovement_AttackStartRotationSpeedMult_OptionText", TrueDirectionalMovement.GetAttackStartRotationSpeedMult(), "{2}", _directionalMovementFlags)
		AddSliderOptionST("AttackMidRotationSpeedMult_S", "$TrueDirectionalMovement_AttackMidRotationSpeedMult_OptionText", TrueDirectionalMovement.GetAttackMidRotationSpeedMult(), "{2}", _directionalMovementFlags)
		AddSliderOptionST("AttackEndRotationSpeedMult_S", "$TrueDirectionalMovement_AttackEndRotationSpeedMult_OptionText", TrueDirectionalMovement.GetAttackEndRotationSpeedMult(), "{2}", _directionalMovementFlags)
		AddSliderOptionST("AirRotationSpeedMult_S", "$TrueDirectionalMovement_AirRotationSpeedMult_OptionText", TrueDirectionalMovement.GetAirRotationSpeedMult(), "{2}", _directionalMovementFlags)
		AddToggleOptionST("FaceCrosshairInstantly_S", "$TrueDirectionalMovement_FaceCrosshairInstantly_OptionText", TrueDirectionalMovement.GetFaceCrosshairInstantly(), _directionalMovementFlags)
		AddToggleOptionST("DisableAttackRotationMultipliersForTransformations_S", "$TrueDirectionalMovement_DisableAttackRotationMultipliersForTransformations_OptionText", TrueDirectionalMovement.GetDisableAttackRotationMultipliersForTransformations(), _directionalMovementFlags)

		UpdateDirectionalMovementFlags()
		UpdateHeadtrackingFlags()

	ElseIf (a_page == "$TrueDirectionalMovement_TargetLockPage")
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)

		AddHeaderOption("$TrueDirectionalMovement_TargetLockSettings_HeaderText")
		AddToggleOptionST("AutoTargetNextOnDeath_S", "$TrueDirectionalMovement_AutoTargetNextOnDeath_OptionText", TrueDirectionalMovement.GetAutoTargetNextOnDeath())
		AddToggleOptionST("TargetLockTestLOS_S", "$TrueDirectionalMovement_TargetLockTestLOS_OptionText", TrueDirectionalMovement.GetTargetLockTestLOS())
		AddToggleOptionST("TargetLockHostileActorsOnly_S", "$TrueDirectionalMovement_TargetLockHostileActorsOnly_OptionText", TrueDirectionalMovement.GetTargetLockHostileActorsOnly())

		AddHeaderOption("$TrueDirectionalMovement_CameraSettings_HeaderText")
		AddSliderOptionST("TargetLockDistance_S", "$TrueDirectionalMovement_TargetLockDistance_OptionText", TrueDirectionalMovement.GetTargetLockDistance())
		AddSliderOptionST("TargetLockPitchAdjustSpeed_S", "$TrueDirectionalMovement_TargetLockPitchAdjustSpeed_OptionText", TrueDirectionalMovement.GetTargetLockPitchAdjustSpeed(), "{1}")
		AddSliderOptionST("TargetLockYawAdjustSpeed_S", "$TrueDirectionalMovement_TargetLockYawAdjustSpeed_OptionText", TrueDirectionalMovement.GetTargetLockYawAdjustSpeed(), "{1}")
		AddSliderOptionST("TargetLockPitchOffsetStrength_S", "$TrueDirectionalMovement_TargetLockPitchOffsetStrength_OptionText", TrueDirectionalMovement.GetTargetLockPitchOffsetStrength(), "{2}")

		AddHeaderOption("$TrueDirectionalMovement_ProjectileSettings_HeaderText")
		AddMenuOptionST("TargetLockArrowAimType_S", "$TrueDirectionalMovement_TargetLockArrowAimType_OptionText", _targetLockProjectileAimTypeNames[TrueDirectionalMovement.GetTargetLockArrowAimType()])
		AddMenuOptionST("TargetLockMissileAimType_S", "$TrueDirectionalMovement_TargetLockMissileAimType_OptionText", _targetLockProjectileAimTypeNames[TrueDirectionalMovement.GetTargetLockMissileAimType()])

		SetCursorPosition(1)
		AddHeaderOption("$TrueDirectionalMovement_Controls_HeaderText")
		AddKeyMapOptionST("TargetLockKey_K", "$TrueDirectionalMovement_TargetLockKey_OptionText", TrueDirectionalMovement.GetTargetLockKey())
		AddToggleOptionST("TargetLockUseMouse_S", "$TrueDirectionalMovement_TargetLockUseMouse_OptionText", TrueDirectionalMovement.GetTargetLockUseMouse())
		AddToggleOptionST("TargetLockUseScrollWheel_S", "$TrueDirectionalMovement_TargetLockUseScrollWheel_OptionText", TrueDirectionalMovement.GetTargetLockUseScrollWheel())
		AddKeyMapOptionST("SwitchTargetLeftKey_K", "$TrueDirectionalMovement_SwitchTargetLeftKey_OptionText", TrueDirectionalMovement.GetSwitchTargetLeftKey())
		AddKeyMapOptionST("SwitchTargetRightKey_K", "$TrueDirectionalMovement_SwitchTargetRightKey_OptionText", TrueDirectionalMovement.GetSwitchTargetRightKey())

	ElseIf (a_page == "$TrueDirectionalMovement_TargetLockWidgetPage")
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)

		AddHeaderOption("$TrueDirectionalMovement_VisibilitySettings_HeaderText")
		AddToggleOptionST("WidgetShowReticle_S", "$TrueDirectionalMovement_WidgetShowReticle_OptionText", TrueDirectionalMovement.GetWidgetShowReticle())
		AddToggleOptionST("WidgetShowTargetBar_S", "$TrueDirectionalMovement_WidgetShowTargetBar_OptionText", TrueDirectionalMovement.GetWidgetShowTargetBar())
		AddToggleOptionST("WidgetShowSoftTargetBar_S", "$TrueDirectionalMovement_WidgetShowSoftTargetBar_OptionText", TrueDirectionalMovement.GetWidgetShowSoftTargetBar())
		AddToggleOptionST("WidgetShowTargetName_S", "$TrueDirectionalMovement_WidgetShowTargetName_OptionText", TrueDirectionalMovement.GetWidgetShowTargetName(), _widgetTargetBarFlags)
		AddToggleOptionST("WidgetShowDamage_S", "$TrueDirectionalMovement_WidgetShowDamage_OptionText", TrueDirectionalMovement.GetWidgetShowDamage(), _widgetTargetBarFlags)
		AddToggleOptionST("WidgetShowHealthPhantom_S", "$TrueDirectionalMovement_WidgetShowHealthPhantom_OptionText", TrueDirectionalMovement.GetWidgetShowHealthPhantom(), _widgetTargetBarFlags)
		AddToggleOptionST("WidgetHideVanillaTargetBar_S", "$TrueDirectionalMovement_WidgetHideVanillaTargetBar_OptionText", TrueDirectionalMovement.GetWidgetHideVanillaTargetBar(), _widgetTargetBarFlags)

		AddHeaderOption("$TrueDirectionalMovement_WidgetSettings_HeaderText")
		AddMenuOptionST("WidgetReticleStyle_S", "$TrueDirectionalMovement_WidgetReticleStyle_OptionText", _reticleStyleNames[TrueDirectionalMovement.GetWidgetReticleStyle()], _widgetReticleFlags)
		AddMenuOptionST("WidgetTargetLevelMode_S", "$TrueDirectionalMovement_WidgetTargetLevelMode_OptionText", _targetBarLevelModeNames[TrueDirectionalMovement.GetWidgetTargetLevelMode()], _widgetTargetBarFlags)
		AddSliderOptionST("WidgetTargetLevelThreshold_S", "$TrueDirectionalMovement_WidgetTargetLevelThreshold_OptionText", TrueDirectionalMovement.GetWidgetTargetLevelThreshold(), "{0}", _widgetTargetLevelFlags)
		AddSliderOptionST("WidgetHealthPhantomDuration_S", "$TrueDirectionalMovement_WidgetHealthPhantomDuration_OptionText", TrueDirectionalMovement.GetWidgetHealthPhantomDuration(), "{2}s", _widgetHealthPhantomFlags)
		AddSliderOptionST("WidgetDamageDuration_S", "$TrueDirectionalMovement_WidgetDamageDuration_OptionText", TrueDirectionalMovement.GetWidgetDamageDuration(), "{2}s", _widgetDamageFlags)
		
		SetCursorPosition(1)
		AddHeaderOption("$TrueDirectionalMovement_ScaleOpacitySettings_HeaderText")
		AddSliderOptionST("WidgetReticleScale_S", "$TrueDirectionalMovement_WidgetReticleScale_OptionText", TrueDirectionalMovement.GetWidgetReticleScale(), "{1}", _widgetReticleFlags)
		AddToggleOptionST("WidgetUseHUDOpacityForReticle_S", "$TrueDirectionalMovement_WidgetUseHUDOpacityForReticle_OptionText", TrueDirectionalMovement.GetWidgetUseHUDOpacityForReticle(), _widgetReticleFlags)
		AddSliderOptionST("WidgetReticleOpacity_S", "$TrueDirectionalMovement_WidgetReticleOpacity_OptionText", TrueDirectionalMovement.GetWidgetReticleOpacity(), "{2}", _widgetReticleOpacityFlags)
		AddSliderOptionST("WidgetTargetBarScale_S", "$TrueDirectionalMovement_WidgetTargetBarScale_OptionText", TrueDirectionalMovement.GetWidgetTargetBarScale(), "{1}", _widgetTargetBarFlags)
		AddToggleOptionST("WidgetUseHUDOpacityForTargetBar_S", "$TrueDirectionalMovement_WidgetUseHUDOpacityForTargetBar_OptionText", TrueDirectionalMovement.GetWidgetUseHUDOpacityForTargetBar(), _widgetTargetBarFlags)
		AddSliderOptionST("WidgetTargetBarOpacity_S", "$TrueDirectionalMovement_WidgetTargetBarOpacity_OptionText", TrueDirectionalMovement.GetWidgetTargetBarOpacity(), "{2}", _widgetTargetBarOpacityFlags)

		UpdateTargetBarFlags()
		UpdateReticleFlags()

	ElseIf (a_page == "$TrueDirectionalMovement_BossBarWidgetPage")
		SetCursorFillMode(TOP_TO_BOTTOM)
		SetCursorPosition(0)

		UpdateBossBarFlags()

		AddHeaderOption("$TrueDirectionalMovement_VisibilitySettings_HeaderText")
		AddToggleOptionST("ShowBossBar_S", "$TrueDirectionalMovement_ShowBossBar_OptionText", TrueDirectionalMovement.GetShowBossBar())
		AddToggleOptionST("ShowBossDamage_S", "$TrueDirectionalMovement_ShowBossDamage_OptionText", TrueDirectionalMovement.GetShowBossDamage(), _bossBarFlags)
		AddToggleOptionST("ShowBossHealthPhantom_S", "$TrueDirectionalMovement_ShowBossHealthPhantom_OptionText", TrueDirectionalMovement.GetShowBossHealthPhantom(), _bossBarFlags)

		AddHeaderOption("$TrueDirectionalMovement_WidgetSettings_HeaderText")
		AddMenuOptionST("BossNameAlignment_S", "$TrueDirectionalMovement_BossNameAlignment_OptionText", _nameAlignmentNames[TrueDirectionalMovement.GetBossNameAlignment()], _bossBarFlags)
		AddMenuOptionST("BossLevelMode_S", "$TrueDirectionalMovement_BossLevelMode_OptionText", _bossBarLevelModeNames[TrueDirectionalMovement.GetBossLevelMode()], _bossBarFlags)
		AddSliderOptionST("BossLevelThreshold_S", "$TrueDirectionalMovement_BossLevelThreshold_OptionText", TrueDirectionalMovement.GetBossLevelThreshold(), "{0}", _bossLevelFlags)
		AddSliderOptionST("BossHealthPhantomDuration_S", "$TrueDirectionalMovement_BossHealthPhantomDuration_OptionText", TrueDirectionalMovement.GetBossHealthPhantomDuration(), "{2}s", _bossHealthPhantomFlags)
		AddSliderOptionST("BossDamageDuration_S", "$TrueDirectionalMovement_BossDamageDuration_OptionText", TrueDirectionalMovement.GetBossDamageDuration(), "{2}s", _bossDamageFlags)

		SetCursorPosition(1)
		AddHeaderOption("$TrueDirectionalMovement_ScaleOpacityPositionSettings_HeaderText")
		AddSliderOptionST("BossBarScale_S", "$TrueDirectionalMovement_BossBarScale_OptionText", TrueDirectionalMovement.GetBossBarScale(), "{1}", _bossBarFlags)
		AddToggleOptionST("UseHUDOpacityForBossBar_S", "$TrueDirectionalMovement_UseHUDOpacityForBossBar_OptionText", TrueDirectionalMovement.GetUseHUDOpacityForBossBar(), _bossBarFlags)
		AddSliderOptionST("BossBarOpacity_S", "$TrueDirectionalMovement_BossBarOpacity_OptionText", TrueDirectionalMovement.GetBossBarOpacity(), "{2}", _bossBarOpacityFlags)
		AddSliderOptionST("BossBarX_S", "$TrueDirectionalMovement_BossBarX_OptionText", TrueDirectionalMovement.GetBossBarX(), "{3}", _bossBarFlags)
		AddSliderOptionST("BossBarY_S", "$TrueDirectionalMovement_BossBarY_OptionText", TrueDirectionalMovement.GetBossBarY(), "{3}", _bossBarFlags)
		AddToggleOptionST("BossBarOffsetSubtitles_S", "$TrueDirectionalMovement_BossBarOffsetSubtitles_OptionText", TrueDirectionalMovement.GetBossBarOffsetSubtitles(), _bossBarFlags)
		AddSliderOptionST("MultipleBossBarsOffset_S", "$TrueDirectionalMovement_MultipleBossBarsOffset_OptionText", TrueDirectionalMovement.GetMultipleBossBarsOffset(), "{1}", _bossBarFlags)
		AddToggleOptionST("MultipleBossBarsStackUpwards_S", "$TrueDirectionalMovement_MultipleBossBarsStackUpwards_OptionText", TrueDirectionalMovement.GetMultipleBossBarsStackUpwards(), _bossBarFlags)

		

	EndIf
EndEvent


Function UpdateDirectionalMovementFlags()
	If (TrueDirectionalMovement.GetFreeCamera())
		_directionalMovementFlags = OPTION_FLAG_NONE
	Else
		_directionalMovementFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_directionalMovementFlags, true, "DialogueMode_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "FaceCrosshairWhileBlocking_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "RunningRotationSpeedMult_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "SprintingRotationSpeedMult_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "AttackStartRotationSpeedMult_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "AttackMidRotationSpeedMult_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "AttackEndRotationSpeedMult_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "AirRotationSpeedMult_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "DisableAttackRotationMultipliersForTransformations_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "FaceCrosshairInstantly_S")
	SetOptionFlagsST(_directionalMovementFlags, true, "StopOnDirectionChange_S")
EndFunction


State DirectionalMovement_S
	Event OnSelectST()
		TrueDirectionalMovement.SetFreeCamera(!TrueDirectionalMovement.GetFreeCamera())
		UpdateDirectionalMovementFlags()		
		SetToggleOptionValueST(TrueDirectionalMovement.GetFreeCamera())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetFreeCamera(true)
		UpdateDirectionalMovementFlags()	
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_DirectionalMovement_InfoText")
	EndEvent
EndState


State DialogueMode_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetDialogueMode())
		SetMenuDialogDefaultIndex(2)
		SetMenuDialogOptions(_dialogueModeNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetDialogueMode(a_index)
		SetMenuOptionValueST(_dialogueModeNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetDialogueMode(2)
		SetMenuOptionValueST(_dialogueModeNames[2])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_DialogueMode_InfoText")
	EndEvent
EndState


State FaceCrosshairWhileBlocking_S
	Event OnSelectST()
		TrueDirectionalMovement.SetFaceCrosshairWhileBlocking(!TrueDirectionalMovement.GetFaceCrosshairWhileBlocking())
		SetToggleOptionValueST(TrueDirectionalMovement.GetFaceCrosshairWhileBlocking())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetFaceCrosshairWhileBlocking(true)
		UpdateDirectionalMovementFlags()	
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_FaceCrosshairWhileBlocking_InfoText")
	EndEvent
EndState


Function UpdateHeadtrackingFlags()
	If (TrueDirectionalMovement.GetHeadtracking())
		_headtrackingFlags = OPTION_FLAG_NONE
		If (TrueDirectionalMovement.GetCameraHeadtracking())
			_cameraHeadtrackingFlags = OPTION_FLAG_NONE
		Else
			_cameraHeadtrackingFlags = OPTION_FLAG_DISABLED
		Endif
	Else
		_headtrackingFlags = OPTION_FLAG_DISABLED
		_cameraHeadtrackingFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_headtrackingFlags, true, "DialogueHeadtrackingDuration_S")
	SetOptionFlagsST(_headtrackingFlags, true, "CameraHeadtracking_S")
	SetOptionFlagsST(_cameraHeadtrackingFlags, true, "CameraHeadtrackingStrength_S")
	SetOptionFlagsST(_cameraHeadtrackingFlags, true, "StopCameraHeadtrackingBehindPlayer_S")
EndFunction


State Headtracking_S
	Event OnSelectST()
		TrueDirectionalMovement.SetHeadtracking(!TrueDirectionalMovement.GetHeadtracking())
		UpdateHeadtrackingFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetHeadtracking())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetHeadtracking(true)
		UpdateHeadtrackingFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_Headtracking_InfoText")
	EndEvent
EndState


State DialogueHeadtrackingDuration_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetDialogueHeadtrackingDuration())
		SetSliderDialogDefaultValue(3.0)
		SetSliderDialogRange(0.5, 10.0)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetDialogueHeadtrackingDuration(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetDialogueHeadtrackingDuration(3.0)
		SetSliderOptionValueST(3.0, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_DialogueHeadtrackingDuration_InfoText")
	EndEvent
EndState


Function UpdateCameraHeadtrackingFlags()
	If (TrueDirectionalMovement.GetCameraHeadtracking())
		_cameraHeadtrackingFlags = OPTION_FLAG_NONE
	Else
		_cameraHeadtrackingFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_cameraHeadtrackingFlags, true, "CameraHeadtrackingStrength_S")
	SetOptionFlagsST(_cameraHeadtrackingFlags, true, "StopCameraHeadtrackingBehindPlayer_S")
EndFunction


State CameraHeadtracking_S
	Event OnSelectST()
		TrueDirectionalMovement.SetCameraHeadtracking(!TrueDirectionalMovement.GetCameraHeadtracking())
		UpdateCameraHeadtrackingFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetCameraHeadtracking())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetCameraHeadtracking(true)
		UpdateCameraHeadtrackingFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_CameraHeadtracking_InfoText")
	EndEvent
EndState


State CameraHeadtrackingStrength_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetCameraHeadtrackingStrength())
		SetSliderDialogDefaultValue(0.5)
		SetSliderDialogRange(0.1, 2.0)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetCameraHeadtrackingStrength(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetCameraHeadtrackingStrength(0.5)
		SetSliderOptionValueST(0.5, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_CameraHeadtrackingStrength_InfoText")
	EndEvent
EndState


State StopCameraHeadtrackingBehindPlayer_S
	Event OnSelectST()
		TrueDirectionalMovement.SetStopCameraHeadtrackingBehindPlayer(!TrueDirectionalMovement.GetStopCameraHeadtrackingBehindPlayer())
		SetToggleOptionValueST(TrueDirectionalMovement.GetStopCameraHeadtrackingBehindPlayer())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetStopCameraHeadtrackingBehindPlayer(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_StopCameraHeadtrackingBehindPlayer_InfoText")
	EndEvent
EndState


State RunningRotationSpeedMult_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetRunningRotationSpeedMult())
		SetSliderDialogDefaultValue(1.5)
		SetSliderDialogRange(0.1, 5.0)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetRunningRotationSpeedMult(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetRunningRotationSpeedMult(1.5)
		SetSliderOptionValueST(1.5, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_RunningRotationSpeedMult_InfoText")
	EndEvent
EndState


State SprintingRotationSpeedMult_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetSprintingRotationSpeedMult())
		SetSliderDialogDefaultValue(2.0)
		SetSliderDialogRange(0.1, 5.0)
		SetSliderDialogInterval(0.05)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetSprintingRotationSpeedMult(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetSprintingRotationSpeedMult(2.0)
		SetSliderOptionValueST(2.0, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_SprintingRotationSpeedMult_InfoText")
	EndEvent
EndState


State AttackStartRotationSpeedMult_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetAttackStartRotationSpeedMult())
		SetSliderDialogDefaultValue(5.0)
		SetSliderDialogRange(0.0, 20.0)
		SetSliderDialogInterval(0.05)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetAttackStartRotationSpeedMult(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetAttackStartRotationSpeedMult(5.0)
		SetSliderOptionValueST(5.0, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_AttackStartRotationSpeedMult_InfoText")
	EndEvent
EndState


State AttackMidRotationSpeedMult_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetAttackMidRotationSpeedMult())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.0, 4.0)
		SetSliderDialogInterval(0.05)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetAttackMidRotationSpeedMult(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetAttackMidRotationSpeedMult(1.0)
		SetSliderOptionValueST(1.0, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_AttackMidRotationSpeedMult_InfoText")
	EndEvent
EndState


State AttackEndRotationSpeedMult_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetAttackEndRotationSpeedMult())
		SetSliderDialogDefaultValue(0.0)
		SetSliderDialogRange(0.0, 4.0)
		SetSliderDialogInterval(0.05)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetAttackEndRotationSpeedMult(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetAttackEndRotationSpeedMult(0.0)
		SetSliderOptionValueST(0.0, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_AttackEndRotationSpeedMult_InfoText")
	EndEvent
EndState


State AirRotationSpeedMult_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetAirRotationSpeedMult())
		SetSliderDialogDefaultValue(0.5)
		SetSliderDialogRange(0.0, 1.0)
		SetSliderDialogInterval(0.05)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetAirRotationSpeedMult(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetAirRotationSpeedMult(0.5)
		SetSliderOptionValueST(0.5, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_AirRotationSpeedMult_InfoText")
	EndEvent
EndState


State DisableAttackRotationMultipliersForTransformations_S
	Event OnSelectST()
		TrueDirectionalMovement.SetDisableAttackRotationMultipliersForTransformations(!TrueDirectionalMovement.GetDisableAttackRotationMultipliersForTransformations())
		SetToggleOptionValueST(TrueDirectionalMovement.GetDisableAttackRotationMultipliersForTransformations())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetDisableAttackRotationMultipliersForTransformations(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_DisableAttackRotationMultipliersForTransformations_InfoText")
	EndEvent
EndState


State StopOnDirectionChange_S
	Event OnSelectST()
		TrueDirectionalMovement.SetStopOnDirectionChange(!TrueDirectionalMovement.GetStopOnDirectionChange())
		SetToggleOptionValueST(TrueDirectionalMovement.GetStopOnDirectionChange())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetStopOnDirectionChange(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_StopOnDirectionChange_InfoText")
	EndEvent
EndState


State FaceCrosshairInstantly_S
	Event OnSelectST()
		TrueDirectionalMovement.SetFaceCrosshairInstantly(!TrueDirectionalMovement.GetFaceCrosshairInstantly())
		SetToggleOptionValueST(TrueDirectionalMovement.GetFaceCrosshairInstantly())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetFaceCrosshairInstantly(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_FaceCrosshairInstantly_InfoText")
	EndEvent
EndState


State TargetLockDistance_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetTargetLockDistance())
		SetSliderDialogDefaultValue(2000)
		SetSliderDialogRange(100, 8000)
		SetSliderDialogInterval(1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetTargetLockDistance(a_value)
		SetSliderOptionValueST(a_value)
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockDistance(2000)
		SetSliderOptionValueST(2000)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockDistance_InfoText")
	EndEvent
EndState


State TargetLockPitchAdjustSpeed_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetTargetLockPitchAdjustSpeed())
		SetSliderDialogDefaultValue(2.0)
		SetSliderDialogRange(0.1, 20.0)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetTargetLockPitchAdjustSpeed(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockPitchAdjustSpeed(2.0)
		SetSliderOptionValueST(2.0, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockPitchAdjustSpeed_InfoText")
	EndEvent
EndState


State TargetLockYawAdjustSpeed_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetTargetLockYawAdjustSpeed())
		SetSliderDialogDefaultValue(8.0)
		SetSliderDialogRange(0.1, 40.0)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetTargetLockYawAdjustSpeed(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockYawAdjustSpeed(8.0)
		SetSliderOptionValueST(8.0, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockYawAdjustSpeed_InfoText")
	EndEvent
EndState


State TargetLockPitchOffsetStrength_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetTargetLockPitchOffsetStrength())
		SetSliderDialogDefaultValue(0.25)
		SetSliderDialogRange(0.0, 1)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetTargetLockPitchOffsetStrength(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockPitchOffsetStrength(0.25)
		SetSliderOptionValueST(0.25, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockPitchOffsetStrength_InfoText")
	EndEvent
EndState


State TargetLockUseMouse_S
	Event OnSelectST()
		TrueDirectionalMovement.SetTargetLockUseMouse(!TrueDirectionalMovement.GetTargetLockUseMouse())
		SetToggleOptionValueST(TrueDirectionalMovement.GetTargetLockUseMouse())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockUseMouse(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockUseMouse_InfoText")
	EndEvent
EndState


State TargetLockUseScrollWheel_S
	Event OnSelectST()
		TrueDirectionalMovement.SetTargetLockUseScrollWheel(!TrueDirectionalMovement.GetTargetLockUseScrollWheel())
		SetToggleOptionValueST(TrueDirectionalMovement.GetTargetLockUseScrollWheel())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockUseScrollWheel(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockUseScrollWheel_InfoText")
	EndEvent
EndState


State AutoTargetNextOnDeath_S
	Event OnSelectST()
		TrueDirectionalMovement.SetAutoTargetNextOnDeath(!TrueDirectionalMovement.GetAutoTargetNextOnDeath())
		SetToggleOptionValueST(TrueDirectionalMovement.GetAutoTargetNextOnDeath())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetAutoTargetNextOnDeath(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_AutoTargetNextOnDeath_InfoText")
	EndEvent
EndState


State TargetLockTestLOS_S
	Event OnSelectST()
		TrueDirectionalMovement.SetTargetLockTestLOS(!TrueDirectionalMovement.GetTargetLockTestLOS())
		SetToggleOptionValueST(TrueDirectionalMovement.GetTargetLockTestLOS())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockTestLOS(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockTestLOS_InfoText")
	EndEvent
EndState


State TargetLockHostileActorsOnly_S
	Event OnSelectST()
		TrueDirectionalMovement.SetTargetLockHostileActorsOnly(!TrueDirectionalMovement.GetTargetLockHostileActorsOnly())
		SetToggleOptionValueST(TrueDirectionalMovement.GetTargetLockHostileActorsOnly())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockHostileActorsOnly(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockHostileActorsOnly_InfoText")
	EndEvent
EndState


State TargetLockArrowAimType_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetTargetLockArrowAimType())
		SetMenuDialogDefaultIndex(1)
		SetMenuDialogOptions(_targetLockProjectileAimTypeNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetTargetLockArrowAimType(a_index)
		SetMenuOptionValueST(_targetLockProjectileAimTypeNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockArrowAimType(1)
		SetMenuOptionValueST(_targetLockProjectileAimTypeNames[1])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockArrowAimType_InfoText")
	EndEvent
EndState


State TargetLockMissileAimType_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetTargetLockMissileAimType())
		SetMenuDialogDefaultIndex(1)
		SetMenuDialogOptions(_targetLockProjectileAimTypeNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetTargetLockMissileAimType(a_index)
		SetMenuOptionValueST(_targetLockProjectileAimTypeNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockMissileAimType(1)
		SetMenuOptionValueST(_targetLockProjectileAimTypeNames[1])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockMissileAimType_InfoText")
	EndEvent
EndState


Function UpdateTargetBarFlags()
	If (TrueDirectionalMovement.GetWidgetShowTargetBar())
		_widgetTargetBarFlags = OPTION_FLAG_NONE
		If (TrueDirectionalMovement.GetWidgetTargetLevelMode() > 0)
			_widgetTargetLevelFlags = OPTION_FLAG_NONE
		Else
			_widgetTargetLevelFlags = OPTION_FLAG_DISABLED
		EndIf
		If (TrueDirectionalMovement.GetWidgetShowDamage())
			_widgetDamageFlags = OPTION_FLAG_NONE
		Else
			_widgetDamageFlags = OPTION_FLAG_DISABLED
		EndIf
		If (TrueDirectionalMovement.GetWidgetShowHealthPhantom())
			_widgetHealthPhantomFlags = OPTION_FLAG_NONE
		Else
			_widgetHealthPhantomFlags = OPTION_FLAG_DISABLED
		EndIf
		If (!TrueDirectionalMovement.GetWidgetUseHUDOpacityForTargetBar())
			_widgetTargetBarOpacityFlags = OPTION_FLAG_NONE
		Else
			_widgetTargetBarOpacityFlags = OPTION_FLAG_DISABLED
		EndIf
	Else
		_widgetTargetBarFlags = OPTION_FLAG_DISABLED
		_widgetTargetLevelFlags = OPTION_FLAG_DISABLED
		_widgetDamageFlags = OPTION_FLAG_DISABLED
		_widgetHealthPhantomFlags = OPTION_FLAG_DISABLED
		_widgetTargetBarOpacityFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetShowSoftTargetBar_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetShowTargetName_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetTargetLevelMode_S")
	SetOptionFlagsST(_widgetTargetLevelFlags, true, "WidgetTargetLevelThreshold_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetShowDamage_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetShowHealthPhantom_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetHideVanillaTargetBar_S")
	SetOptionFlagsST(_widgetHealthPhantomFlags, true, "WidgetHealthPhantomDuration_S")
	SetOptionFlagsST(_widgetDamageFlags, true, "WidgetDamageDuration_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetTargetBarScale_S")
	SetOptionFlagsST(_widgetTargetBarFlags, true, "WidgetUseHUDOpacityForTargetBar_S")
	SetOptionFlagsST(_widgetTargetBarOpacityFlags, true, "WidgetTargetBarOpacity_S")
EndFunction


Function UpdateReticleFlags()
	If (TrueDirectionalMovement.GetWidgetShowReticle())
		_widgetReticleFlags = OPTION_FLAG_NONE
		If (!TrueDirectionalMovement.GetWidgetUseHUDOpacityForReticle())
			_widgetReticleOpacityFlags = OPTION_FLAG_NONE
		Else
			_widgetReticleOpacityFlags = OPTION_FLAG_DISABLED
		EndIf
	Else
		_widgetReticleFlags = OPTION_FLAG_DISABLED
		_widgetReticleOpacityFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetReticleFlags, true, "WidgetReticleScale_S")
	SetOptionFlagsST(_widgetReticleFlags, true, "WidgetReticleStyle_S")
	SetOptionFlagsST(_widgetReticleFlags, true, "WidgetUseHUDOpacityForReticle_S")
	SetOptionFlagsST(_widgetReticleOpacityFlags, true, "WidgetReticleOpacity_S")
EndFunction


State WidgetShowReticle_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetShowReticle(!TrueDirectionalMovement.GetWidgetShowReticle())
		UpdateReticleFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetShowReticle())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetShowReticle(true)
		UpdateReticleFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetShowReticle_InfoText")
	EndEvent
EndState


State WidgetReticleStyle_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetWidgetReticleStyle())
		SetMenuDialogDefaultIndex(1)
		SetMenuDialogOptions(_reticleStyleNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetWidgetReticleStyle(a_index)
		SetMenuOptionValueST(_reticleStyleNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetReticleStyle(0)
		SetMenuOptionValueST(_reticleStyleNames[0])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetReticleStyle_InfoText")
	EndEvent
EndState


State WidgetShowTargetBar_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetShowTargetBar(!TrueDirectionalMovement.GetWidgetShowTargetBar())
		UpdateTargetBarFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetShowTargetBar())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetShowTargetBar(true)
		UpdateTargetBarFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetShowTargetBar_InfoText")
	EndEvent
EndState


State WidgetShowSoftTargetBar_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetShowSoftTargetBar(!TrueDirectionalMovement.GetWidgetShowSoftTargetBar())
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetShowSoftTargetBar())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetShowSoftTargetBar(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetShowSoftTargetBar_InfoText")
	EndEvent
EndState


State WidgetShowTargetName_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetShowTargetName(!TrueDirectionalMovement.GetWidgetShowTargetName())
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetShowTargetName())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetShowTargetName(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetShowTargetName_InfoText")
	EndEvent
EndState


Function UpdateTargetLevelFlags()
	If (TrueDirectionalMovement.GetWidgetTargetLevelMode() > 0)
		_widgetTargetLevelFlags = OPTION_FLAG_NONE
	Else
		_widgetTargetLevelFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetTargetLevelFlags, true, "WidgetTargetLevelThreshold_S")
EndFunction


State WidgetTargetLevelMode_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetWidgetTargetLevelMode())
		SetMenuDialogDefaultIndex(1)
		SetMenuDialogOptions(_targetBarLevelModeNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetWidgetTargetLevelMode(a_index)
		UpdateTargetLevelFlags()		
		SetMenuOptionValueST(_targetBarLevelModeNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetTargetLevelMode(3)
		UpdateTargetLevelFlags()
		SetMenuOptionValueST(_targetBarLevelModeNames[3])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetTargetLevelMode_InfoText")
	EndEvent
EndState


State WidgetTargetLevelThreshold_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetTargetLevelThreshold())
		SetSliderDialogDefaultValue(10)
		SetSliderDialogRange(0, 50)
		SetSliderDialogInterval(1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetTargetLevelThreshold(a_value as Int)
		SetSliderOptionValueST(a_value, "{0}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetTargetLevelThreshold(10)
		SetSliderOptionValueST(10, "{0}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetTargetLevelThreshold_InfoText")
	EndEvent
EndState


Function UpdateTargetDamageFlags()
	If (TrueDirectionalMovement.GetWidgetShowDamage())
		_widgetDamageFlags = OPTION_FLAG_NONE
	Else
		_widgetDamageFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetDamageFlags, true, "WidgetDamageDuration_S")
EndFunction


State WidgetShowDamage_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetShowDamage(!TrueDirectionalMovement.GetWidgetShowDamage())
		UpdateTargetDamageFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetShowDamage())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetShowDamage(true)
		UpdateTargetDamageFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetShowDamage_InfoText")
	EndEvent
EndState


Function UpdateTargetHealthPhantomFlags()
	If (TrueDirectionalMovement.GetWidgetShowHealthPhantom())
		_widgetHealthPhantomFlags = OPTION_FLAG_NONE
	Else
		_widgetHealthPhantomFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetHealthPhantomFlags, true, "WidgetHealthPhantomDuration_S")
EndFunction


State WidgetShowHealthPhantom_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetShowHealthPhantom(!TrueDirectionalMovement.GetWidgetShowHealthPhantom())
		UpdateTargetHealthPhantomFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetShowHealthPhantom())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetShowHealthPhantom(true)
		UpdateTargetHealthPhantomFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetShowHealthPhantom_InfoText")
	EndEvent
EndState


State WidgetHideVanillaTargetBar_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetHideVanillaTargetBar(!TrueDirectionalMovement.GetWidgetHideVanillaTargetBar())
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetHideVanillaTargetBar())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetHideVanillaTargetBar(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetHideVanillaTargetBar_InfoText")
	EndEvent
EndState


State WidgetHealthPhantomDuration_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetHealthPhantomDuration())
		SetSliderDialogDefaultValue(0.75)
		SetSliderDialogRange(0.01, 5)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetHealthPhantomDuration(a_value)
		SetSliderOptionValueST(a_value, "{2}s")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetHealthPhantomDuration(0.75)
		SetSliderOptionValueST(0.75, "{2}s")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetHealthPhantomDuration_InfoText")
	EndEvent
EndState


State WidgetDamageDuration_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetDamageDuration())
		SetSliderDialogDefaultValue(2.0)
		SetSliderDialogRange(0.01, 5)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetDamageDuration(a_value)
		SetSliderOptionValueST(a_value, "{2}s")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetDamageDuration(2.0)
		SetSliderOptionValueST(2.0, "{2}s")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetDamageDuration_InfoText")
	EndEvent
EndState


State WidgetReticleScale_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetReticleScale())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.1, 2)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetReticleScale(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetReticleScale(1)
		SetSliderOptionValueST(1, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetReticleScale_InfoText")
	EndEvent
EndState


Function UpdateReticleOpacityFlags()
	If (!TrueDirectionalMovement.GetWidgetUseHUDOpacityForReticle())
		_widgetReticleOpacityFlags = OPTION_FLAG_NONE
	Else
		_widgetReticleOpacityFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetReticleOpacityFlags, true, "WidgetReticleOpacity_S")
EndFunction


State WidgetUseHUDOpacityForReticle_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetUseHUDOpacityForReticle(!TrueDirectionalMovement.GetWidgetUseHUDOpacityForReticle())
		UpdateReticleOpacityFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetUseHUDOpacityForReticle())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetUseHUDOpacityForReticle(true)
		UpdateReticleOpacityFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetUseHUDOpacityForReticle_InfoText")
	EndEvent
EndState


State WidgetReticleOpacity_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetReticleOpacity())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.01, 1)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetReticleOpacity(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetReticleOpacity(1)
		SetSliderOptionValueST(1, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetReticleOpacity_InfoText")
	EndEvent
EndState


State WidgetTargetBarScale_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetTargetBarScale())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.1, 2)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetTargetBarScale(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetTargetBarScale(1)
		SetSliderOptionValueST(1, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetTargetBarScale_InfoText")
	EndEvent
EndState


Function UpdateTargetBarOpacityFlags()
	If (!TrueDirectionalMovement.GetWidgetUseHUDOpacityForTargetBar())
		_widgetTargetBarOpacityFlags = OPTION_FLAG_NONE
	Else
		_widgetTargetBarOpacityFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_widgetTargetBarOpacityFlags, true, "WidgetTargetBarOpacity_S")
EndFunction


State WidgetUseHUDOpacityForTargetBar_S
	Event OnSelectST()
		TrueDirectionalMovement.SetWidgetUseHUDOpacityForTargetBar(!TrueDirectionalMovement.GetWidgetUseHUDOpacityForTargetBar())
		UpdateTargetBarOpacityFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetWidgetUseHUDOpacityForTargetBar())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetUseHUDOpacityForTargetBar(true)
		UpdateTargetBarOpacityFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetUseHUDOpacityForTargetBar_InfoText")
	EndEvent
EndState


State WidgetTargetBarOpacity_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetWidgetTargetBarOpacity())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.01, 1)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetWidgetTargetBarOpacity(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetWidgetTargetBarOpacity(1)
		SetSliderOptionValueST(1, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_WidgetTargetBarOpacity_InfoText")
	EndEvent
EndState


Function UpdateBossBarFlags()
	If (TrueDirectionalMovement.GetShowBossBar())
		_bossBarFlags = OPTION_FLAG_NONE
		If (TrueDirectionalMovement.GetBossLevelMode() > 0)
			_bossLevelFlags = OPTION_FLAG_NONE
		Else
			_bossLevelFlags = OPTION_FLAG_DISABLED
		EndIf
		If (TrueDirectionalMovement.GetShowBossDamage())
			_bossDamageFlags = OPTION_FLAG_NONE
		Else
			_bossDamageFlags = OPTION_FLAG_DISABLED
		EndIf
		If (TrueDirectionalMovement.GetShowBossHealthPhantom())
			_bossHealthPhantomFlags = OPTION_FLAG_NONE
		Else
			_bossHealthPhantomFlags = OPTION_FLAG_DISABLED
		EndIf
		If (!TrueDirectionalMovement.GetUseHUDOpacityForBossBar())
			_bossBarOpacityFlags = OPTION_FLAG_NONE
		Else
			_bossBarOpacityFlags = OPTION_FLAG_DISABLED
		EndIf
	Else
		_bossBarFlags = OPTION_FLAG_DISABLED
		_bossLevelFlags = OPTION_FLAG_DISABLED
		_bossDamageFlags = OPTION_FLAG_DISABLED
		_bossHealthPhantomFlags = OPTION_FLAG_DISABLED
		_bossBarOpacityFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_bossBarFlags, true, "BossNameAlignment_S")
	SetOptionFlagsST(_bossBarFlags, true, "BossLevelMode_S")
	SetOptionFlagsST(_bossLevelFlags, true, "BossLevelThreshold_S")
	SetOptionFlagsST(_bossBarFlags, true, "ShowBossDamage_S")
	SetOptionFlagsST(_bossBarFlags, true, "ShowBossHealthPhantom_S")
	SetOptionFlagsST(_bossHealthPhantomFlags, true, "BossHealthPhantomDuration_S")
	SetOptionFlagsST(_bossDamageFlags, true, "BossDamageDuration_S")
	SetOptionFlagsST(_bossBarFlags, true, "BossBarScale_S")
	SetOptionFlagsST(_bossBarFlags, true, "BossBarX_S")
	SetOptionFlagsST(_bossBarFlags, true, "BossBarY_S")
	SetOptionFlagsST(_bossBarFlags, true, "BossBarOffsetSubtitles_S")
	SetOptionFlagsST(_bossBarFlags, true, "MultipleBossBarsOffset_S")
	SetOptionFlagsST(_bossBarFlags, true, "MultipleBossBarsStackUpwards_S")
	SetOptionFlagsST(_bossBarFlags, true, "UseHUDOpacityForBossBar_S")
	SetOptionFlagsST(_bossBarOpacityFlags, true, "BossBarOpacity_S")
EndFunction


State ShowBossBar_S
	Event OnSelectST()
		TrueDirectionalMovement.SetShowBossBar(!TrueDirectionalMovement.GetShowBossBar())
		UpdateBossBarFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetShowBossBar())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetShowBossBar(true)
		UpdateBossBarFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_ShowBossBar_InfoText")
	EndEvent
EndState


State BossNameAlignment_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetBossNameAlignment())
		SetMenuDialogDefaultIndex(0)
		SetMenuDialogOptions(_nameAlignmentNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetBossNameAlignment(a_index)
		SetMenuOptionValueST(_nameAlignmentNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossNameAlignment(0)
		SetMenuOptionValueST(_nameAlignmentNames[0])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossNameAlignment_InfoText")
	EndEvent
EndState


Function UpdateBossLevelFlags()
	If (TrueDirectionalMovement.GetBossLevelMode() > 0)
		_bossLevelFlags = OPTION_FLAG_NONE
	Else
		_bossLevelFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_bossLevelFlags, true, "BossLevelThreshold_S")
EndFunction


State BossLevelMode_S
	Event OnMenuOpenST()
		SetMenuDialogStartIndex(TrueDirectionalMovement.GetBossLevelMode())
		SetMenuDialogDefaultIndex(1)
		SetMenuDialogOptions(_bossBarLevelModeNames)
	EndEvent

	Event OnMenuAcceptST(Int a_index)
		TrueDirectionalMovement.SetBossLevelMode(a_index)
		UpdateBossLevelFlags()		
		SetMenuOptionValueST(_bossBarLevelModeNames[a_index])
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossLevelMode(1)
		UpdateBossLevelFlags()
		SetMenuOptionValueST(_bossBarLevelModeNames[1])
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossLevelMode_InfoText")
	EndEvent
EndState


State BossLevelThreshold_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossLevelThreshold())
		SetSliderDialogDefaultValue(10)
		SetSliderDialogRange(0, 50)
		SetSliderDialogInterval(1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossLevelThreshold(a_value as Int)
		SetSliderOptionValueST(a_value, "{0}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossLevelThreshold(10)
		SetSliderOptionValueST(10, "{0}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossLevelThreshold_InfoText")
	EndEvent
EndState


Function UpdateBossDamageFlags()
	If (TrueDirectionalMovement.GetShowBossDamage())
		_bossDamageFlags = OPTION_FLAG_NONE
	Else
		_bossDamageFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_bossDamageFlags, true, "BossDamageDuration_S")
EndFunction


State ShowBossDamage_S
	Event OnSelectST()
		TrueDirectionalMovement.SetShowBossDamage(!TrueDirectionalMovement.GetShowBossDamage())
		UpdateBossDamageFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetShowBossDamage())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetShowBossDamage(true)
		UpdateBossDamageFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_ShowBossDamage_InfoText")
	EndEvent
EndState


Function UpdateBossHealthPhantomFlags()
	If (TrueDirectionalMovement.GetShowBossHealthPhantom())
		_bossHealthPhantomFlags = OPTION_FLAG_NONE
	Else
		_bossHealthPhantomFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_bossHealthPhantomFlags, true, "BossHealthPhantomDuration_S")
EndFunction


State ShowBossHealthPhantom_S
	Event OnSelectST()
		TrueDirectionalMovement.SetShowBossHealthPhantom(!TrueDirectionalMovement.GetShowBossHealthPhantom())
		UpdateBossHealthPhantomFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetShowBossHealthPhantom())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetShowBossHealthPhantom(true)
		UpdateBossHealthPhantomFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_ShowBossHealthPhantom_InfoText")
	EndEvent
EndState


State BossHealthPhantomDuration_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossHealthPhantomDuration())
		SetSliderDialogDefaultValue(0.75)
		SetSliderDialogRange(0.01, 5)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossHealthPhantomDuration(a_value)
		SetSliderOptionValueST(a_value, "{2}s")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossHealthPhantomDuration(0.75)
		SetSliderOptionValueST(0.75, "{2}s")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossHealthPhantomDuration_InfoText")
	EndEvent
EndState


State BossDamageDuration_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossDamageDuration())
		SetSliderDialogDefaultValue(2.0)
		SetSliderDialogRange(0.01, 5)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossDamageDuration(a_value)
		SetSliderOptionValueST(a_value, "{2}s")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossDamageDuration(2.0)
		SetSliderOptionValueST(2.0, "{2}s")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossDamageDuration_InfoText")
	EndEvent
EndState


State BossBarScale_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossBarScale())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.1, 2)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossBarScale(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossBarScale(1)
		SetSliderOptionValueST(1, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossBarScale_InfoText")
	EndEvent
EndState


Function UpdateBossBarOpacityFlags()
	If (!TrueDirectionalMovement.GetUseHUDOpacityForBossBar())
		_bossBarOpacityFlags = OPTION_FLAG_NONE
	Else
		_bossBarOpacityFlags = OPTION_FLAG_DISABLED
	EndIf
	SetOptionFlagsST(_bossBarOpacityFlags, true, "BossBarOpacity_S")
EndFunction


State UseHUDOpacityForBossBar_S
	Event OnSelectST()
		TrueDirectionalMovement.SetUseHUDOpacityForBossBar(!TrueDirectionalMovement.GetUseHUDOpacityForBossBar())
		UpdateBossBarOpacityFlags()
		SetToggleOptionValueST(TrueDirectionalMovement.GetUseHUDOpacityForBossBar())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetUseHUDOpacityForBossBar(true)
		UpdateBossBarOpacityFlags()
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_UseHUDOpacityForBossBar_InfoText")
	EndEvent
EndState


State BossBarOpacity_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossBarOpacity())
		SetSliderDialogDefaultValue(1.0)
		SetSliderDialogRange(0.01, 1)
		SetSliderDialogInterval(0.01)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossBarOpacity(a_value)
		SetSliderOptionValueST(a_value, "{2}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossBarOpacity(1)
		SetSliderOptionValueST(1, "{2}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossBarOpacity_InfoText")
	EndEvent
EndState


State BossBarX_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossBarX())
		SetSliderDialogDefaultValue(0.5)
		SetSliderDialogRange(0.001, 1)
		SetSliderDialogInterval(0.001)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossBarX(a_value)
		SetSliderOptionValueST(a_value, "{3}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossBarX(0.5)
		SetSliderOptionValueST(0.5, "{3}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossBarX_InfoText")
	EndEvent
EndState


State BossBarY_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetBossBarY())
		SetSliderDialogDefaultValue(0.87)
		SetSliderDialogRange(0.001, 1)
		SetSliderDialogInterval(0.001)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetBossBarY(a_value)
		SetSliderOptionValueST(a_value, "{3}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossBarY(0.87)
		SetSliderOptionValueST(0.87, "{3}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossBarY_InfoText")
	EndEvent
EndState


State BossBarOffsetSubtitles_S
	Event OnSelectST()
		TrueDirectionalMovement.SetBossBarOffsetSubtitles(!TrueDirectionalMovement.GetBossBarOffsetSubtitles())
		SetToggleOptionValueST(TrueDirectionalMovement.GetBossBarOffsetSubtitles())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetBossBarOffsetSubtitles(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_BossBarOffsetSubtitles_InfoText")
	EndEvent
EndState


State MultipleBossBarsOffset_S
	Event OnSliderOpenST()
		SetSliderDialogStartValue(TrueDirectionalMovement.GetMultipleBossBarsOffset())
		SetSliderDialogDefaultValue(45)
		SetSliderDialogRange(1, 100)
		SetSliderDialogInterval(0.1)
	EndEvent

	Event OnSliderAcceptST(Float a_value)
		TrueDirectionalMovement.SetMultipleBossBarsOffset(a_value)
		SetSliderOptionValueST(a_value, "{1}")
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetMultipleBossBarsOffset(45)
		SetSliderOptionValueST(45, "{1}")
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_MultipleBossBarsOffset_InfoText")
	EndEvent
EndState


State MultipleBossBarsStackUpwards_S
	Event OnSelectST()
		TrueDirectionalMovement.SetMultipleBossBarsStackUpwards(!TrueDirectionalMovement.GetMultipleBossBarsStackUpwards())
		SetToggleOptionValueST(TrueDirectionalMovement.GetMultipleBossBarsStackUpwards())
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetMultipleBossBarsStackUpwards(true)
		SetToggleOptionValueST(true)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_MultipleBossBarsStackUpwards_InfoText")
	EndEvent
EndState


State TargetLockKey_K
	Event OnKeyMapChangeST(Int a_newKeyCode, String a_conflictControl, String a_conflictName)
		If (HandleKeyConflict(a_conflictControl, a_conflictName))
			TrueDirectionalMovement.SetTargetLockKey(a_newKeyCode)
			SetKeymapOptionValueST(a_newKeyCode)
		EndIf
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetTargetLockKey(-1)
		SetKeymapOptionValueST(-1)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_TargetLockKey_InfoText")
	EndEvent
EndState


State SwitchTargetLeftKey_K
	Event OnKeyMapChangeST(Int a_newKeyCode, String a_conflictControl, String a_conflictName)
		If (HandleKeyConflict(a_conflictControl, a_conflictName))
			TrueDirectionalMovement.SetSwitchTargetLeftKey(a_newKeyCode)
			SetKeymapOptionValueST(a_newKeyCode)
		EndIf
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetSwitchTargetLeftKey(-1)
		SetKeymapOptionValueST(-1)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_SwitchTargetLeftKey_InfoText")
	EndEvent
EndState


State SwitchTargetRightKey_K
	Event OnKeyMapChangeST(Int a_newKeyCode, String a_conflictControl, String a_conflictName)
		If (HandleKeyConflict(a_conflictControl, a_conflictName))
			TrueDirectionalMovement.SetSwitchTargetRightKey(a_newKeyCode)
			SetKeymapOptionValueST(a_newKeyCode)
		EndIf
	EndEvent

	Event OnDefaultST()
		TrueDirectionalMovement.SetSwitchTargetRightKey(-1)
		SetKeymapOptionValueST(-1)
	EndEvent

	Event OnHighlightST()
		SetInfoText("$TrueDirectionalMovement_SwitchTargetRightKey_InfoText")
	EndEvent
EndState


Bool Function HandleKeyConflict(String a_conflictControl, String a_conflictName)
	If (a_conflictControl != "")
		String msg
		If (a_conflictName != "")
			msg = "$TrueDirectionalMovement_KeyConflict_Control{" + a_conflictControl + "}_Name{" + a_conflictName + "}"
		Else
			msg = "$TrueDirectionalMovement_KeyConflict_Control{" + a_conflictControl + "}"
		EndIf
		Return ShowMessage(msg, True, "$Yes", "$No")
	EndIf
	Return True
EndFunction
