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

	/// <summary>
	/// 武器初期化
	/// </summary>
	/// <param name="modelHandle">モデルハンドル</param>
	/// <param name="colRad">当たり判定半径</param>
	/// <param name="colHeight">当たり判定の高さ</param>
	/// <param name="transOffset">位置補正</param>
	/// <param name="scale">拡縮補正</param>
	/// <param name="angle">角度補正</param>
	void Init(int modelHandle, float colRad,
		float colHeight,
		Vector3 transOffset = Vector3(),
		Vector3 scale = Vector3(1, 1, 1),
		Vector3 angle = Vector3());

	/// <summary>
	/// 武器更新
	/// </summary>
	/// <param name="parentWorldMatrix">ワールド行列とみなす行列</param>
	void Update(Matrix4x4 parentWorldMatrix);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();


	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	void SetCollisionState(bool isCollision);

private:
	// モデルハンドル
	int _modelHandle;

	Vector3 _transOffset;
	Vector3 _rotAngle;
	Vector3 _scale;
};

