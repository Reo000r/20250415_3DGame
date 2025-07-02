#pragma once
#include <string>
#include <DxLib.h>

namespace StringUtility {
	/// <summary>
	/// string‚©‚çwstring‚Ö•ÏŠ·‚·‚é
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::wstring GetWStringFromString(const std::string& str);

	/// <summary>
	/// wstring‚©‚çstring‚Ö•ÏŠ·‚·‚é
	/// “®ì–¢Šm”F
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string GetStringFromWString(const std::wstring& wstr);
}