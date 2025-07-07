#pragma once
#include "ColliderData.h"

class ColliderDataCapsule : public ColliderData {
public:
	ColliderDataCapsule(bool isTrigger, float rad, Vector3 start_, Vector3 end_);
	
	Vector3 GetStartPos();
	Vector3 GetEndPos();

	// ���a
	float radius;
private:
	// �J�n/�I���ʒu
	Vector3 start;
	Vector3 end;
};
