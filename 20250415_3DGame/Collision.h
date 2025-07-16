#pragma once
#include "Geometry.h"

// �����蔻��n�̏������s���֐����܂Ƃ߂��t�@�C��



/// <summary>
/// �_�Ɛ����̍ŋߐړ_�����߂�
/// </summary>
/// <param name="point">�_</param>
/// <param name="start">�����̎n�_</param>
/// <param name="end">�����̏I�_</param>
/// <returns>������̍ŋߐړ_</returns>
Position3 ClosestPointPointAndSegment(const Position3& point, 
	const Position3& start, const Position3& end);

/// <summary>
/// 2�̐����̍ŋߐړ_�����߂�
/// </summary>
/// <param name="startA">����A�n�_</param>
/// <param name="endA">����A�I�_</param>
/// <param name="startB">����B�n�_</param>
/// <param name="endB">����B�I�_</param>
/// <param name="closestPointA">A��̍ŋߐړ_</param>
/// <param name="closestPointB">B��̍ŋߐړ_</param>
void ClosestPointSegments(
	const Position3& startA, const Position3& endA,
	const Position3& startB, const Position3& endB,
	Position3& closestPointA, Position3& closestPointB);

/// <summary>
/// 2�̐����̍ŋߐړ_�����߂邽�߂̃p�����[�^���v�Z����
/// (ClosestPointSegments�̕⏕�֐�)
/// </summary>
/// <param name="lenSq1">����1�̒�����2��</param>
/// <param name="lenSq2">����2�̒�����2��</param>
/// <param name="dot12">�����x�N�g�����m�̓���</param>
/// <param name="dot1r">����1�ƃI�t�Z�b�g�x�N�g���̓���</param>
/// <param name="dot2r">����2�ƃI�t�Z�b�g�x�N�g���̓���</param>
/// <param name="param1">�o�͗p�̃p�����[�^1</param>
/// <param name="param2">�o�͗p�̃p�����[�^2</param>
void CalculateClosestSegmentParameters(
	float lenSq1, float lenSq2, float dot12,
	float dot1r, float dot2r,
	float& param1, float& param2);
