#pragma once
#include <string>
#include <DxLib.h>

namespace StringUtility {
	/// <summary>
	/// string����wstring�֕ϊ�����
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::wstring GetWStringFromString(const std::string& str);

	/// <summary>
	/// wstring����string�֕ϊ�����
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string GetStringFromWString(const std::wstring& wstr);
}