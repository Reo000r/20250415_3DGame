#include "ColliderDataSphere.h"

ColliderDataSphere::ColliderDataSphere(bool isTrigger) :
	ColliderData(PhysicsData::ColliderKind::Sphere, isTrigger),
	radius(0.0f)
{
}
