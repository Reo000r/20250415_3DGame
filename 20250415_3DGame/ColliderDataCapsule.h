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

	void SetAngle(const Vector3& angle) { _angle = angle; }

	// 以降の変数のアクセス権を渡す
	friend Collider;
	friend Physics;

private:
	// 半径
	float _radius;
	// 角度
	Vector3 _angle;
	// startからendまでの距離
	float _dist;
};
