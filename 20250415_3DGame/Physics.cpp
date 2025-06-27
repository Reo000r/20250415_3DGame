#include "Physics.h"
#include "Collider.h"
#include "ColliderData.h"
#include "Rigidbody.h"

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

		// �\��ʒu�A�ړ��ʐݒ�
		Position3 nextPos = pos + vel;
		collider->rigidbody->SetVel(vel);
		collider->nextPos = nextPos;
	}

	// �����蔻��`�F�b�N�inextPos�w��j
	//std::list<OnCollideInfo> onCollideInfo = CheckCollide();

	// �ʒu�m��
	FixPosition();

	//// ������ʒm
	//for (auto& info : onCollideInfo)
	//{
	//	info.owner->OnCollide(info.colider);
	//}
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
					if (false/*IsCollide(objA, objB)*/) {
						auto priorityA = objA->GetPriority();
						auto priorityB = objB->GetPriority();

						std::shared_ptr<Collider> primary = objA;
						std::shared_ptr<Collider> secondary = objB;

						// �ǂ�����g���K�[�łȂ���Ύ��ڕW�ʒu�C��
						bool isTriggerAorB = objA->colliderData->IsTrigger() || objB->colliderData->IsTrigger();
						if (!isTriggerAorB) {
							// �ړ��D��x�𐔎��ɒ������Ƃ��ɍ��������ړ�
							if (priorityA > priorityB) {
								primary = objB;
								secondary = objA;
							}
							FixNextPosition(primary, secondary);
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
							// ���̍���ē�����肱�����̕���������
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

void Physics::FixNextPosition(std::shared_ptr<Collider> primary, std::shared_ptr<Collider> secondary) const
{
	// (���̏ꍇ�̂�)
	Vector3 priToSec = primary->nextPos - secondary->nextPos;
	Vector3 priToSecDir = priToSec.Normalize();
	// �␳�O�̈ʒu
	Position3 oldSecPos = secondary->nextPos;
	// (���̂܂܂��Ƃ��傤�Ǔ�����ʒu�ɂȂ�̂ŏ����␳���|����)
	float  awayDist = /*secondary->radius + primary->radius +*/ PhysicsData::kFixPositionOffset;
	Vector3 priToNewSecVel = priToSecDir * awayDist;
	Position3 fixedPos = secondary->nextPos + priToNewSecVel;
	secondary->nextPos = fixedPos;
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
