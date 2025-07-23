#pragma once
#include "ColliderData.h"

class Collider;
class Physics;

class ColliderDataCapsule : public ColliderData {
public:
	ColliderDataCapsule(bool isTrigger, float rad, 
		Vector3 angle = Vector3Up());
	
	/// <summary>
	/// 始点を返す(原点)
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	Vector3 GetStartPos(Position3 pos);
	/// <summary>
	/// 終点を返す(pos+offset)
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	Vector3 GetEndPos(Position3 pos);
	float GetRad() { return _radius; }
	Vector3 GetAngle() { return _offset.Normalize(); }
	float GetDist() { return _offset.Magnitude(); }

	void SetStartToEnd(const Vector3& offset) { _offset = offset; }

	// 以降の変数のアクセス権を渡す
	friend Collider;
	friend Physics;

private:
	// 半径
	float _radius;
	// startからendまでのベクトル
	Vector3 _offset;
};
