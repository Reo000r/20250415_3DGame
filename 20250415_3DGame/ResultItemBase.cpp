#include "ResultItemBase.h"
#include <DxLib.h>

ResultItemBase::ResultItemBase() :
	_fontHandle(-1)
{
}

ResultItemBase::~ResultItemBase()
{
	if (_fontHandle != -1) {
		DeleteFontToHandle(_fontHandle);
	}
}
