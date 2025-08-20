#pragma once
#include <memory>
#include <unordered_map>

class Collider;
class Player;

/// <summary>
/// Playerのバフ状態を管理する
/// </summary>
class PlayerBuffManager final
{
public:
	enum class BuffType {
		None,
		Heal,		// 継続回復
		ScoreBoost,	// 取得スコア増加
		Strength,	// 攻撃力増加
	};

	struct BuffData {
		BuffType type;
		int activeFrame = 0;
		float amount = 0.0f;
		bool isActive = false;
	};

public:

	void Init(std::weak_ptr<Player> owner);
	void Update();

	/// <summary>
	/// バフを設定
	/// </summary>
	void AttachBuff(BuffData data);

	/// <summary>
	/// 指定のバフが有効かどうか返す
	/// </summary>
	float IsActiveBuffState(BuffType buff) const;
	/// <summary>
	/// 指定のバフがどの程度の効果量か返す
	/// 0の場合は効果なし
	/// </summary>
	float GetBuffAmount(BuffType buff) const;

private:

	/// <summary>
	/// バフの初期化
	/// </summary>
	void InitBuff();
	/// <summary>
	/// バフの残り時間を更新
	/// </summary>
	void UpdateBuff();


	std::weak_ptr<Player> _owner;

	std::vector<BuffData> _buffs;	// バフと詳細

};

