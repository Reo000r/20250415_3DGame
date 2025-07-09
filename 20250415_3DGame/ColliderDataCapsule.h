#pragma once
#include "ColliderData.h"

class Collider;
class Physics;

class ColliderDataCapsule : public ColliderData {
public:
	ColliderDataCapsule(bool isTrigger, float rad, float dist, 
		Vector3 angle = Vector3Up());
	
	Vector3 GetStartPos(Position3 pos);
	Vector3 GetEndPos(Position3 pos);
	float GetRad() { return _radius; }
	Vector3 GetAngle() { return _angle; }
	float GetDist() { return _dist; }

	// ˆÈ~‚Ì•Ï”‚ÌƒAƒNƒZƒXŒ ‚ğ“n‚·
	friend Collider;
	friend Physics;

private:
	// ”¼Œa
	float _radius;
	// Šp“x
	Vector3 _angle;
	// start‚©‚çend‚Ü‚Å‚Ì‹——£
	float _dist;
};
