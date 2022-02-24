#include "Widgets/TargetLockReticle.h"

#include "Offsets.h"
#include "Settings.h"
#include "Utils.h"
#include "DirectionalMovementHandler.h"

namespace Scaleform
{
	void TargetLockReticle::Update(float a_deltaTime)
	{
		if (_interpTimer > 0) {
			_interpTimer -= a_deltaTime;

			if (_interpTimer < 0) {
				_interpTimer = 0;
			}

			_interpAlpha = 1.f - _interpTimer / _interpDuration;
		}

		if (_interpTimer == 0.f && _widgetState != WidgetState::kPendingRemoval) {
			_interpMode = InterpMode::kNone;
		}

		RE::GFxValue result;
		_object.Invoke("isReadyToRemove", &result);
		if (result.GetBool()) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		UpdatePosition();
		UpdateInfo();
	}

	void TargetLockReticle::Initialize()
	{
		LoadConfig();

		SetWidgetState(WidgetState::kActive);

		RE::GRectF rect = _view->GetVisibleFrameRect();
		_lastScreenPos.x = rect.right * 0.5f;
		_lastScreenPos.y = rect.bottom * 0.5f;
	}

	void TargetLockReticle::Dispose()
	{
		_object.Invoke("cleanUp", nullptr, nullptr, 0);
		DirectionalMovementHandler::GetSingleton()->ReticleRemoved();
	}

	void TargetLockReticle::SetWidgetState(WidgetState a_widgetState)
	{
		_widgetState = a_widgetState;

		// if the reticle is 'transforming' from vanilla crosshair, do the position interpolation
		if (_reticleStyle == ReticleStyle::kCrosshair) {
			if (_widgetState == WidgetState::kActive) {
				StartInterpolation(InterpMode::kCrosshairToTarget);
			} else if (_widgetState == WidgetState::kPendingRemoval) {
				StartInterpolation(InterpMode::kTargetToCrosshair);
			}
		}
	}

	void TargetLockReticle::ChangeTarget(RE::ObjectRefHandle a_refHandle, RE::NiPointer<RE::NiAVObject> a_targetPoint)
	{
		AddWidgetTask([=]() {
			_refHandle = a_refHandle;
			_targetPoint = a_targetPoint;	

			if (_widgetState >= WidgetState::kPendingRemoval) {
				SetWidgetState(WidgetState::kActive);

				RE::GFxValue arg;
				arg.SetBoolean(false);
				_object.Invoke("setReadyToRemove", nullptr, &arg, 1);
			} else {
				StartInterpolation(InterpMode::kTargetToTarget);
				_object.Invoke("playChangeTargetTimeline", nullptr, nullptr, 0);
			}
		});
	}

	void TargetLockReticle::UpdatePosition()
	{
		if (!_refHandle) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		RE::GRectF rect = _view->GetVisibleFrameRect();

		if (_interpMode == InterpMode::kTargetToCrosshair) {
			_desiredScreenPos.x = rect.right * 0.5f;
			_desiredScreenPos.y = rect.bottom * 0.5f;
		} else {
			RE::NiPoint3 targetWorldPos = _targetPoint ? _targetPoint->world.translate : _refHandle.get()->GetLookingAtLocation();
			//GetTargetPos(_refHandle, targetWorldPos, Settings::uReticleAnchor == WidgetAnchor::kBody);
			//GetTargetPointPosition(_refHandle, _targetPoint, targetWorldPos);

			RE::NiCamera::WorldPtToScreenPt3((float(*)[4])g_worldToCamMatrix, *g_viewPort, targetWorldPos, _desiredScreenPos.x, _desiredScreenPos.y, _depth, 1e-5f);

			_desiredScreenPos.y = 1.0f - _desiredScreenPos.y;  // Flip y for Flash coordinate system
			_desiredScreenPos.y = rect.top + (rect.bottom - rect.top) * _desiredScreenPos.y;
			_desiredScreenPos.x = rect.left + (rect.right - rect.left) * _desiredScreenPos.x;
		}

		RE::NiPoint2 screenPos;

		// if we're interpolating, lerp between the positions
		if (_interpTimer > 0) {
			screenPos.x = InterpEaseIn(_lastScreenPos.x, _desiredScreenPos.x, _interpAlpha, 2);
			screenPos.y = InterpEaseIn(_lastScreenPos.y, _desiredScreenPos.y, _interpAlpha, 2);
		} else {
			screenPos = _desiredScreenPos;
		}

		//if (_interpTimer <= 0.f) {
		//	_lastScreenPos = screenPos;
		//}

		float scale = 100.f * Settings::fReticleScale;

		RE::GFxValue::DisplayInfo displayInfo;
		displayInfo.SetPosition(screenPos.x, screenPos.y);
		displayInfo.SetScale(scale, scale);
		_object.SetDisplayInfo(displayInfo);
	}

	void TargetLockReticle::UpdateInfo()
	{
		if (!_refHandle || !_refHandle.get()) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		auto actor = _refHandle.get()->As<RE::Actor>();
		if (!actor) {
			_widgetState = WidgetState::kRemoved;
			return;
		}

		RE::GFxValue arg;
		arg.SetBoolean(_widgetState == kPendingRemoval ? true : false);

		_object.Invoke("updateData", nullptr, &arg, 1);
	}

	void TargetLockReticle::LoadConfig()
	{
		RE::GFxValue args[2];
		args[0].SetNumber(static_cast<uint32_t>(_reticleStyle));
		args[1].SetNumber((Settings::bReticleUseHUDOpacity ? *g_fHUDOpacity : Settings::fReticleOpacity) * 100.f);
		_object.Invoke("loadConfig", nullptr, args, 2);
	}

	void TargetLockReticle::StartInterpolation(InterpMode a_interpMode)
	{
		//if (_interpMode == InterpMode::kNone) {
		//	RE::GRectF rect = view->GetVisibleFrameRect();
		//	_lastScreenPos.x = rect.right * 0.5f;
		//	_lastScreenPos.y = rect.bottom * 0.5f;
		//} else {
		//	RE::GFxValue::DisplayInfo displayInfo;
		//	object.GetDisplayInfo(&displayInfo);
		//	_lastScreenPos.x = static_cast<float>(displayInfo.GetX());
		//	_lastScreenPos.y = static_cast<float>(displayInfo.GetY());
		//}

		RE::GFxValue::DisplayInfo displayInfo;
		_object.GetDisplayInfo(&displayInfo);
		_lastScreenPos.x = static_cast<float>(displayInfo.GetX());
		_lastScreenPos.y = static_cast<float>(displayInfo.GetY());

		if (_interpMode == InterpMode::kCrosshairToTarget && a_interpMode == InterpMode::kTargetToCrosshair ||
			_interpMode == InterpMode::kTargetToCrosshair && a_interpMode == InterpMode::kCrosshairToTarget) {
			_interpDuration = _fullInterpDuration * _interpAlpha;
		} else {
			_interpDuration = _fullInterpDuration;
		}

		_interpTimer = _interpDuration;
		_interpMode = a_interpMode;
	}
}
