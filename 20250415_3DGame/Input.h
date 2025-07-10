#pragma once
#include "Vector3.h"

#include <map>
#include <string>
#include <vector>

#define USE_STICK	// ����ŃX�e�B�b�N���g��

/// <summary>
/// ���͌n���R���g���[������N���X
/// �V���O���g��������
/// </summary>
class Input final {
public:
	/// <summary>
	/// �V���O���g���I�u�W�F�N�g��Ԃ�
	/// </summary>
	/// <returns>�V���O���g���I�u�W�F�N�g</returns>
	static Input& GetInstance();

	/// <summary>
	/// ���͏��̍X�V
	/// </summary>
	void Update();

	/// <summary>
	/// ������Ă��邩�ǂ����̎擾
	/// </summary>
	/// <param name="button">������s���{�^��</param>
	/// <returns>������Ă����true�A�łȂ����false</returns>
	bool IsPress(const char* key) const;

	/// <summary>
	/// �����ꂽ�u�Ԃ��ǂ����̎擾
	/// </summary>
	/// <param name="button">������s���{�^��</param>
	/// <returns>������Ă����true�A�łȂ����false</returns>
	bool IsTrigger(const char* key) const;

	/// <summary>
	/// �E�X�e�B�b�N�̓��͏���Vector3�^�ŕԂ�
	/// x�Az�ɒl������
	/// </summary>
	/// <returns>�E�X�e�B�b�N�̓��͏��</returns>
	Vector3 GetPadRightSitck() const;
	/// <summary>
	/// �E�X�e�B�b�N�̓��͏���Vector3�^�ŕԂ�
	/// x�Az�ɒl������
	/// �Ō�ɂ��������͂�Ԃ�
	/// </summary>
	/// <returns>�E�X�e�B�b�N�̓��͏��</returns>
	Vector3 GetPadRightSitckLast() const;

	/// <summary>
	/// ���X�e�B�b�N�̓��͏���Vector3�^�ŕԂ�
	/// x�Az�ɒl������
	/// </summary>
	/// <returns>���X�e�B�b�N�̓��͏��</returns>
	Vector3 GetPadLeftSitck() const;
	/// <summary>
	/// ���X�e�B�b�N�̓��͏���Vector3�^�ŕԂ�
	/// x�Az�ɒl������
	/// �Ō�ɂ��������͂�Ԃ�
	/// </summary>
	/// <returns>���X�e�B�b�N�̓��͏��</returns>
	Vector3 GetPadLeftSitckLast() const;

private:
	Input();
	Input(const Input&) = delete;
	void operator=(const Input&) = delete;

	// ���Ӌ@����
	enum class PeripheralType {
		keybd,	// �L�[�{�[�h
		pad1	// �p�b�h�P
	};
	///���͏���`�p
	struct InputState {
		PeripheralType type;	// ���Ӌ@����
		int id;					// ���ۂ̓��͂�\���萔
	};

	using InputTable_t = std::map<std::string, std::vector<InputState>>;
	InputTable_t _inputTable;		// ���O�Ǝ��ۂ̓��͂̑Ή��\
	InputTable_t _tempInputTable;	// ���O�Ǝ��ۂ̓��͂̑Ή��\(�ꎞ�I�ȃR�s�[)

	/// <summary>
	/// ���͂������l�ɐݒ肷��
	/// </summary>
	void SetDefault();

	///// <summary>
	///// �L�[�R���t�B�O�p(�s�����ύX���m�肷��)
	///// </summary>
	//void CommitEdittedInputTable();

	///// <summary>
	///// �L�[�R���t�B�O�p(�s�����ύX���Ȃ��������Ƃɂ���)
	///// </summary>
	//void RollbackEdittedInputTable();

	// �����ꂽ���ǂ������L�^���Ă�������
	using InputRecord_t = std::map<std::string, bool>;
	InputRecord_t _current;		// ���݉����Ă邩�ǂ���
	InputRecord_t _last;		// ���O�ɉ�����Ă����ǂ���

	std::vector<std::string> _orderForDisplay;	// �\���p(map�̏����̓R���g���[���ł��Ȃ�����)

	// ���͕ۑ�
	int _currentRawPadState;			// ���̐��p�b�h�X�e�[�g
	int _lastRawPadState;				// ���O�̐��p�b�h�X�e�[�g
	char _currentRawKeybdState[256];	// ���̃L�[�{�[�h���
	char _lastRawKeybdState[256];		// ���O�̃L�[�{�[�h���

	// ���E�X�e�B�b�N���͏��
	Vector3 _rightStickInput;
	Vector3 _leftStickInput;
	// ���E�X�e�B�b�N�̍Ō�̓��͏��
	Vector3 _rightStickLastInput;
	Vector3 _leftStickLastInput;

	/// <summary>
	/// ���݂̃L�[�{�[�h��Ԃ𒲂ׂčł�
	/// �Ⴂ�L�[�R�[�h��Ԃ�
	/// �ЂƂ�������ĂȂ����-1��Ԃ�
	/// </summary>
	/// <returns></returns>
	int GetKeyboradState()const;

	/// <summary>
	/// ���݂�PAD��Ԃ𒲂ׂčł�
	/// �Ⴂ�L�[���͂�Ԃ�
	/// �ЂƂ�������ĂȂ����-1��Ԃ�
	/// </summary>
	int GetPadState(int padno)const;

	/// <summary>
	/// ���݂̓��̓e�[�u�����Z�[�u����
	/// </summary>
	void SaveInputTable();

	/// <summary>
	/// ���̓e�[�u�������[�h���Ĕ��f����
	/// </summary>
	void LoadInputTable();
};