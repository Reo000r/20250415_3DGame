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

std::string StringUtility::GetStringFromWString(const std::wstring& wstr)
{
	// CP_UTF8 �͕ϊ���̕����R�[�h��UTF-8�Ɏw��
	// �ϊ���̕����񒷂��擾
	int buffer_size = WideCharToMultiByte(
		CP_UTF8, 
		0, 
		wstr.c_str(), 
		-1, 
		NULL, 
		0, 
		NULL, 
		NULL);
	if (buffer_size == 0) {
	    return ""; // �ϊ����s
	}
	
	// �o�b�t�@���m��
	std::string str(buffer_size - 1, '\0'); // -1�̓k���I�[������
	
	// �ϊ����s
	WideCharToMultiByte(
		CP_UTF8, 
		0, 
		wstr.c_str(), 
		-1, 
		&str[0], 
		buffer_size, 
		NULL, 
		NULL);
	
	return str;
}
