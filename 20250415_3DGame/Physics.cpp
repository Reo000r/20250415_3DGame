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
	// (������Ȃ������ꍇ��end)
	auto it = (
		std::find(
			_colliders.begin(),
			_colliders.end(),
			collider));
	// ������Ȃ����(�o�^����Ă��Ȃ����)
	if (it == _colliders.end())
	{
		_colliders.emplace_back(collider);	// �o�^
	}
	// ���ɓo�^����Ă�����assert
	else
	{
		assert(false && "�w���collider�͓o�^��");
	}
}

void Physics::Release(std::shared_ptr<Collider> collider)
{
	// �o�^����(eraseif �vC++20)
	auto count = std::erase_if(
		_colliders,
		[collider](std::shared_ptr<Collider> target) { return target == collider; });
	// �o�^����ĂȂ�������assert
	if (count <= 0)
	{
		assert(false && "�w���collider�͖��o�^");
	}
}

void Physics::Update()
{
	// �ړ�
	for (auto& collider : _colliders) {
		// �ʒu�Ɉړ��ʂ𑫂�
		Position3 pos = collider->rigidbody->GetPos();
		Vector3 vel = collider->rigidbody->GetVel();

		// �����ʂ��|����
		vel.x *= PhysicsData::decelerationRate * 0.5f;
		vel.z *= PhysicsData::decelerationRate * 0.5f;

		// �d�͂𗘗p����Ȃ�d�͂�^����
		if (collider->rigidbody->UseGravity()) {
			vel += PhysicsData::Gravity;

			// �ő�d�͉����x��菬����������␳
			// (�d�͂̓}�C�i�X�̂���)
			if (vel.y < PhysicsData::MaxGravityAccel.y) {
				vel.y = PhysicsData::MaxGravityAccel.y;
			}
		}

		// �ړ��ʐ؂�̂ď���
		Vector3 velXZ = vel;
		velXZ.y = 0.0f;
		// �ړ����Ă��Ȃ��Ƃ݂Ȃ����臒l�������������
		if (vel.Magnitude() < PhysicsData::sleepThreshold) {
			vel = {};
		}
		// XZ�݂̂�����臒l�������������
		else if (velXZ.Magnitude() < PhysicsData::sleepThreshold) {
			vel.x = vel.z = 0.0f;
		}

		// ���Ƃ��Ƃ̏��A�\������f�o�b�O�\��
#if _DEBUG
		// ��
		if (collider->colliderData->GetKind() == PhysicsData::ColliderKind::Sphere)
		{
			auto sphereData = std::static_pointer_cast<ColliderDataSphere>(collider->colliderData);
			float radius = sphereData->_radius;
			DebugDraw::GetInstance().DrawSphere(pos, radius, 0xff00ff);
		}
		// �J�v�Z��
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

		// �\��ʒu�A�ړ��ʐݒ�
		Position3 nextPos = pos + vel;
		collider->rigidbody->SetVel(vel);
		collider->nextPos = nextPos;
	}

	// �����蔻��`�F�b�N�inextPos�w��j
	std::list<OnCollideInfo> onCollideInfo = CheckCollide();

	// �ʒu�m��
	FixPosition();

	// ������ʒm
	for (auto& info : onCollideInfo)
	{
		info.owner->OnCollide(info.colider);
	}
}

std::list<Physics::OnCollideInfo> Physics::CheckCollide() const
{
	std::list<OnCollideInfo> onCollideInfo;
	// �Փ˒ʒm�A�|�W�V�����␳
	bool doCheck = true;
	int	checkCount = 0;	// �`�F�b�N��
	while (doCheck) {
		doCheck = false;
		++checkCount;

		// 2�d���[�v�őS�I�u�W�F�N�g�����蔻��
		// �d���̂ŋ߂��I�u�W�F�N�g���m�̂ݓ����蔻�肷��ȂǍH�v������
		for (auto& objA : _colliders) {
			for (auto& objB : _colliders) {
				if (objA != objB) {
					// �Ԃ����Ă����
					if (IsCollide(objA, objB)) {
						auto priorityA = objA->GetPriority();
						auto priorityB = objB->GetPriority();

						std::shared_ptr<Collider> primary = objA;
						std::shared_ptr<Collider> secondary = objB;

						// �ǂ�����g���K�[�łȂ���Ύ��ڕW�ʒu�C��
						// (�ǂ��炩���g���K�[�Ȃ�␳�������΂�)
						bool isTriggerAorB = objA->colliderData->IsTrigger() || objB->colliderData->IsTrigger();
						if (!isTriggerAorB) {
							// �ړ��D��x�𐔎��ɒ������Ƃ��ɍ��������ړ�
							if (priorityA > priorityB) {
								primary = objB;
								secondary = objA;
							}
							// �ʒu�␳���s��
							// priority�������������ꍇ�͗��������߂�
							FixNextPosition(primary, secondary, (priorityA == priorityB));
						}

						// �Փ˒ʒm���̍X�V
						bool hasPrimaryInfo = false;
						bool hasSecondaryInfo = false;
						for (const auto& item : onCollideInfo) {
							// ���ɒʒm���X�g�Ɋ܂܂�Ă�����Ă΂Ȃ�
							if (item.owner == primary) {
								hasPrimaryInfo = true;
							}
							if (item.owner == secondary) {
								hasSecondaryInfo = true;
							}
						}
						if (!hasPrimaryInfo) {
							// MEMO:(���̍���ē�����肱�����̕���������)
							onCollideInfo.push_back({ primary, secondary });
						}
						if (!hasSecondaryInfo) {
							onCollideInfo.push_back({ secondary, primary });
						}

						// ��x�ł��q�b�g+�␳������Փ˔���ƕ␳���Ȃ���
						if (!isTriggerAorB) {	// �Е����g���K�[�Ȃ�q�b�g�Ƃ�Ȃ����Ȃ�
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

		// �������[�v����
		if (checkCount > PhysicsData::kCheckCollideMaxCount && doCheck) {
#if _DEBUG
			//printfDx("�����蔻��`�F�b�N�񐔂��ő吔(%d)�𒴂���\n",
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

	// collidable�̎�ނɂ���āA�����蔻��𕪂���
	auto aKind = objA->colliderData->GetKind();
	auto bKind = objB->colliderData->GetKind();

	auto aTag = objA->GetTag();
	auto bTag = objB->GetTag();

	// �ǂ��炩�̃I�u�W�F�N�g������̃^�O�𖳎�����ݒ�ɂȂ��Ă�����return
	if (objA->colliderData->IsThroughTarget(bTag) ||
		objB->colliderData->IsThroughTarget(aTag)) return false;

	// �����m
	if (aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Sphere)
	{
		auto atob = objB->nextPos - objA->nextPos;
		auto atobLength = atob.Magnitude();

		// ���݂��̋������A���ꂼ��̔��a�𑫂������̂�菬������Γ�����
		auto objAColliderData = std::static_pointer_cast<ColliderDataSphere>(objA->colliderData);
		auto objBColliderData = std::static_pointer_cast<ColliderDataSphere>(objB->colliderData);
		isHit = (atobLength < objAColliderData->_radius + objBColliderData->_radius);
	}
	// �J�v�Z�����m
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule)
	{

	}
	// ���ƃJ�v�Z��
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) || 
			(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere))
	{

	}

	return isHit;
}

void Physics::FixNextPosition(std::shared_ptr<Collider> primary, std::shared_ptr<Collider> secondary, bool isMutualPushback) const
{
	// collidable�̎�ނɂ���āA�����蔻��𕪂���
	auto aKind = primary->colliderData->GetKind();
	auto bKind = secondary->colliderData->GetKind();

	// �����m
	if (aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Sphere)
	{
		// �����蔻��f�[�^�擾
		auto priColliderData = std::static_pointer_cast<ColliderDataSphere>(primary->colliderData);
		auto secColliderData = std::static_pointer_cast<ColliderDataSphere>(secondary->colliderData);

		Vector3 priToSec = primary->nextPos - secondary->nextPos;
		Vector3 priToSecDir = priToSec.Normalize();
		// �␳�O�̈ʒu
		Position3 oldSecPos = secondary->nextPos;
		// ���̂܂܂��Ƃ��傤�Ǔ�����ʒu�ɂȂ�̂ŏ����␳���|����
		float  awayDist = secColliderData->_radius + priColliderData->_radius + PhysicsData::kFixPositionOffset;
		Vector3 priToNewSecVel = priToSecDir * awayDist;
		// �����␳����ꍇ
		if (isMutualPushback) {

		}
		// sec�̂ݕ␳����ꍇ
		else {
			Position3 fixedPos = secondary->nextPos + priToNewSecVel;
			secondary->nextPos = fixedPos;
		}
	}
	// �J�v�Z�����m
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule)
	{

	}
	// ���ƃJ�v�Z��
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
			// ������𖳗������
			if (collider->nextPos.y <= 0.0f) {
				collider->nextPos.y = 0.0f;
			}
		}
		else {
			
		}
		// Pos���X�V����̂ŁAvelocity�������Ɉړ�����velocity�ɏC��
		toFixedPos = collider->nextPos - collider->rigidbody->GetPos();

		collider->rigidbody->SetVel(toFixedPos);

		// �ʒu�m��
		collider->rigidbody->SetPos(collider->nextPos);
	}
}
