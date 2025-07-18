#pragma once
#include "ColliderData.h"

class ColliderDataSphere : public ColliderData{
public:
	ColliderDataSphere(bool isTrigger, float rad);
	// 半径
	float _radius;
};

