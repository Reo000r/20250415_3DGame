#pragma once
#include "ColliderData.h"

class ColliderDataSphere : public ColliderData{
public:
	ColliderDataSphere(bool isTrigger, float rad);
	// ���a
	float _radius;
};

