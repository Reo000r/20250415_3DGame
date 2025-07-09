#pragma once
#include <memory>
#include "Vector3.h"
#include "ProjectSettings.h"

class Rigidbody;
class Physics;
class ColliderData;
class PlayerState;

/// <summary>
/// �����蔻��������������I�u�W�F�N�g�Ɍp����������N���X
/// </summary>
class Collider abstract : public std::enable_shared_from_this<Collider> {
public:
	/// <summary>
	/// �R���X�g���N�^
	/// </summary>
	/// <param name="priority">�ʒu�␳�̗D��x</param>
	/// <param name="tag">�^�O</param>
	/// <param name="colliderKind">�����蔻����</param>
	/// <param name="isTrigger"></param>
	Collider(PhysicsData::Priority priority, PhysicsData::GameObjectTag tag, PhysicsData::ColliderKind colliderKind, bool isTrigger);
	virtual ~Collider();
	void EntryPhysics(std::weak_ptr<Physics> physics_);
	void ReleasePhysics();

	/// <summary>
	/// �Փ˂����Ƃ��ɌĂ΂��
	/// </summary>
	/// <param name="colider"></param>
	virtual void OnCollide(const std::weak_ptr<Collider> collider) abstract;

	PhysicsData::GameObjectTag GetTag() const	{ return tag; }
	// �ʒu�␳�D��x����Ԃ�
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
	
protected:	// ���ꂼ��̃I�u�W�F�N�g����Ăт�������
	void SetColliderData(
		PhysicsData::ColliderKind kind, bool isTrigger, 
		float rad = 0.0f, float dist = 0.0f, Vector3 angle = Vector3Up());

private:
	PhysicsData::GameObjectTag	tag;
	// �ʒu�␳�D��x���
	PhysicsData::Priority priority;

private:
	// Physics��Collidable�����R�ɊǗ����邽�߂Ƀt�����h
	friend Physics;

	// �ȉ���Physics�݂̂������^��ϐ�

	// �ړ��\��ʒu
	Vector3 nextPos;
};

