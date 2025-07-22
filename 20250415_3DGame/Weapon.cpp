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
    // modelはanimator側で消している
}

void Weapon::Init(int modelHandle, float rad, float dist, Vector3 transOffset, Vector3 scale, Vector3 angle)
{
	assert(modelHandle >= 0 && "モデルハンドルが正しくない");
	_modelHandle = modelHandle;
	_scale = scale;

	Matrix4x4 scaleMatrix = MatGetScale(scale);
	
	_transOffset = transOffset;
	_rotAngle = angle;
	_scale = scale;

	// 当たり判定のデータを適用
	SetColliderData(
		PhysicsData::ColliderKind::Capsule, 
		true, rad, dist, Vector3());
}

void Weapon::Update(Matrix4x4 parentWorldMatrix)
{
    // 各オフセットの行列を生成
    Matrix4x4 scaleMatrix = MatGetScale(_scale);
    Matrix4x4 rotY = MatRotateY(_rotAngle.y);
    Matrix4x4 rotX = MatRotateX(_rotAngle.x);
    Matrix4x4 rotZ = MatRotateZ(_rotAngle.z);
    Matrix4x4 rotationMatrix = MatMultiple(MatMultiple(rotZ, rotX), rotY);
    Matrix4x4 translationMatrix = MatTranslate(_transOffset);

    // 親の行列に対して補正値を合成する
    // 親 -> 平行移動 -> 回転 -> 拡縮の順
    Matrix4x4 worldMatrix = MatMultiple(
        scaleMatrix, MatMultiple(rotationMatrix, 
            MatMultiple(translationMatrix, parentWorldMatrix)));

    // モデルに最終的なワールド行列を適用
    MV1SetMatrix(_modelHandle, worldMatrix);

    // Rigidbodyの位置と当たり判定の向きを更新
    // (回転とスケールが適用される前の行列から計算)
    auto m1 = MatMultiple(translationMatrix, parentWorldMatrix);
    Position3 modelWorldPos = Vector3(
        m1.m[3][0],
        m1.m[3][1],
        m1.m[3][2]
    );
    rigidbody->SetPos(modelWorldPos);

    // 当たり判定の向きは回転まで適用した行列から取得
    auto m2 = MatMultiple(rotationMatrix, m1);
    Vector3 weaponDirection = Vector3(
        m2.m[1][0],
        m2.m[1][1],
        m2.m[1][2]
    ).Normalize();
    auto capsuleData = std::static_pointer_cast<ColliderDataCapsule>(colliderData);
    if (capsuleData) {
        capsuleData->SetAngle(weaponDirection);
    }
}

void Weapon::Draw()
{
	// 描画
	MV1DrawModel(_modelHandle);
}

void Weapon::OnCollide(const std::weak_ptr<Collider> collider)
{
}
