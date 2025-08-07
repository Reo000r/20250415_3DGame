#pragma once
#include "ResultItemBase.h"

class ResultScoreItem final : public ResultItemBase
{
public:
	ResultScoreItem(int finalScore);
	~ResultScoreItem();

	void Update(int animationTimer) override;
	void Draw(float labelX, float valueX, float baseY) const override;

	bool IsAnimationFinished() const override;

private:

	int _finalScore;
	int _displayScore;
	int _finalizedDigitCount;
	bool _isAnimationFinished;
};