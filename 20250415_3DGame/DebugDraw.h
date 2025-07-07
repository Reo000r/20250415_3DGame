#pragma once
#include "Geometry.h"
#include <vector>
#include <string>

/// <summary>
/// �f�o�b�O�p�̕`������܂Ƃ߁A��ŕ\������N���X
/// </summary>
class DebugDraw
{
public:
	static void Clear();
	static void Draw();

	/// <summary>
	/// �����`����o�^
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="color"></param>
	static void DrawLine(const Vector3& start, const Vector3& end, int color);

	/// <summary>
	/// ���`����o�^
	/// </summary>
	/// <param name="center"></param>
	/// <param name="rad"></param>
	/// <param name="color"></param>
	static void DrawSphere(const Vector3& center, float rad, int color);
	
	/// <summary>
	/// �J�v�Z���`����o�^
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	/// <param name="rad"></param>
	/// <param name="color"></param>
	static void DrawCapsule(const Vector3& start, const Vector3& end, float rad, int color);

private:
	// �������
	struct LineInfo
	{
		Vector3	start;
		Vector3	end;
		int		color;
	};
	// �����
	struct SphereInfo
	{
		Vector3	center;
		float	rad;
		int		color;
	};
	// �J�v�Z�����
	struct CapsuleInfo
	{
		Vector3	start;
		Vector3	end;
		float	rad;
		int		color;
	};

	static std::vector<LineInfo>	_lineInfo;
	static std::vector<SphereInfo>	_sphereInfo;
	static std::vector<CapsuleInfo>	_capsuleInfo;
};