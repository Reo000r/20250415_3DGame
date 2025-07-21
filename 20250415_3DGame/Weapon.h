#pragma once
#include "Geometry.h"
#include "Collider.h"

/// <summary>
/// プレイヤーや敵が使う武器クラス
/// </summary>
class Weapon : public Collider
{
public:
	Weapon();
	~Weapon();

	void Init(int modelHandle, float rad, float dist, 
		Vector3 transOffset = Vector3(),
		Vector3 scale = Vector3(1,1,1),
		Vector3 angle = Vector3());
	void Update(Matrix4x4 parentWorldMatrix);
	void Draw();

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

private:
	// モデルハンドル
	int _modelHandle;

	Vector3 _transOffset;
	Vector3 _rotAngle;
	Vector3 _scale;
};

