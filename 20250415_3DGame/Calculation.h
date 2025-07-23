#pragma once
#include <cmath>

namespace Calculation {
	inline constexpr float kPi = 3.1415926535897932384626433832795028f;

	/// <summary>
	/// 弧度法(ラジアン)へ変換
	/// </summary>
	/// <param name="degree">度数法</param>
	/// <returns>弧度法</returns>
	inline float ToRadian(float degree) {
		return (kPi / 180.0f) * degree;
	}
	/// <summary>
	/// 度数法(デグリー)へ変換
	/// </summary>
	/// <param name="radian">弧度法</param>
	/// <returns>度数法</returns>
	inline float ToDegree(float radian) {
		return (180.0f / kPi) * radian;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="radian"></param>
	/// <returns></returns>
	inline float RadianNormalize(float radian) {
		// まず、入力値を kPi*2 で割った剰余を求めることで、
		// 計算量を少なくしつつ、値を kPi*-2 から kPi*2 の間に収める。
		float wrapped = fmodf(radian, 2.0f * kPi);

		// 剰余計算の結果がまだ範囲外にある場合、
		// -kPi <= wrapped <= kPi
		// になるように調整
		if (wrapped < -kPi) {
			wrapped += 2.0f * kPi;
		}
		else if (wrapped >= kPi) {
			wrapped -= 2.0f * kPi;
		}

		return wrapped;
	}
}

// ほかのクラスからCalcという名前で使用できる
// (Calculationと呼び出しごとに打つのは冗長な為)
namespace Calc = Calculation;