#pragma once
#include "ColliderData.h"

class ColliderDataSphere : public ColliderData{
public:
	ColliderDataSphere(bool isTrigger);
	// ���a
	float radius;
};

