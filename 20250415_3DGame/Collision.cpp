#include "Collision.h"
#include "ProjectSettings.h"

#include <algorithm>

Position3 ClosestPointPointAndSegment(const Position3& point, const Position3& start, const Position3& end)
{
	// �����̎n�_����I�_�ւ̃x�N�g��
	Vector3 segmentVec = end - start;
	// �����̎n�_����w�肳�ꂽ�_�ւ̃x�N�g��
    Vector3 pointToStart = point - start;
    
    // �����̒�����2��
    float segmentLengthSq = segmentVec.SqrMagnitude();
    
    // �����̒������ق�0�̏ꍇ�A�n�_���ŋߐړ_
	if (segmentLengthSq < 0.000001f) {
		return start;
	}
	
	// �_������x�N�g���Ɏˉe�����Ƃ��̃p�����[�^t���v�Z
	// t = (point - start)�E(end - start) / |end - start|^2
	float t = Dot(pointToStart, segmentVec) / segmentLengthSq;
	
	// �p�����[�^t��0.0�`1.0�͈̔͂ɃN�����v����
	t = std::clamp(t, 0.0f, 1.0f);
	
	// �ŋߐړ_���v�Z���ĕԂ�
	return (start + segmentVec * t);
}

void ClosestPointSegments(const Position3& startA, const Position3& endA, const Position3& startB, const Position3& endB, Position3& closestPointA, Position3& closestPointB)
{
	// 2�̐����̍ŋߐړ_�����߂鏈��
	Vector3 segAVec = endA - startA;	// ����A�̕����x�N�g��
	Vector3 segBVec = endB - startB;	// ����B�̕����x�N�g��
	Vector3 offsetVec = startA - startB;

	float segALenSq = segAVec.SqrMagnitude();	// ����A�̒�����2��
	float segBLenSq = segBVec.SqrMagnitude();	// ����B�̒�����2��
	float segBDotOffset = Dot(segBVec, offsetVec);	// ����B�̕����x�N�g���Ɨ��n�_�ԃx�N�g���̓���

	// �����̐������_�Ƃ݂Ȃ���ꍇ
	if (segALenSq <= PhysicsData::kZeroTolerance && segBLenSq <= PhysicsData::kZeroTolerance) {
		// �������_�̏ꍇ
		closestPointA = startA;
		closestPointB = startB;
		return;		// �����I��
	}
	// ����A�݂̂��_�̏ꍇ
	else if (segALenSq <= PhysicsData::kZeroTolerance) {
		closestPointA = startA;
		closestPointB = ClosestPointPointAndSegment(startA, startB, endB);
		return;		// �����I��
	}
	// ����B���_�ł���ꍇ
	else if (segBLenSq <= PhysicsData::kZeroTolerance) {
		closestPointA = ClosestPointPointAndSegment(startB, startA, endA);
		closestPointB = startB;
		return;		// �����I��
	}

	// �����������Ƃ��Ĉ�����ꍇ
	// ����A�̕����x�N�g���Ɨ��n�_�ԃx�N�g���̓���
	float segADotOffset = Dot(segAVec, offsetVec);
	// ����A�Ɛ���B�̕����x�N�g���̓���
	float segADotSegB = Dot(segAVec, segBVec);
	// ������̍ŋߐړ_�����߂邽�߂̕ϐ�(0.0�`1.0)
	float paramA, paramB;

	// �ŋߐړ_�̃p�����[�^���v�Z
	CalculateClosestSegmentParameters(
		segALenSq, segBLenSq, segADotSegB, segADotOffset, segBDotOffset, 
		paramA, paramB);

	// ���߂��l��p���āA������̍ŋߐړ_�����߂�
	closestPointA = startA + segAVec * paramA;
	closestPointB = startB + segBVec * paramB;
}

void CalculateClosestSegmentParameters(
	float segALenSq, float segBLenSq, float segADotSegB,
	float segADotOffset, float segBDotOffset,
	float& paramA, float& paramB)
{
	// �ŋߐړ_�����߂鎮�̕���
	float denom = segALenSq * segBLenSq - segADotSegB * segADotSegB;

	// 2���������s�łȂ��ꍇ
	if (std::abs(denom) > PhysicsData::kZeroTolerance) {
		// ������̍ŋߐړ_���v�Z
		paramA = (segADotSegB * segBDotOffset - segADotOffset * segBLenSq) / denom;
		paramB = (segALenSq * segBDotOffset - segADotSegB * segADotOffset) / denom;
	}
	// ���������s�ȏꍇ
	else {
		// �v�Z���ȗ������邽�߁A�Е��̎n�_����Ɍv�Z����
		paramA = 0.0f;
		paramB = segBDotOffset / segBLenSq;
	}

	// �v�Z���ꂽ�p�����[�^�������Ƃ�0-1�͈͓̔��ɂ��邩�`�F�b�N
	// �ŋߐړ_���������̓����ɂ���P�[�X
	if (paramA >= 0.0f && paramA <= 1.0f && paramB >= 0.0f && paramB <= 1.0f) {
		// �͈͓��Ȃ̂ŉ������Ȃ�
		return;
	}

	// ����ȊO�̏ꍇ�A�ŋߐړ_�͏��Ȃ��Ƃ��Е��̐����̒[�_�ɂ���
	// �ŋߐړ_�������̊O���ɂ���ꍇ�A�[�_�ɃN�����v���čČv�Z����

	// paramA��0-1�ɃN�����v
	paramA = std::clamp(paramA, 0.0f, 1.0f);
	// �N�����v����paramA������paramB���Čv�Z
	paramB = (segBDotOffset + segADotSegB * paramA) / segBLenSq;
	paramB = std::clamp(paramB, 0.0f, 1.0f);

	// ����ɁA�N�����v����paramB������paramA���āX�v�Z
	// ����ɂ��A�����͈̔͂��݂��ɍœK�ȏ�ԂɎ�������
	paramA = (-segADotOffset + segADotSegB * paramB) / segALenSq;
	paramA = std::clamp(paramA, 0.0f, 1.0f);

	return;
}