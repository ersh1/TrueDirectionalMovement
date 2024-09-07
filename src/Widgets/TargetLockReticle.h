#pragma once
#include "API/TrueHUDAPI.h"
#include "Settings.h"

namespace Scaleform
{
	class TargetLockReticle : public TRUEHUD_API::WidgetBase
	{
	public:
		enum WidgetStateMode : std::uint8_t
		{
			kAdd = 0,
			kRemove = 3,
		};

		enum InterpMode : std::uint8_t
		{
			kNone,
			kCrosshairToTarget,
			kTargetToTarget,
			kTargetToCrosshair
		};

		TargetLockReticle(uint32_t a_widgetID, RE::ObjectRefHandle a_refHandle, RE::NiPointer<RE::NiAVObject> a_targetPoint) :
			WidgetBase(a_widgetID),
			_refHandle(a_refHandle),
			_targetPoint(a_targetPoint)
		{}

		TargetLockReticle(uint32_t a_widgetID, RE::ObjectRefHandle a_refHandle, RE::NiPointer<RE::NiAVObject> a_targetPoint, ReticleStyle a_reticleStyle) :
			WidgetBase(a_widgetID),
			_refHandle(a_refHandle),
			_targetPoint(a_targetPoint),
			_reticleStyle(a_reticleStyle)
		{}

		virtual void Update(float a_deltaTime) override;
		virtual void Initialize() override;
		virtual void Dispose() override;

		virtual void SetWidgetState(WidgetState a_widgetState);
		virtual void ChangeTarget(RE::ObjectRefHandle a_refHandle, RE::NiPointer<RE::NiAVObject> a_targetPoint);

		RE::ObjectRefHandle _refHandle;
		RE::NiPointer<RE::NiAVObject> _targetPoint;
		ReticleStyle _reticleStyle;

	protected:
		virtual void UpdatePosition();
		virtual void UpdateInfo();
		virtual void LoadConfig();
		virtual void StartInterpolation(InterpMode a_interpMode);

	private:
		float _interpTimer = 0.f;
		float _interpDuration = 0.f;
		float _interpAlpha = 0.f;

		RE::NiPoint2 _lastScreenPos;
		RE::NiPoint2 _desiredScreenPos;

		InterpMode _interpMode = kNone;

		static constexpr float _fullInterpDuration = 0.5f;
	};
}
