#pragma once
#include "Geometry.h"
#include "Collider.h"

/// <summary>
/// プレイヤーや敵が使う武器クラス
/// </summary>
class Weapon : public Collider
{
public:
	Weapon();
	~Weapon();

	void Init(int modelHandle, float rad, Vector3 start, Vector3 end);
	void Update(Matrix4x4 worldMatrix);
	void Draw();

private:
	// モデルハンドル
	int _modelHandle;
};

