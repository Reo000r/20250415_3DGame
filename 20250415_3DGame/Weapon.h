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

	void Init(int modelHandle, float rad, float dist, Vector3 angle = Vector3Up());
	void Update(Matrix4x4 worldMatrix);
	void Draw() ;

	/// <summary>
	/// �Փ˂����Ƃ��ɌĂ΂��
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

private:
	// ���f���n���h��
	int _modelHandle;
};

