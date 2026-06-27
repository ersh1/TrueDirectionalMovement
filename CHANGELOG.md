Changelog:

Enable TargetLock when mounted on a dragon:
------------------------------------------

  Hooks.cpp and Hooks.h
  * New class DragonCameraState, same functions and members as HorseCameraState, but for the dragon camerastate
  * New hook DragonCameraStateHook (same functions as HorseCameraStateHook, but hooking into the DragonCameraState)
  * Renamed SaveCamera::RotationType::kHorse to SaveCamera::RotationType::kMount to reflect both horse and dragon mounts (no functional changes)

  DirectionalMovementHandler.cpp and Hooks.cpp
  * Added checks for RE::CameraStates::kDragon whererever there are checks for RE::CameraState::kMount
  * Use DragonCameraState instead of HorseCameraState in case camera state is RE::CameraState::kDragon

  DirectionalMovementHandler.cpp
  * modified handling of camera snap in ToggleTargetLock() and LookAtTarget() such that DragonCameraState->dragonRefHandle.get()->As<RE::Actor>()->data.angle.x is not modified.
  ( otherwise if target lock is on, the dragon's orientation flickers between two positions while the dragon is switching flying states (flying->hover, or hover->land)
  * In ToggleTargetLock(), when toggling the TargetLock on, the target lock is set to the dragon's current combat target instead of using the actor which is closest to the center of the screen
  (only in case IDRC is active and the dragon is mounted and in combat). This functionality uses IDRC's API function APIs::IDRC->GetCurrentTarget(). It is used if APIs::IDRC->UseTarget() returns true.
  * In LookAtTarget(): 
    - use dragon instead of player as reference because player's yaw is changing with dragon's head orientation.

    - enhanced the handling of the situation when the target is behind the camera (ie camera is between player and target):
      - added distance check to bIsBehind to avoid that the camera is rotating towards the player-axis in case the target is further away from the player as the camera
    (in that case, the target ended up behind the camera).  
      - The distance check also addresses this case: If the target is close behind the camera there was a tendency for camera oscillation between two positions close to the player-target axis. Reason is that projectedDirectionToTargetXY changes significantly through the camera movement because the distance btw camera and target is short. That in turn leads to angleDelta values switching sign between frames.

  * IDRC API is added via API/IDRC_API.h, and connected in API/APIManager.cpp and API/APIManager.h 
      * IDRC API requires unreleased version of IDRC (https://github.com/staalo18/IntuitiveDragonRideControl)


Changed handling of reference pitch for horseCameraState and dragonCameraState:
-------------------------------------------------------------------------------
  * DirectionalMovementHandler.cpp: in LookAtTarget(), use referencePitch = 0 for horse and dragon camera states, instead of _desiredPlayerPitch.


New option: TargetLock - Min height above ground:
-------------------------------------------------
  * Introduced functionality to keep the camera above mininmal height over ground. Two new functions:
    * DirectionalMovementHandler.cpp: GetCameraAngle() -  Checks for ground level and adapts angle if camera would move below min height.
    *  Utils.cpp: GetLandHeightWithWater() - provides z-coord of land height, considering water level.
  *  New MCM option  (in TargetLock category): "Min Camera Height Above Ground", with default 35. This required changes in:
      MCM/Config/TrueDirectionalMovement/config.json
      MCM/Config/TrueDirectionalMovement/settings.ini
      Translation/TrueDirectionalMovement_english.txt
      Settings.cpp
      Settings.h
  * In Settings.h: bTargetLockConsiderGroundLevel (true) - switch to turn off Min-height-above-ground functionality

New option: TargetLock - Lock behind target:
--------------------------------------------
  * DirectionalMovementHandler:
    * new functions:
        EnableLockBehindTarget)
        ToggleLockBehindTarget()
        GetNominalCameraToPlayerDistance()
        GetNominalCameraPosition()
        UpdateMoveCameraBehindTarget()
    * LookAtTarget(): Modifications to compute yaw and pitch for the "lock-behind-target" case (probably more bad math ahead ...). 
  * Utils.cpp: new function GetFlyingState()
  * Settings.h: fCameraBehindTargetMinDistance, fCameraBehindTargetNoSwitchRange (not in MCM)
  * New MCM options (in TargetLock category): "Enable Lock Behind Target" (default: off), and "Toggle Lock-Behind-Target Key" (default: undefined). This required changes in:
      MCM/Config/TrueDirectionalMovement/config.json
      MCM/Config/TrueDirectionalMovement/settings.ini
      Translation/TrueDirectionalMovement_english.txt
      Settings.cpp
      Settings.h
      Events.cpp (in InputEventHandler::ProcessEvent() - check for Toggle Lock-Behind-Target Key)
