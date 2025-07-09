#pragma once
#include "Geometry.h"
#include <vector>
#include <string>

/// <summary>
/// �f�o�b�O�p�̕`������܂Ƃ߁A��ŕ\������N���X
/// </summary>
class DebugDraw final {
public:
	/// <summary>
	/// �V���O���g���I�u�W�F�N�g��Ԃ�
	/// </summary>
	/// <returns>�V���O���g���I�u�W�F�N�g</returns>
	static DebugDraw& GetInstance();

	void Clear();
	void Draw();

	/// <summary>
	/// �����`����o�^
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="color"></param>
	void DrawLine(const Vector3& start, const Vector3& end, int color);

	/// <summary>
	/// ���`����o�^
	/// </summary>
	/// <param name="center"></param>
	/// <param name="rad"></param>
	/// <param name="color"></param>
	void DrawSphere(const Vector3& center, float rad, int color);
	
	/// <summary>
	/// �J�v�Z���`����o�^
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="rad"></param>
	/// <param name="color"></param>
	void DrawCapsule(const Vector3& start, const Vector3& end, float rad, int color);

private:
	DebugDraw();
	DebugDraw(const DebugDraw&) = delete;
	void operator=(const DebugDraw&) = delete;

	// �������
	struct LineInfo {
		Vector3	start;
		Vector3	end;
		int		color;
	};
	// �����
	struct SphereInfo {
		Vector3	center;
		float	rad;
		int		color;
	};
	// �J�v�Z�����
	struct CapsuleInfo {
		Vector3	start;
		Vector3	end;
		float	rad;
		int		color;
	};

	std::vector<LineInfo>	_lineInfo;
	std::vector<SphereInfo>	_sphereInfo;
	std::vector<CapsuleInfo>	_capsuleInfo;
};