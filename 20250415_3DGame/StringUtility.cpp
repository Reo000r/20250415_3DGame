#include "StringUtility.h"

std::wstring StringUtility::GetWStringFromString(const std::string& str)
{
	// �܂��͕ϊ���̃T�C�Y�𑪂��Ă���
		// (�ϊ��O�ƕϊ���ŃT�C�Y���قȂ�ꍇ�����邽��)
	auto size = MultiByteToWideChar(
		CP_ACP,  // ���݂̃R�[�h�y�[�W
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,  // 0�ł��ʂ�炵��
		str.c_str(),
		str.size(),
		nullptr,
		0
	);

	// ���̃T�C�Y�����ɕԂ��ׂ�string�����T�C�Y
	std::wstring wstr;
	wstr.resize(size);

	// �T�C�Y���m�肵���̂ł�����xMultiByteToWideChar���Ăяo��(���ۂɕϊ�)
	MultiByteToWideChar(
		CP_ACP,  // ���݂̃R�[�h�y�[�W
		MB_PRECOMPOSED | MB_ERR_INVALID_CHARS,
		str.c_str(),
		str.size(),
		wstr.data(),  // wstr�̃f�[�^�{�̂̐擪�A�h���X
		wstr.size()   // wstr�̃f�[�^�T�C�Y
	);

	return wstr;
}
