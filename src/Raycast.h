#pragma once

#include "Offsets.h"

namespace Raycast
{
	struct RayHitCollector : public RE::hkpRayHitCollector
	{
	public:
		RayHitCollector();

		virtual void AddRayHit(const RE::hkpCdBody& cdBody, const RE::hkpShapeRayCastCollectorOutput& hitInfo) override;

		void Reset();

		RE::hkpShapeRayCastCollectorOutput closestHitInfo;
		bool doesHitExist = false;
	};

	/*struct AllRayHitCollector : public RE::hkpRayHitCollector
	{
	public:
		AllRayHitCollector();

		virtual void AddRayHit(const RE::hkpCdBody& cdBody, const RE::hkpShapeRayCastCollectorOutput& hitInfo) override;

		inline void Reset();

		std::vector<std::pair<RE::hkpCdBody*, RE::hkpShapeRayCastCollectorOutput>> hits;
	};*/
}
