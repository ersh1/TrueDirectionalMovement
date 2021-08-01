#pragma once

#define PI 3.14159265f
#define TWOTHIRDS_PI 2.09439510f
#define TWO_PI 6.28318531f
#define PI2 1.57079633f
#define PI3 1.04719755f
#define PI4 0.78539816f

struct AngleZX
{
	double z;
	double x;
	double distance;
};

void GetAngle(const RE::NiPoint3& a_from, const RE::NiPoint3& a_to, AngleZX& angle);
bool GetAngle(RE::TESObjectREFR* a_target, AngleZX& angle);
RE::NiPoint3 GetCameraPos();
float NormalAbsoluteAngle(float a_angle);
float NormalRelativeAngle(float a_angle);
bool GetTargetPos(RE::ObjectRefHandle a_target, RE::NiPoint3& pos);
void SetRotationMatrix(RE::NiMatrix3& a_matrix, float sacb, float cacb, float sb);
bool PredictAimProjectile(RE::NiPoint3 a_projectilePos, RE::NiPoint3 a_targetPosition, RE::NiPoint3 a_targetVelocity, float a_gravity, RE::NiPoint3& a_projectileVelocity);

inline bool ApproximatelyEqual(float A, float B)
{
	return ((A - B) < FLT_EPSILON) && ((B - A) < FLT_EPSILON);
}

inline RE::NiPoint2 Vec2Rotate(const RE::NiPoint2& vec, float angle)
{
	RE::NiPoint2 ret;
	ret.x = vec.x * cos(angle) - vec.y * sin(angle);
	ret.y = vec.x * sin(angle) + vec.y * cos(angle);
	return ret;
}

inline RE::NiPoint3 RotateAngleAxis(const RE::NiPoint3& vec, const float angle, const RE::NiPoint3& axis)
{
	float S = sin(angle);
	float C = cos(angle);

	const float XX = axis.x * axis.x;
	const float YY = axis.y * axis.y;
	const float ZZ = axis.z * axis.z;

	const float XY = axis.x * axis.y;
	const float YZ = axis.y * axis.z;
	const float ZX = axis.z * axis.x;

	const float XS = axis.x * S;
	const float YS = axis.y * S;
	const float ZS = axis.z * S;

	const float OMC = 1.f - C;

	return RE::NiPoint3(
		(OMC * XX + C) * vec.x + (OMC * XY - ZS) * vec.y + (OMC * ZX + YS) * vec.z,
		(OMC * XY + ZS) * vec.x + (OMC * YY + C) * vec.y + (OMC * YZ - XS) * vec.z,
		(OMC * ZX - YS) * vec.x + (OMC * YZ + XS) * vec.y + (OMC * ZZ + C) * vec.z
	);
}

//inline float ClampAngle(float angle, float min, float max)
//{
//	return fmod(angle, max - min) + min;
//}

inline float ClipAngle(float angle, float min, float max)
{
	return fmin(max, fmax(min, angle));
}

inline float GetAngle(RE::NiPoint2& a, RE::NiPoint2& b)
{
	return atan2(a.Cross(b), a.Dot(b));
}

inline RE::NiPoint3 ToOrientationRotation(const RE::NiPoint3& a_vector)
{
	RE::NiPoint3 ret;

	// Pitch
	ret.x = atan2(a_vector.z, std::sqrtf(a_vector.x * a_vector.x + a_vector.y * a_vector.y));

	// Yaw
	ret.y = atan2(a_vector.y, a_vector.x);

	// Roll
	ret.z = 0;

	return ret;
}

inline RE::NiPoint3 RotationToDirection(const float a_yaw, const float a_pitch)
{
	RE::NiPoint3 ret;

	float CP, SP, CY, SY;
	CP = cos(a_pitch);
	SP = sin(a_pitch);
	CY = cos(a_yaw);
	SY = sin(a_yaw);

	ret.x = CP * CY;
	ret.y = CP * SY;
	ret.z = SP;

	return ret;
}

inline RE::NiPoint3 Project(const RE::NiPoint3& A, const RE::NiPoint3& B)
{
	return (B * ((A.x * B.x + A.y * B.y + A.z * B.z) / (B.x * B.x + B.y * B.y + B.z * B.z)));
}

inline float Clamp(float value, float min, float max)
{
	return value < min ? min : value < max ? value : max;
}

inline float InterpEaseIn(const float& A, const float& B, float alpha, float exp)
{
	float const modifiedAlpha = std::pow(alpha, exp);
	return std::lerp(A, B, modifiedAlpha);
}

inline float InterpEaseOut(const float& A, const float& B, float alpha, float exp)
{
	float const modifiedAlpha = 1.f - pow(1.f - alpha, exp);
	return std::lerp(A, B, modifiedAlpha);
}

inline float InterpEaseInOut(const float& A, const float& B, float alpha, float exp)
{
	return std::lerp(A, B, (alpha < 0.5f) ? InterpEaseIn(0.f, 1.f, alpha * 2.f, exp) * 0.5f : InterpEaseOut(0.f, 1.f, alpha * 2.f - 1.f, exp) * 0.5f + 0.5f);
}

inline float InterpTo(float a_current, float a_target, float a_deltaTime, float a_interpSpeed)
{
	if (a_interpSpeed <= 0.f) {
		return a_target;
	}

	const float distance = a_target - a_current;

	if (distance * distance < FLT_EPSILON) {
		return a_target;
	}

	const float delta = distance * Clamp(a_deltaTime * a_interpSpeed, 0.f, 1.f);

	return a_current + delta;
}

inline float InterpAngleTo(float a_current, float a_target, float a_deltaTime, float a_interpSpeed)
{
	if (a_interpSpeed <= 0.f) {
		return a_target;
	}

	const float distance = NormalRelativeAngle(a_target - a_current);

	if (distance * distance < FLT_EPSILON) {
		return a_target;
	}

	const float delta = distance * Clamp(a_deltaTime * a_interpSpeed, 0.f, 1.f);

	return a_current + delta;
}
