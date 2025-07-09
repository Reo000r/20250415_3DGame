#include "ColliderDataCapsule.h"

ColliderDataCapsule::ColliderDataCapsule(bool isTrigger, float rad, float dist
	, Vector3 angle) :
	ColliderData(PhysicsData::ColliderKind::Capsule, isTrigger),
	_radius(rad),
	_angle(angle),
	_dist(dist)
{
}

Vector3 ColliderDataCapsule::GetStartPos(Position3 pos)
{
	return (pos - (_angle.Normalize() * _dist * 0.5f));
}

Vector3 ColliderDataCapsule::GetEndPos(Position3 pos)
{
	return (pos + (_angle.Normalize() * _dist * 0.5f));
}
