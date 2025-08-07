#pragma once
#include "ResultItemBase.h"

class ResultTimeItem final : public ResultItemBase
{
public:
	ResultTimeItem(float clearTime);
	~ResultTimeItem();

	void Update(int animationTimer) override;
	void Draw(float labelX, float valueX, float baseY) const override;
	bool IsAnimationFinished() const override;

private:

	float _clearTime;
	int _displayTime;	// MMSSss
	int _finalizedDigitCount;
	bool _isAnimationFinished;
};