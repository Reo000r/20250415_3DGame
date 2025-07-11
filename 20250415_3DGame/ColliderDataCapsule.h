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

	// Θ~ΜΟΜANZX πn·
	friend Collider;
	friend Physics;

private:
	// Όa
	float _radius;
	// px
	Vector3 _angle;
	// start©ηendάΕΜ£
	float _dist;
};
