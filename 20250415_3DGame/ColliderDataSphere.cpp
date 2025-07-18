#include "ColliderDataSphere.h"

ColliderDataSphere::ColliderDataSphere(bool isTrigger, float rad) :
	ColliderData(PhysicsData::ColliderKind::Sphere, isTrigger),
	_radius(rad)
{
}
