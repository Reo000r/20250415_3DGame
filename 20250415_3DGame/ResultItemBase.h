#pragma once

/// <summary>
/// リザルト表示項目のインターフェースを定義する基底クラス
/// </summary>
class ResultItemBase abstract
{
public:
	ResultItemBase();
	virtual ~ResultItemBase();

	/// <summary>
	/// アニメーションタイマーを受け取り、内部状態を更新する
	/// </summary>
	/// <param name="animationTimer"></param>
	virtual void Update(int animationTimer) abstract;

	/// <summary>
	/// 基準となる座標を受け取り、項目を描画する
	/// </summary>
	virtual void Draw(float labelX, float valueX, float baseY) const abstract;

	/// <summary>
	/// アニメーションが完了したか
	/// </summary>
	/// <returns></returns>
	virtual bool IsAnimationFinished() const abstract;

protected:

	int _fontHandle;
};
