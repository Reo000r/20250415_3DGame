#include "Weapon.h"
#include "Rigidbody.h"
#include "ColliderData.h"
#include "ColliderDataCapsule.h"
#include <cassert>

#include <DxLib.h>

Weapon::Weapon() :
	Collider(PhysicsData::Priority::Static, 
		PhysicsData::GameObjectTag::PlayerAttack, 
		PhysicsData::ColliderKind::Capsule, 
		true),
	_modelHandle(-1)
{
	rigidbody->Init(false);
}

Weapon::~Weapon()
{
	MV1DeleteModel(_modelHandle);
}

void Weapon::Init(int modelHandle, Matrix4x4 localOffsetMatrix, float rad, float dist, Vector3 angle)
{
	assert(modelHandle >= 0 && "モデルハンドルが正しくない");
	_modelHandle = modelHandle;

	_localOffsetMatrix = localOffsetMatrix;

	// 当たり判定のデータを適用
	SetColliderData(PhysicsData::ColliderKind::Capsule, true, rad, dist, angle);
}

void Weapon::Update(Matrix4x4 parentWorldMatrix)
{
	// 武器自身のワールド行列を計算
	// 親のワールド行列に武器のローカルオフセット行列を乗算
	Matrix4x4 weaponWorldMatrix = MatMultiple(_localOffsetMatrix, parentWorldMatrix);

	// モデルのワールド行列を適用
	MV1SetMatrix(_modelHandle, weaponWorldMatrix);

	// Rigidbodyの位置を更新
	// モデルのワールド行列から位置情報を取り出し、Rigidbodyに設定
	// これにより、物理演算（衝突判定など）がモデルの描画位置と同期する
	Position3 modelWorldPos = Vector3(
		weaponWorldMatrix.m[3][0],
		weaponWorldMatrix.m[3][1],
		weaponWorldMatrix.m[3][2]
	);
	rigidbody->SetPos(modelWorldPos);
}

void Weapon::Draw()
{
	// 描画
	MV1DrawModel(_modelHandle);
}

void Weapon::OnCollide(const std::weak_ptr<Collider> collider)
{
}
