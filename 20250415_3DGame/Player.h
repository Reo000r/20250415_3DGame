#pragma once
#include "Geometry.h"
#include "Collider.h"
#include <memory>

class Camera;
class Animator;
class WeaponPlayer;

/// <summary>
/// 
/// </summary>
class Player final : public Collider {
public:
	Player();
	~Player();

	void Init(/*int modelHandle, */std::weak_ptr<Camera> camera);
	void Update();
	void Draw();

	/// <summary>
	/// 衝突したときに呼ばれる
	/// </summary>
	/// <param name="colider"></param>
	void OnCollide(const std::weak_ptr<Collider> collider) override;

	float GetHitPoint() { return _hitPoint; }
	bool IsAlive() { return (_hitPoint > 0.0f); }
	
	/// <summary>
	/// ダメージを受ける処理
	/// </summary>
	/// <param name="damage">受けるダメージ量</param>
	/// <param name="attacker">攻撃してきた相手</param>
	void TakeDamage(float damage, std::shared_ptr<Collider> attacker);

private:
	// UpdateのStateパターン
	// _nowUpdateStateが変数であることを分かりやすくしている
	using UpdateFunc_t = void(Player::*)();
	UpdateFunc_t _nowUpdateState;

private:
	/// <summary>
	/// ステートの遷移条件を確認し、変更可能なステートがあればそれに遷移する
	/// </summary>
	void CheckStateTransition();

	/// <summary>
	/// 武器更新
	/// </summary>
	void WeaponUpdate();

	void UpdateIdle();
	void UpdateWalk();
	void UpdateDash();
	void UpdateAttackFirst();
	void UpdateAttackSecond();
	void UpdateAttackThird();
	void UpdateDamage();
	void UpdateDeath();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move(const float speed);
	/// <summary>
	/// 進行方向への方向転換処理
	/// </summary>
	void Rotate();

	std::unique_ptr<Animator> _animator;

	std::weak_ptr<Camera> _camera;

	std::unique_ptr<WeaponPlayer> _weapon;

	float _rotAngle;
	//Matrix4x4 _rotMtx;
	Quaternion _quaternion;

	// 攻撃の派生入力があったかどうか
	bool  _hasDerivedAttackInput;

	float _hitPoint;

	// 攻撃力
	float _attackPower;

	int _reactCooltime;
};

