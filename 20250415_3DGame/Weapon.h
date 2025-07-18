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

	void Init(int modelHandle, Matrix4x4 localOffsetMatrix, float rad, float dist, Vector3 angle = Vector3Up());
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

	Matrix4x4 _localOffsetMatrix;
};

