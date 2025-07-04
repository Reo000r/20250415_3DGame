#pragma once

#include <string>
#include <forward_list>

class Animator
{
public:
	struct AnimData
	{
		int animIndex = -1;			// �A�j���[�V�����ԍ�(���f�[�^�ɂ�����)
		int attachNo = -1;			// �A�^�b�`�ԍ�
		std::wstring animName = L"";	// �A�j���[�V�����̖��O
		float animSpeed = 1.0f;		// �A�j���[�V�����̍Đ����x
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

	/// <summary>
	/// �w�肳�ꂽ�A�j���[�V�����̍X�V
	/// </summary>
	/// <param name="data"></param>
	void UpdateAnim(AnimData& data);

	/// <summary>
	/// �u�����h�䗦�̍X�V
	/// </summary>
	void UpdateAnimBlendRate();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="animName"></param>
	/// <param name="isLoop"></param>
	void ChangeAnim(const std::wstring animName, bool isLoop);

	AnimData& FindAnimData(const std::wstring animName);

	int GetModelHandle() const{ return _model; }

	std::wstring GetCurrentAnimName() const{ return _currentAnimName; }
	std::wstring GetBlendingAnimName() const{ return _blendingAnimName; }

	/// <summary>
	/// �w��̃A�j���[�V�������I�����Ă��邩
	/// </summary>
	/// <param name="animName"></param>
	/// <returns></returns>
	bool IsEnd(const std::wstring animName) { return FindAnimData(animName).isEnd; }

private:
	// ���f���n���h��
	int _model;
	
	// �A�j���[�V������forward_list(list)
	// ���Ԃ��C�ɂ���K�v���Ȃ�����
	// �������̌������̂���forward_list
	std::forward_list<AnimData> _animDataList;

	// ���ݍĐ����̃A�j���[�V������
	std::wstring _currentAnimName;
	// ���Ƀ��C���ƂȂ�u�����h���̃A�j���[�V������
	std::wstring _blendingAnimName;

	// �A�j���[�V�����̃u�����h�䗦
	// current->blending
	// 0.0->1.0
	float _blendRate;
};

