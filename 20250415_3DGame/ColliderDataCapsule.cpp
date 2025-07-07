#include "ColliderDataCapsule.h"

ColliderDataCapsule::ColliderDataCapsule(bool isTrigger, float rad, Vector3 start_, Vector3 end_):
	ColliderData(PhysicsData::ColliderKind::Capsule, isTrigger),
	radius(rad),
	start(start_),
	end(end_)
{
}

Vector3 ColliderDataCapsule::GetStartPos()
{
	return Vector3();
}

Vector3 ColliderDataCapsule::GetEndPos()
{
	return Vector3();
}
