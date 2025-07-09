#include "Physics.h"
#include "Collider.h"
#include "ColliderData.h"
#include "ColliderDataSphere.h"
#include "ColliderDataCapsule.h"
#include "Rigidbody.h"
#include "DebugDraw.h"

#include <cassert>
#include <vector>

void Physics::Entry(std::shared_ptr<Collider> collider)
{
	// (見つからなかった場合はend)
	auto it = (
		std::find(
			_colliders.begin(),
			_colliders.end(),
			collider));
	// 見つからなければ(登録されていなければ)
	if (it == _colliders.end())
	{
		_colliders.emplace_back(collider);	// 登録
	}
	// 既に登録されていたらassert
	else
	{
		assert(false && "指定のcolliderは登録済");
	}
}

void Physics::Release(std::shared_ptr<Collider> collider)
{
	// 登録解除(eraseif 要C++20)
	auto count = std::erase_if(
		_colliders,
		[collider](std::shared_ptr<Collider> target) { return target == collider; });
	// 登録されてなかったらassert
	if (count <= 0)
	{
		assert(false && "指定のcolliderは未登録");
	}
}

void Physics::Update()
{
	// 移動
	for (auto& collider : _colliders) {
		// 位置に移動量を足す
		Position3 pos = collider->rigidbody->GetPos();
		Vector3 vel = collider->rigidbody->GetVel();

		// 減速量を掛ける
		vel.x *= PhysicsData::decelerationRate * 0.5f;
		vel.z *= PhysicsData::decelerationRate * 0.5f;

		// 重力を利用するなら重力を与える
		if (collider->rigidbody->UseGravity()) {
			vel += PhysicsData::Gravity;

			// 最大重力加速度より小さかったら補正
			// (重力はマイナスのため)
			if (vel.y < PhysicsData::MaxGravityAccel.y) {
				vel.y = PhysicsData::MaxGravityAccel.y;
			}
		}

		// 移動量切り捨て処理
		Vector3 velXZ = vel;
		velXZ.y = 0.0f;
		// 移動していないとみなされる閾値よりも小さければ
		if (vel.Magnitude() < PhysicsData::sleepThreshold) {
			vel = {};
		}
		// XZのみを見て閾値よりも小さければ
		else if (velXZ.Magnitude() < PhysicsData::sleepThreshold) {
			vel.x = vel.z = 0.0f;
		}

		// もともとの情報、予定情報をデバッグ表示
#if _DEBUG
		// 球
		if (collider->colliderData->GetKind() == PhysicsData::ColliderKind::Sphere)
		{
			auto sphereData = std::static_pointer_cast<ColliderDataSphere>(collider->colliderData);
			float radius = sphereData->_radius;
			DebugDraw::GetInstance().DrawSphere(pos, radius, 0xff00ff);
		}
		// カプセル
		if (collider->colliderData->GetKind() == PhysicsData::ColliderKind::Capsule)
		{
			auto capsuleData = std::static_pointer_cast<ColliderDataCapsule>(collider->colliderData);
			Position3 pos = collider->rigidbody->GetPos();
			Position3 start = capsuleData->GetStartPos(pos);
			Position3 end = capsuleData->GetEndPos(pos);
			float radius = capsuleData->GetRad();
			DebugDraw::GetInstance().DrawSphere(start, radius, 0x0000ff);
			DebugDraw::GetInstance().DrawSphere(end, radius, 0xff0000);
			DebugDraw::GetInstance().DrawCapsule(start, end, radius, 0xff00ff);
		}
#endif

		// 予定位置、移動量設定
		Position3 nextPos = pos + vel;
		collider->rigidbody->SetVel(vel);
		collider->nextPos = nextPos;
	}

	// 当たり判定チェック（nextPos指定）
	std::list<OnCollideInfo> onCollideInfo = CheckCollide();

	// 位置確定
	FixPosition();

	// 当たり通知
	for (auto& info : onCollideInfo)
	{
		info.owner->OnCollide(info.colider);
	}
}

std::list<Physics::OnCollideInfo> Physics::CheckCollide() const
{
	std::list<OnCollideInfo> onCollideInfo;
	// 衝突通知、ポジション補正
	bool doCheck = true;
	int	checkCount = 0;	// チェック回数
	while (doCheck) {
		doCheck = false;
		++checkCount;

		// 2重ループで全オブジェクト当たり判定
		// 重いので近いオブジェクト同士のみ当たり判定するなど工夫がいる
		for (auto& objA : _colliders) {
			for (auto& objB : _colliders) {
				if (objA != objB) {
					// ぶつかっていれば
					if (IsCollide(objA, objB)) {
						auto priorityA = objA->GetPriority();
						auto priorityB = objB->GetPriority();

						std::shared_ptr<Collider> primary = objA;
						std::shared_ptr<Collider> secondary = objB;

						// どちらもトリガーでなければ次目標位置修正
						// (どちらかがトリガーなら補正処理を飛ばす)
						bool isTriggerAorB = objA->colliderData->IsTrigger() || objB->colliderData->IsTrigger();
						if (!isTriggerAorB) {
							// 移動優先度を数字に直したときに高い方を移動
							if (priorityA > priorityB) {
								primary = objB;
								secondary = objA;
							}
							// 位置補正を行う
							// priorityが同じだった場合は両方押し戻す
							FixNextPosition(primary, secondary, (priorityA == priorityB));
						}

						// 衝突通知情報の更新
						bool hasPrimaryInfo = false;
						bool hasSecondaryInfo = false;
						for (const auto& item : onCollideInfo) {
							// 既に通知リストに含まれていたら呼ばない
							if (item.owner == primary) {
								hasPrimaryInfo = true;
							}
							if (item.owner == secondary) {
								hasSecondaryInfo = true;
							}
						}
						if (!hasPrimaryInfo) {
							// MEMO:(実体作って入れるよりこっちの方が速そう)
							onCollideInfo.push_back({ primary, secondary });
						}
						if (!hasSecondaryInfo) {
							onCollideInfo.push_back({ secondary, primary });
						}

						// 一度でもヒット+補正したら衝突判定と補正やりなおし
						if (!isTriggerAorB) {	// 片方がトリガーならヒットとりなおさない
							doCheck = true;
						}
						break;
					}
				}
			}
			if (doCheck) {
				break;
			}
		}

		// 無限ループ避け
		if (checkCount > PhysicsData::kCheckCollideMaxCount && doCheck) {
#if _DEBUG
			//printfDx("当たり判定チェック回数が最大数(%d)を超えた\n",
			//	PhysicsData::kCheckCollideMaxCount);
#endif
			break;
		}
	}
	return onCollideInfo;
}

bool Physics::IsCollide(const std::shared_ptr<Collider> objA, const std::shared_ptr<Collider> objB) const
{
	bool isHit = false;

	// collidableの種類によって、当たり判定を分ける
	auto aKind = objA->colliderData->GetKind();
	auto bKind = objB->colliderData->GetKind();

	auto aTag = objA->GetTag();
	auto bTag = objB->GetTag();

	// どちらかのオブジェクトが相手のタグを無視する設定になっていたらreturn
	if (objA->colliderData->IsThroughTarget(bTag) ||
		objB->colliderData->IsThroughTarget(aTag)) return false;

	// 球同士
	if (aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Sphere)
	{
		auto atob = objB->nextPos - objA->nextPos;
		auto atobLength = atob.Magnitude();

		// お互いの距離が、それぞれの半径を足したものより小さければ当たる
		auto objAColliderData = std::static_pointer_cast<ColliderDataSphere>(objA->colliderData);
		auto objBColliderData = std::static_pointer_cast<ColliderDataSphere>(objB->colliderData);
		isHit = (atobLength < objAColliderData->_radius + objBColliderData->_radius);
	}
	// カプセル同士
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule)
	{

	}
	// 球とカプセル
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) || 
			(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere))
	{

	}

	return isHit;
}

void Physics::FixNextPosition(std::shared_ptr<Collider> primary, std::shared_ptr<Collider> secondary, bool isMutualPushback) const
{
	// collidableの種類によって、当たり判定を分ける
	auto aKind = primary->colliderData->GetKind();
	auto bKind = secondary->colliderData->GetKind();

	// 球同士
	if (aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Sphere)
	{
		// 当たり判定データ取得
		auto priColliderData = std::static_pointer_cast<ColliderDataSphere>(primary->colliderData);
		auto secColliderData = std::static_pointer_cast<ColliderDataSphere>(secondary->colliderData);

		Vector3 priToSec = primary->nextPos - secondary->nextPos;
		Vector3 priToSecDir = priToSec.Normalize();
		// 補正前の位置
		Position3 oldSecPos = secondary->nextPos;
		// そのままだとちょうど当たる位置になるので少し補正を掛ける
		float  awayDist = secColliderData->_radius + priColliderData->_radius + PhysicsData::kFixPositionOffset;
		Vector3 priToNewSecVel = priToSecDir * awayDist;
		// 両方補正する場合
		if (isMutualPushback) {

		}
		// secのみ補正する場合
		else {
			Position3 fixedPos = secondary->nextPos + priToNewSecVel;
			secondary->nextPos = fixedPos;
		}
	}
	// カプセル同士
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule)
	{

	}
	// 球とカプセル
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) ||
		(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere))
	{

	}
	
}

void Physics::FixPosition()
{
	for (auto& collider : _colliders) {
		Vector3 toFixedPos;
		if (true) {
			// 床判定を無理やり作る
			if (collider->nextPos.y <= 0.0f) {
				collider->nextPos.y = 0.0f;
			}
		}
		else {
			
		}
		// Posを更新するので、velocityもそこに移動するvelocityに修正
		toFixedPos = collider->nextPos - collider->rigidbody->GetPos();

		collider->rigidbody->SetVel(toFixedPos);

		// 位置確定
		collider->rigidbody->SetPos(collider->nextPos);
	}
}
