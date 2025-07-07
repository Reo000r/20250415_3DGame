#pragma once
#include "Geometry.h"
#include "Collider.h"

/// <summary>
/// �v���C���[��G���g������N���X
/// </summary>
class Weapon : public Collider
{
public:
	Weapon();
	~Weapon();

	void Init(int modelHandle, float rad, Vector3 start, Vector3 end);
	void Update(Matrix4x4 worldMatrix);
	void Draw();

private:
	// ���f���n���h��
	int _modelHandle;
};

