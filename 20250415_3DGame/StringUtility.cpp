#include "StringUtility.h"

std::wstring StringUtility::GetWStringFromString(const std::string& str)
{
	// まずは変換後のサイズを測っておく
		// (変換前と変換後でサイズが異なる場合があるため)
	auto size = MultiByteToWideChar(
		CP_ACP,  // 現在のコードページ
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,  // 0でも通るらしい
		str.c_str(),
		str.size(),
		nullptr,
		0
	);

	// そのサイズを元に返すべきstringをリサイズ
	std::wstring wstr;
	wstr.resize(size);

	// サイズが確定したのでもう一度MultiByteToWideCharを呼び出す(実際に変換)
	MultiByteToWideChar(
		CP_ACP,  // 現在のコードページ
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		str.size(),
		wstr.data(),  // wstrのデータ本体の先頭アドレス
		wstr.size()   // wstrのデータサイズ
	);

	return wstr;
}
