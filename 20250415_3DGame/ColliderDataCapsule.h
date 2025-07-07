#pragma once
#include "ColliderData.h"

class ColliderDataCapsule : public ColliderData {
public:
	ColliderDataCapsule(bool isTrigger, float rad, Vector3 start_, Vector3 end_);
	
	Vector3 GetStartPos();
	Vector3 GetEndPos();

	// 半径
	float radius;
private:
	// 開始/終了位置
	Vector3 start;
	Vector3 end;
};
