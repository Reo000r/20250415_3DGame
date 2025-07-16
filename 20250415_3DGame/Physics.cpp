#include "Physics.h"
#include "Collider.h"
#include "ColliderData.h"
#include "ColliderDataSphere.h"
#include "ColliderDataCapsule.h"
#include "Rigidbody.h"
#include "DebugDraw.h"
#include "Collision.h"

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

	// Collider�̎�ނɂ���āA�����蔻��𕪂���
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
		auto capsuleA = std::static_pointer_cast<ColliderDataCapsule>(objA->colliderData);
		auto capsuleB = std::static_pointer_cast<ColliderDataCapsule>(objB->colliderData);
		
		// �J�v�Z��A�̐����Ɣ��a
		Vector3 startA = capsuleA->GetStartPos(objA->nextPos);
		Vector3 endA = capsuleA->GetEndPos(objA->nextPos);
		float radiusA = capsuleA->_radius;
		
		// �J�v�Z��B�̐����Ɣ��a
		Vector3 startB = capsuleB->GetStartPos(objB->nextPos);
		Vector3 endB = capsuleB->GetEndPos(objB->nextPos);
		float radiusB = capsuleB->_radius;
		
		// 2�̐����̍ŋߐړ_�����߂�
		Vector3 pA, pB;
		ClosestPointSegments(startA, endA, startB, endB, pA, pB);

		// �ŋߐړ_�Ԃ̋�����2����v�Z
		float distSq = (pA - pB).SqrMagnitude();
		float radSum = radiusA + radiusB;
		
		// �ŋߐړ_�Ԃ̋������A���a�̍��v��菬�������ǂ����ŏՓ˂𔻒�
		isHit = distSq < (radSum * radSum);
	}
	// ���ƃJ�v�Z��
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) || 
			(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere))
	{
		// ���ƃJ�v�Z���𔻒肷��
		std::shared_ptr<Collider> sphereObj;
		std::shared_ptr<Collider> capsuleObj;
		// objA�����ł��邩���`�F�b�N
		if (aKind == PhysicsData::ColliderKind::Sphere) {
			sphereObj = objA;
			capsuleObj = objB;
		}
		// �łȂ����objA�̓J�v�Z���AobjB����
		else {
			sphereObj = objB;
			capsuleObj = objA;
		}

		// ���ꂼ��̃R���C�_�[�����擾
		auto sphereData = std::static_pointer_cast<ColliderDataSphere>(sphereObj->colliderData);
		auto capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsuleObj->colliderData);
		
		// ���̏����擾
		Vector3 sphereCenter = sphereObj->nextPos;
		float sphereRadius = sphereData->_radius;
		
		// �J�v�Z���̏����擾
		Vector3 capsuleStart = capsuleData->GetStartPos(capsuleObj->nextPos);
		Vector3 capsuleEnd = capsuleData->GetEndPos(capsuleObj->nextPos);
		float capsuleRadius = capsuleData->_radius;
		
		// �_�Ɛ����̍ŋߐړ_�����߂�
		Vector3 closestPointOnCapsuleAxis = 
			ClosestPointPointAndSegment(
				sphereCenter, 
				capsuleStart, capsuleEnd);
		
		// �ŋߐړ_�Ԃ̋�����2����v�Z
		float distSq = (sphereCenter - closestPointOnCapsuleAxis).SqrMagnitude();
		// ���a�̍��v���v�Z
		float radSum = sphereRadius + capsuleRadius;
		
		// ���������a�̍��v��菬����������
		isHit = distSq < (radSum * radSum);
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

		// �����߂������̌���
		// secondary����primary�֌������x�N�g�����v�Z���A���K������
		Vector3 pushBackVec = primary->nextPos - secondary->nextPos;
		// �������[���ɋ߂��ꍇ�́A�����߂��������s��ɂȂ邽�ߏ������X�L�b�v
		if (pushBackVec.SqrMagnitude() < PhysicsData::kZeroTolerance) {
			return;
		}
		pushBackVec.Normalized();

		// �����߂�����(�ђʐ[�x)�̌v�Z
		// ���݂̒��S�Ԃ̋������v�Z
		float currentDist = (primary->nextPos - secondary->nextPos).Magnitude();
		// 2�̋��̔��a�̍��v
		float radiusSum = priColliderData->_radius + secColliderData->_radius;
		// �ђʐ[�x�ɃI�t�Z�b�g���������A�ŏI�I�ȉ����߂��������v�Z
		float pushBackDist = (radiusSum - currentDist) + PhysicsData::kFixPositionOffset;

		// �ʒu�̏C��
		// �v�Z���������Ƌ�������A�����߂��x�N�g���𐶐�
		Vector3 fixVec = pushBackVec * pushBackDist;

		// �D��x�������ł��݂��ɉ����߂��ꍇ
		if (isMutualPushback) {
			// �����߂��ʂ𔼕����ɕ�����
			Vector3 halfFixVec = fixVec * 0.5f;
			primary->nextPos += halfFixVec;
			secondary->nextPos -= halfFixVec;
		}
		// secondary�݂̂������߂��ꍇ
		else {
			// secondary���Aprimary���痣������(pushBackVec�̋t����)�ɉ����߂�
			secondary->nextPos -= fixVec;
		}
	}
	// �J�v�Z�����m
	else if (aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Capsule)
	{
		/*
		1. �Փˏ��̎擾
      �܂��A�Փ˂��Ă���2�̃J�v�Z���i����A�AB�Ƃ��܂��j���ꂼ��̎��̃t���[���̗\���ʒu�A���S���̎n�_�ƏI
  �_�A�����Ĕ��a���擾���܂��B


   2. �ŋߖT�_�̌v�Z
      �����蔻��̎��Ɠ��l�ɁA�J�v�Z��A�̒��S���i�����j�ƃJ�v�Z��B�̒��S���̊ԂŁA�ł��������߂��Ȃ�2�̓_
  �iA�̒��S����̓_Pa�AB�̒��S����̓_Pb�j���v�Z���܂��B����� ClosestPointSegments
  �֐����g���΋��߂��܂��B


   3. �����߂������̌���
      �X�e�b�v2�ŋ��߂�2�̍ŋߖT�_�iPa �� Pb�j�����ԃx�N�g�����v�Z���܂��B���̃x�N�g�����A���݂�����������
  ���߂̍ł������I�ȁu�����߂������v�ɂȂ�܂��B���̃x�N�g���𐳋K���i������1�Ɂj���Ă����܂��B


   4. �����߂������i�ђʐ[�x�j�̌v�Z
      ���ɁA�ǂꂾ���̋����������߂��΂悢�����v�Z���܂��B
       * �܂��A2�̃J�v�Z�����ǂꂾ���߂荞��ł��邩�i���ђʐ[�x�j�����߂܂��B����́u�J�v�Z��A��B�̔��a�̍�
         �v�v����u�ŋߖT�_Pa��Pb�̌��݂̋����v���������ƂŌv�Z�ł��܂��B
       * �����߂�����ɍĂѐڐG���Ȃ��悤�A�v�Z�����ђʐ[�x�ɁA�����킸���ȃI�t�Z�b�g�l�iPhysicsData::kFixPosi
         tionOffset�Ȃǁj�������܂��B���ꂪ�ŏI�I�ȁu�����߂������v�ƂȂ�܂��B


   5. �ʒu�̏C��
      �Ō�ɁA�v�Z�����u�����߂������v�Ɓu�����߂������v���g���āA�J�v�Z���̈ʒu���C�����܂��B
       * �D��x�ɉ����ĕЕ������𓮂����ꍇ�F
         �D��x�̒Ⴂ���̃J�v�Z���̗\���ʒu�inextPos�j���A�Z�o���������Ƌ��������ړ������܂��B
       * �D��x�������ł��݂��ɉ����߂��ꍇ�F �����߂��x�N�g���𔼕����ɕ����A�Е��͂��̕����ɁA�����Е��͂�
         �̋t�����ɁA���ꂼ��\���ʒu���ړ������܂��B

		*/


		// �����蔻��f�[�^�擾
		auto priCapsuleData = std::static_pointer_cast<ColliderDataCapsule>(primary->colliderData);
		auto secCapsuleData = std::static_pointer_cast<ColliderDataCapsule>(secondary->colliderData);

		// primary�J�v�Z���̏����擾
		Position3 priStart = priCapsuleData->GetStartPos(primary->nextPos);
		Position3 priEnd = priCapsuleData->GetEndPos(primary->nextPos);
		float priRadius = priCapsuleData->_radius;

		// secondary�J�v�Z���̏����擾
		Position3 secStart = secCapsuleData->GetStartPos(secondary->nextPos);
		Position3 secEnd = secCapsuleData->GetEndPos(secondary->nextPos);
		float secRadius = secCapsuleData->_radius;

		// �ŋߖT�_�̌v�Z
		// 2�̃J�v�Z���̒��S����ōł��߂��_(pPri, pSec)���v�Z
		Position3 pPri, pSec;
		ClosestPointSegments(priStart, priEnd, secStart, secEnd, pPri, pSec);

		// �����߂������̌���
		// �ŋߖT�_�Ԃ̃x�N�g�����v�Z���A�����߂�����������
		Vector3 pushBackVec = pSec - pPri;
		// �������[���ɋ߂��ꍇ�́A�J�v�Z���̒��S�ʒu��������������߂���i�߂荞�݂����Ă���ꍇ�Ȃǁj
		if (pushBackVec.SqrMagnitude() < PhysicsData::kZeroTolerance) {
			pushBackVec = secondary->nextPos - primary->nextPos;
		}
		pushBackVec.Normalized();

		// �����߂�����(�ђʐ[�x)�̌v�Z
		// �ŋߖT�_�Ԃ̌��݂̋������v�Z
		float currentDist = (pSec - pPri).Magnitude();
		// 2�̃J�v�Z���̔��a�̍��v
		float radiusSum = priRadius + secRadius;
		// �ђʐ[�x�ɃI�t�Z�b�g���������A�ŏI�I�ȉ����߂��������v�Z
		float pushBackDist = (radiusSum - currentDist) + PhysicsData::kFixPositionOffset;

		// �ʒu�̏C��
		// �v�Z���������Ƌ������g���āA�J�v�Z���̈ʒu���C��
		Vector3 fixVec = pushBackVec * pushBackDist;

		// �D��x�������ł��݂��ɉ����߂��ꍇ
		if (isMutualPushback) {
			// �����߂��ʂ𔼕����ɕ�����
			Vector3 halfFixVec = fixVec * 0.5f;
			primary->nextPos -= halfFixVec;
			secondary->nextPos += halfFixVec;
		}
		// secondary�݂̂������߂��ꍇ
		else {
			secondary->nextPos += fixVec;
		}
	}
	// ���ƃJ�v�Z��
	else if ((aKind == PhysicsData::ColliderKind::Sphere && bKind == PhysicsData::ColliderKind::Capsule) ||
		(aKind == PhysicsData::ColliderKind::Capsule && bKind == PhysicsData::ColliderKind::Sphere))
	{
		// �����蔻��f�[�^�擾
		std::shared_ptr<Collider> sphereObj;
		std::shared_ptr<Collider> capsuleObj;
		// primary��secondary�����ꂼ�ꋅ���J�v�Z�����𔻕�
		if (primary->colliderData->GetKind() == PhysicsData::ColliderKind::Sphere) {
			sphereObj = primary;
			capsuleObj = secondary;
		}
		else {
			sphereObj = secondary;
			capsuleObj = primary;
		}

		// ���ꂼ���Collider����f�[�^���擾
		auto sphereData = std::static_pointer_cast<ColliderDataSphere>(sphereObj->colliderData);
		auto capsuleData = std::static_pointer_cast<ColliderDataCapsule>(capsuleObj->colliderData);

		// ���̏����擾
		Vector3 sphereCenter = sphereObj->nextPos;
		float sphereRadius = sphereData->_radius;

		// �J�v�Z���̏����擾
		Vector3 capsuleStart = capsuleData->GetStartPos(capsuleObj->nextPos);
		Vector3 capsuleEnd = capsuleData->GetEndPos(capsuleObj->nextPos);
		float capsuleRadius = capsuleData->_radius;

		// �ŋߖT�_�̌v�Z
		// ���̒��S�ƃJ�v�Z���̒��S���Ƃ̍ŋߖT�_���v�Z
		Vector3 closestPointOnCapsuleAxis = ClosestPointPointAndSegment(sphereCenter, capsuleStart, capsuleEnd);

		// �����߂������̌���
		// �J�v�Z���̍ŋߖT�_���狅�̒��S�֌������x�N�g�����A�����߂������Ƃ���
		Vector3 pushBackVec = sphereCenter - closestPointOnCapsuleAxis;
		// �������[���ɋ߂��ꍇ�́A�I�u�W�F�N�g�̒��S�ʒu��������������߂���
		if (pushBackVec.SqrMagnitude() < PhysicsData::kZeroTolerance) {
			pushBackVec = sphereObj->nextPos - capsuleObj->nextPos;
		}
		pushBackVec.Normalized();

		// �����߂�����(�ђʐ[�x)�̌v�Z
		// �ŋߖT�_�Ԃ̌��݂̋������v�Z
		float currentDist = (sphereCenter - closestPointOnCapsuleAxis).Magnitude();
		// 2�̃I�u�W�F�N�g�̔��a�̍��v
		float radiusSum = sphereRadius + capsuleRadius;
		// �ђʐ[�x�ɃI�t�Z�b�g���������A�ŏI�I�ȉ����߂��������v�Z
		float pushBackDist = (radiusSum - currentDist) + PhysicsData::kFixPositionOffset;

		// �ʒu�̏C��
		// �v�Z���������Ƌ������g���āA�I�u�W�F�N�g�̈ʒu���C��
		Vector3 fixVec = pushBackVec * pushBackDist;

		// �D��x�������ł��݂��ɉ����߂��ꍇ
		if (isMutualPushback) {
			// �����߂��ʂ𔼕����ɕ����A���ꂼ��������߂�
			Vector3 halfFixVec = fixVec * 0.5f;
			sphereObj->nextPos += halfFixVec;	// ���������߂�
			capsuleObj->nextPos -= halfFixVec;	// �J�v�Z���������߂�
		}
		// �D��x�ɏ]���ĕЕ��݂̂������߂��ꍇ
		else {
			// �D��x�̒Ⴂ��(secondary)�������߂�
			// (fixVec�̓J�v�Z�����狅�ւ̃x�N�g���̂��߁A�����������g��������)
			if (secondary == sphereObj) {
				secondary->nextPos += fixVec;	// secondary(��)�������߂�
			}
			else {	// secondary == capsuleObj
				secondary->nextPos -= fixVec;	// secondary(�J�v�Z��)�������߂�
			}
		}
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
