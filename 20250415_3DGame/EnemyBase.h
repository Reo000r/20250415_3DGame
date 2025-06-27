#pragma once
#include "Geometry.h"
#include "Collider.h"
#include <memory>

class Camera;
class Animator;

class EnemyBase : public Collider
{
public:
	EnemyBase();
	~EnemyBase();

	void Init(/*int modelHandle, */std::weak_ptr<Camera> camera);
	void Update();
	void Draw();

	/// <summary>
	/// Õ“Ë‚µ‚½‚Æ‚«‚ÉŒÄ‚Î‚ê‚é
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> colider) override;

	Matrix4x4 GetRotMtx() const { return _rotMtx; }

private:
	void UpdateIdle();
	void UpdateWalk();
	void UpdateDash();
	void UpdateJump();
	void UpdateAttack();

	/// <summary>
	/// ˆÚ“®ŠÖ˜Aˆ—
	/// </summary>
	void Move(const float speed);
	/// <summary>
	/// ˆÚ“®ˆ—
	/// </summary>
	void Walk(const float speed);
	/// <summary>
	/// is•ûŒü‚Ö‚Ì•ûŒü“]Š·ˆ—
	/// </summary>
	void Rotate();

	std::shared_ptr<Animator> _animator;

	std::weak_ptr<Camera> _camera;

	float _rotAngle;
	Matrix4x4 _rotMtx;
};