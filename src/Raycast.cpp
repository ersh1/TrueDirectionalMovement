#include "Raycast.h"
#include "Offsets.h"

namespace Raycast
{
	RayHitCollector::RayHitCollector()
	{
		Reset();
	}	

	void RayHitCollector::AddRayHit([[maybe_unused]] const RE::hkpCdBody& cdBody, const RE::hkpShapeRayCastCollectorOutput& hitInfo)
	{
		// Note: for optimization purposes this should set the m_earlyOutHitFraction to:
		// - 0.0 if you want to get no more hits
		// - 1.0 if you want to get all hits (constructor initializes this value to 1.0 by default)
		// - output.m_hitFraction if you only want to get closer hits than one just found

		//while (cdBody->m_parent) {
		//	cdBody = cdBody->m_parent;
		//}
		//_MESSAGE("Raycast hit: %x", cdBody->m_shape ? cdBody->m_shape->m_type : HK_SHAPE_INVALID);

		//m_closestCollidable = cdBody;
		closestHitInfo = hitInfo;
		doesHitExist = true;
		earlyOutHitFraction = hitInfo.hitFraction;  // Only accept closer hits after this
	}

	void RayHitCollector::Reset()
	{
		earlyOutHitFraction = 1.0f;
		doesHitExist = false;
	}

	//AllRayHitCollector::AllRayHitCollector()
	//{
	//	Reset();
	//}

	//void AllRayHitCollector::AddRayHit(const RE::hkpCdBody& cdBody, [[maybe_unused]] const RE::hkpShapeRayCastCollectorOutput& hitInfo)
	//{
	//	// Note: for optimization purposes this should set the m_earlyOutHitFraction to:
	//	// - 0.0 if you want to get no more hits
	//	// - 1.0 if you want to get all hits (constructor initializes this value to 1.0 by default)
	//	// - output.m_hitFraction if you only want to get closer hits than one just found

	//	//while (cdBody->m_parent) {
	//	//	cdBody = cdBody->m_parent;
	//	//}
	//	//_MESSAGE("Raycast hit: %x", cdBody->m_shape ? cdBody->m_shape->m_type : HK_SHAPE_INVALID);

	//	//m_closestCollidable = cdBody;

	//	RE::hkpCdBody* body = const_cast<RE::hkpCdBody*>(&cdBody);
	//	while (body->parent) {
	//		body = const_cast<RE::hkpCdBody*>(body->parent);
	//	}

	//	hits.push_back(std::make_pair(body, hitInfo));
	//	//m_earlyOutHitFraction = hitInfo->m_hitFraction; // Only accept closer hits after this
	//}

	//void AllRayHitCollector::Reset()
	//{
	//	earlyOutDistance = 1.f;
	//	hits.clear();
	//}
}

