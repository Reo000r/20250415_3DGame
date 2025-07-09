#pragma once
#include <memory>
#include "Vector3.h"
#include "ProjectSettings.h"

class Rigidbody;
class Physics;
class ColliderData;
class PlayerState;

/// <summary>
/// 当たり判定を持たせたいオブジェクトに継承させる基底クラス
/// </summary>
class Collider abstract : public std::enable_shared_from_this<Collider> {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="priority">位置補正の優先度</param>
	/// <param name="tag">タグ</param>
	/// <param name="colliderKind">当たり判定種別</param>
	/// <param name="isTrigger"></param>
	Collider(PhysicsData::Priority priority, PhysicsData::GameObjectTag tag, PhysicsData::ColliderKind colliderKind, bool isTrigger);
	virtual ~Collider();
	void EntryPhysics(std::weak_ptr<Physics> physics_);
	void ReleasePhysics();

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	virtual void OnCollide(const std::weak_ptr<Collider> collider) abstract;

	PhysicsData::GameObjectTag GetTag() const	{ return tag; }
	// 位置補正優先度情報を返す
	PhysicsData::Priority GetPriority() const	{ return priority; }
	
	Vector3 GetPos() const;
	Vector3 GetVel() const;
	Vector3 GetDir() const;

protected:
	std::shared_ptr<Rigidbody> rigidbody;
	std::shared_ptr<ColliderData> colliderData;

	std::weak_ptr<Physics> physics;

private:
	std::shared_ptr<ColliderData> CreateColliderData(
		PhysicsData::ColliderKind kind, bool isTrigger, 
		float rad = 0.0f, float dist = 0.0f, Vector3 angle = Vector3Up());
	
protected:	// それぞれのオブジェクトから呼びたいため
	void SetColliderData(
		PhysicsData::ColliderKind kind, bool isTrigger, 
		float rad = 0.0f, float dist = 0.0f, Vector3 angle = Vector3Up());

private:
	PhysicsData::GameObjectTag	tag;
	// 位置補正優先度情報
	PhysicsData::Priority priority;

private:
	// PhysicsがCollidableを自由に管理するためにフレンド
	friend Physics;

	// 以下はPhysicsのみが扱う型や変数

	// 移動予定位置
	Vector3 nextPos;
};

