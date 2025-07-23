#include "ColliderDataCapsule.h"

ColliderDataCapsule::ColliderDataCapsule(bool isTrigger, float rad, Vector3 offset) :
	ColliderData(PhysicsData::ColliderKind::Capsule, isTrigger),
	_radius(rad),
	_offset(offset)
{
}

Vector3 ColliderDataCapsule::GetStartPos(Position3 pos)
{
	return pos;
}

Vector3 ColliderDataCapsule::GetEndPos(Position3 pos)
{
	return (pos + _offset);
}
