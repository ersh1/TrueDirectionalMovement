#include "Utils.h"

void GetAngle(const RE::NiPoint3& a_from, const RE::NiPoint3& a_to, AngleZX& angle)
{
	const auto x = a_to.x - a_from.x;
	const auto y = a_to.y - a_from.y;
	const auto z = a_to.z - a_from.z;
	const auto xy = sqrt(x * x + y * y);

	angle.z = atan2(x, y);
	angle.x = atan2(-z, xy);
	angle.distance = sqrt(xy * xy + z * z);
}

bool GetAngle(RE::TESObjectREFR* a_target, AngleZX& angle)
{
	if (!a_target)
	{
		return false;
	}
	RE::NiPoint3 targetPos = a_target->GetPosition();
	RE::NiPoint3 cameraPos = GetCameraPos();

	GetAngle(cameraPos, targetPos, angle);

	return true;
}

RE::NiPoint3 GetCameraPos()
{
	auto player = RE::PlayerCharacter::GetSingleton();
	auto playerCamera = RE::PlayerCamera::GetSingleton();
	RE::NiPoint3 ret;

	if (playerCamera->currentState == playerCamera->cameraStates[RE::CameraStates::kFirstPerson] || 
		playerCamera->currentState == playerCamera->cameraStates[RE::CameraStates::kThirdPerson] ||
		playerCamera->currentState == playerCamera->cameraStates[RE::CameraStates::kMount]) 
	{
		RE::NiNode* root = playerCamera->cameraRoot.get();
		if (root) 
		{
			ret.x = root->world.translate.x;
			ret.y = root->world.translate.y;
			ret.z = root->world.translate.z;
		}
	} 
	else 
	{
		RE::NiPoint3 playerPos = player->GetLookingAtLocation();

		ret.z = playerPos.z;
		ret.x = player->GetPositionX();
		ret.y = player->GetPositionY();
	}

	return ret;
}

float NormalAbsoluteAngle(float a_angle)
{
	while (a_angle < 0)
		a_angle += TWO_PI;
	while (a_angle > TWO_PI)
		a_angle -= TWO_PI;
	return a_angle;

	//return fmod(a_angle, TWO_PI) >= 0 ? a_angle : (a_angle + TWO_PI);
}

float NormalRelativeAngle(float a_angle)
{
	while (a_angle > PI)
		a_angle -= TWO_PI;
	while (a_angle < -PI)
		a_angle += TWO_PI;
	return a_angle;

	//return fmod(a_angle, TWO_PI) >= 0 ? (a_angle < PI) ? a_angle : a_angle - TWO_PI : (a_angle >= -PI) ? a_angle : a_angle + TWO_PI;
}

// get world coordinates of nodeName for actor
static bool GetNodePosition(RE::ActorPtr a_actor, const char* a_nodeName, RE::NiPoint3& point)
{
	bool bResult = false;

	if (a_nodeName[0]) {
		RE::NiAVObject* object = a_actor->Get3D2();
		if (object)
		{
			object = object->GetObjectByName(a_nodeName);
			if (object) 
			{
				point.x = object->world.translate.x;
				point.y = object->world.translate.y;
				point.z = object->world.translate.z;
				bResult = true;
			}
		}
	}

	return bResult;
}

// acquire actor's torso position
static bool GetTorsoPos(RE::ActorPtr a_actor, RE::NiPoint3& point)
{
	RE::TESRace* race = a_actor->race;
	RE::BGSBodyPartData* bodyPartData = race->bodyPartData;

	// bodyPart->part[0] body
	// bodyPart->part[1] head
	RE::BGSBodyPart* bodyPart = bodyPartData->parts[0];
	if (bodyPart) 
	{
		return GetNodePosition(a_actor, bodyPart->targetName.c_str(), point);
	}

	return false;
}

bool GetTargetPos(RE::ObjectRefHandle a_target, RE::NiPoint3& pos, bool bGetTorsoPos /*= true*/)
{
	auto target = a_target.get();
	if (!target) {
		return false;
	}

	if (target->Get3D2() == nullptr) {
		return false;
	}

	if (target->formType == RE::FormType::ActorCharacter) 
	{
		auto actorPtr = RE::ActorPtr(target->As<RE::Actor>());
		if (!bGetTorsoPos || !GetTorsoPos(actorPtr, pos))
		{
			pos = target->GetLookingAtLocation();
		}
	} 
	else
	{
		pos = target->GetPosition();
	}

	return true;
}

void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb)
{
	float cb = std::sqrtf(1 - sb * sb);
	float ca = cacb / cb;
	float sa = sacb / cb;
	a_matrix.entry[0][0] = ca;
	a_matrix.entry[0][1] = -sacb;
	a_matrix.entry[0][2] = sa * sb;
	a_matrix.entry[1][0] = sa;
	a_matrix.entry[1][1] = cacb;
	a_matrix.entry[1][2] = -ca * sb;
	a_matrix.entry[2][0] = 0.0;
	a_matrix.entry[2][1] = sb;
	a_matrix.entry[2][2] = cb;
}

bool PredictAimProjectile(RE::NiPoint3 a_projectilePos, RE::NiPoint3 a_targetPosition, RE::NiPoint3 a_targetVelocity, float a_gravity, RE::NiPoint3& a_projectileVelocity)
{
	// http://ringofblades.com/Blades/Code/PredictiveAim.cs

	float projectileSpeedSquared = a_projectileVelocity.SqrLength();
	float projectileSpeed = std::sqrtf(projectileSpeedSquared);

	if (projectileSpeed <= 0.f || a_projectilePos == a_targetPosition) {
		return false;
	}

	float targetSpeedSquared = a_targetVelocity.SqrLength();
	float targetSpeed = std::sqrtf(targetSpeedSquared);
	RE::NiPoint3 targetToProjectile = a_projectilePos - a_targetPosition;
	float distanceSquared = targetToProjectile.SqrLength();
	float distance = std::sqrtf(distanceSquared);
	RE::NiPoint3 direction = targetToProjectile;
	direction.Unitize();
	RE::NiPoint3 targetVelocityDirection = a_targetVelocity;
	targetVelocityDirection.Unitize();

	float cosTheta = (targetSpeedSquared > 0) 
		? direction.Dot(targetVelocityDirection) 
		: 1.0f;

	bool bValidSolutionFound = true;
	float t;

	if (ApproximatelyEqual(projectileSpeedSquared, targetSpeedSquared)) {
		// We want to avoid div/0 that can result from target and projectile traveling at the same speed
		//We know that cos(theta) of zero or less means there is no solution, since that would mean B goes backwards or leads to div/0 (infinity)
		if (cosTheta > 0) {
			t = 0.5f * distance / (targetSpeed * cosTheta);
		} else {
			bValidSolutionFound = false;
			t = 1;
		}
	} else {
		float a = projectileSpeedSquared - targetSpeedSquared;
		float b = 2.0f * distance * targetSpeed * cosTheta;
		float c = -distanceSquared;
		float discriminant = b * b - 4.0f * a * c;

		if (discriminant < 0) {
			// NaN
			bValidSolutionFound = false;
			t = 1;
		} else {
			// a will never be zero
			float uglyNumber = sqrtf(discriminant);
			float t0 = 0.5f * (-b + uglyNumber) / a;
			float t1 = 0.5f * (-b - uglyNumber) / a;

			// Assign the lowest positive time to t to aim at the earliest hit
			t = min(t0, t1);
			if (t < FLT_EPSILON) {
				t = max(t0, t1);
			}

			if (t < FLT_EPSILON) {
				// Time can't flow backwards when it comes to aiming.
				// No real solution was found, take a wild shot at the target's future location
				bValidSolutionFound = false;
				t = 1;
			}
		}
	}

	a_projectileVelocity = a_targetVelocity + (-targetToProjectile / t);

	if (!bValidSolutionFound)
	{
		a_projectileVelocity.Unitize();
		a_projectileVelocity *= projectileSpeed;
	}

	if (!ApproximatelyEqual(a_gravity, 0.f))
	{
		float netFallDistance = (a_projectileVelocity * t).z;
		float gravityCompensationSpeed = (netFallDistance + 0.5f * a_gravity * t * t) / t;
		a_projectileVelocity.z = gravityCompensationSpeed;
	}

	return bValidSolutionFound;
}
