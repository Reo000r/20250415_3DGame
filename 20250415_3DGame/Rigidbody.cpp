#include "Rigidbody.h"

Rigidbody::Rigidbody() :
	pos(),
	dir(),
	vel(),
	useGravity(false)
{
}

void Rigidbody::Init(bool useGravity_)
{
	pos = {};
	dir = {};
	vel = {};
	useGravity = useGravity_;
}

void Rigidbody::SetVel(const Vector3& set)
{
	vel = set;
	// ’·‚³‚ª‚ ‚é‚È‚ç
	if (vel.SqrMagnitude() > 0)
	{
		// ³‹K‰»‚µ‚½ƒxƒNƒgƒ‹‚ðŒü‚«‚Æ‚µ‚Ä‘ã“ü
		dir = vel.Normalize();
	}
}
