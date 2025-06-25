#pragma once
#include "Vector3.h"

namespace PhysicsData {
	/// <summary>
	/// �����������̂̔��ʂ��s�����߂̃^�O
	/// </summary>
	enum class GameObjectTag
	{
		None,			// �^�O�Ȃ�
		Player,			// �v���C���[
		Enemy,			// �G
		SystemWall,		// �V�X�e����
		StepGround,		// ����̒n��
	};

	/// <summary>
	/// �����蔻����
	/// </summary>
	enum class ColliderKind {
		Cube,
		Sphere,
		Capsule,
	};

	/// <summary>
	/// �ʒu�␳�̗D�揇��
	/// ���l���Ⴂ�������ɂ���
	/// </summary>
	enum class Priority : int{
		Static,		// �����o����Ȃ�
		High,
		Middle,
		Low,
	};

	/// <summary>
	/// ���E�̏d��
	/// ���f���͉��Z�Ōv�Z
	/// </summary>
	const Vector3 Gravity = { 0.0f, -9.81f * 0.1f, 0.0f };
	const Vector3 MaxGravityAccel = Gravity * 15;
	
	// ������
	const float decelerationRate = 0.98f;
	// �ړ����Ă��Ȃ��Ƃ݂Ȃ����臒l
	const float sleepThreshold = 0.005f;

	// �����蔻��񐔂̍ő吔
	constexpr int kCheckCollideMaxCount = 1000;
	// �����蔻�莞�ɉ����߂��ǉ��␳��
	// (���̂܂ܖ߂��Ƃ��傤�Ǔ������Ă��܂��ʒu�ɂȂ邽��)
	constexpr float kFixPositionOffset = 0.00001f;
}
