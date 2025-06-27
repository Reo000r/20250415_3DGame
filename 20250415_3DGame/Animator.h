#pragma once

#include <string>
#include <vector>

class Animator
{
private:
	// �v���g�^�C�v�錾
	typedef wchar_t TCHAR;

public:
	struct AnimData
	{
		int animIndex = -1;			// �A�j���[�V�����ԍ�(���f�[�^�ɂ�����)
		int attachNo = -1;			// �A�^�b�`�ԍ�
		std::wstring animName = L"";	// �A�j���[�V�����̖��O
		float frame = 0.0f;			// �A�j���[�V�����̍Đ�����
		float totalFrame = 0.0f;	// �A�j���[�V�����̑��Đ�����
		bool  isLoop = false;		// ���[�v���邩
		bool  isEnd = false;		// ���[�v���Ȃ��ꍇ�I�����Ă��邩
	};

	Animator();
	~Animator();

	void Init(int model);
	void Update();

	/// <summary>
	/// �ŏ��Ɏg�p����A�j���[�V������ݒ�
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void SetStartAnim(const std::wstring animName, const bool isLoop = true);
	/// <summary>
	/// �Q�[�����Ŏg�p����A�j���[�V�����f�[�^
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void SetAnimData(const std::wstring animName, const bool isLoop);
	/// <summary>
	/// �A�j���[�V���������w�肵�A�^�b�`
	/// (�u�����h�̐i�s�󋵂��~�܂邽�ߏ���������ړI�Ŏg�p)
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void AttachAnim(const std::wstring animName, const bool isLoop = false);
	void UpdateAnim(AnimData& data);
	void UpdateAnimBlend();
	void ChangeAnim(const std::wstring animName, bool isLoop);

	AnimData& FindAnimData(const std::wstring animName);

	int GetModelHandle() const{ return _model; }
	AnimData& GetBlendingAnim();
	AnimData& GetExistAnim();
	bool GetBlendingAnimFinishState();

private:
	// ���f���n���h��
	int _model;
	
	std::vector<AnimData> _animDataList;

	// ���ݍĐ����̃A�j���[�V������
	std::wstring _currentAnimName;
	// ���Ƀ��C���ƂȂ�u�����h���̃A�j���[�V������
	std::wstring _blendingAnimName;

	// �A�j���[�V�����̃u�����h�䗦
	// 0.0->1.0
	float _blendRate;
};

