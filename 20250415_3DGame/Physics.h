#pragma once
#include <memory>
#include <list>

class Collider;

/// <summary>
/// �����������i��
/// </summary>
class Physics final {
public:
	/// <summary>
	/// �I�u�W�F�N�g�o�^
	/// </summary>
	void Entry(std::shared_ptr<Collider> collider);

	/// <summary>
	/// �I�u�W�F�N�g�o�^����
	/// </summary>
	void Release(std::shared_ptr<Collider> collider);

	void Update();

private:

	// OnCollide�̒x���ʒm�̂��߂̃f�[�^
	struct OnCollideInfo
	{
		std::shared_ptr<Collider> owner;
		std::shared_ptr<Collider> colider;
	};

	// �o�^���ꂽCollider�̃��X�g
	std::list<std::shared_ptr<Collider>> _colliders;

	std::list<OnCollideInfo> CheckCollide() const;

	/// <summary>
	/// �������Ă��邩�ǂ�����������
	/// </summary>
	bool IsCollide(const std::shared_ptr<Collider> objA, const std::shared_ptr<Collider> objB) const;

	/// <summary>
	/// ��������Collider�𓮂��Ȃ����̂Ƃ��āA
	/// �������ɓ�����Collider�̈ʒu��␳����
	/// ��O������true�������Ă����ꍇ�͂����𖳎��������������߂�
	/// </summary>
	/// <param name="primary">�����Ȃ�Collider</param>
	/// <param name="secondary">�␳���s��Collider</param>
	/// <param name="isMutualPushback">�����������߂���</param>
	void FixNextPosition(std::shared_ptr<Collider> primary, std::shared_ptr<Collider> secondary, bool isMutualPushback) const;
	/// <summary>
	/// �ʒu����
	/// </summary>
	void FixPosition();
};

