#include "Animator.h"

#include <DxLib.h>
#include <cassert>

namespace {
	// �A�j���[�V�����̃u�����h����(frame)
	constexpr float kAnimBlendFrame = 8.0f;
	// �A�j���[�V�����̍Đ����x(60f�Ȃ�1.0�A30f�Ȃ�0.5������)
	constexpr float kAnimSpeed = 0.6f;
}

Animator::Animator() :
	_model(-1),
	_blendRate(0.0f)
{
}

Animator::~Animator()
{
	MV1DeleteModel(_model);
}

void Animator::Init(int model)
{
	assert(model >= 0 && "���f���n���h�����������Ȃ�");
	_model = model;
}

void Animator::Update()
{
	// �A�j���[�V�����̍Đ�
	UpdateAnim(FindAnimData(_blendingAnimName));
	// �����A�j���[�V�����łȂ�����
	// �Đ��I���t���O�������Ă��Ȃ��Ȃ�
	if (_currentAnimName != _blendingAnimName &&
		!FindAnimData(_currentAnimName).isEnd) {
		// �Đ�
		UpdateAnim(FindAnimData(_currentAnimName));
	}
	// �u�����h�䗦�X�V
	UpdateAnimBlendRate();

#ifdef _DEBUG	// ���������͑Ή����Ă��Ȃ�
	int y = 16 * 9;
	DrawFormatString(0, y, 0xffffff, L"ExistState = %s", _currentAnimName.c_str());
	y += 16;
	DrawFormatString(0, y, 0xffffff, L"BlendState = %s", _blendingAnimName.c_str());
	y += 16;
	DrawFormatString(0, y, 0xffffff, L"BlendRate = %.2f", _blendRate);
	y += 16;
	for (auto& data : _animDataList) {
		y += 16;
		DrawFormatString(0, y, 0xffffff, L"name:%s frame:%.2f", data.animName.c_str(), data.frame);
	}
#endif
}

void Animator::SetStartAnim(const std::wstring animName, const bool isLoop)
{
	AttachAnim(animName, isLoop);
	// _blendingAnim�݂̂��g�p���邽��
	_blendRate = 1.0f;
	_currentAnimName = _blendingAnimName;
}

void Animator::SetAnimData(const std::wstring animName, const bool isLoop)
{
	// ���łɓ����A�j���[�V�������o�^����Ă��Ȃ����m�F
	for (const auto& anim : _animDataList) {
		if (animName == anim.animName) {
			assert(false && "����̃A�j���[�V������o�^���悤�Ƃ��Ă���");
			return;
		}
	}
	AnimData animData;
	animData.animIndex = MV1GetAnimIndex(_model, animName.c_str());
	animData.attachNo = -1;		// ���ۂɎg���ۂɍX�V����
	animData.animName = animName;
	animData.frame = 0.0f;
	animData.totalFrame = MV1GetAnimTotalTime(_model, animData.animIndex);
	animData.isLoop = isLoop;
	animData.isEnd = false;

	//_animDataList.emplace_back(animData);
	_animDataList.emplace_front(animData);
}

void Animator::AttachAnim(const std::wstring animName, const bool isLoop)
{
	_blendingAnimName = animName;
	AnimData& animData = FindAnimData(_blendingAnimName);

	// ���f���ɃA�j���[�V�������A�^�b�`
	animData.attachNo = MV1AttachAnim(_model, animData.animIndex, -1, false);
	assert(animData.attachNo >= 0 && "�A�j���[�V�����̃A�^�b�`���s");
	animData.frame = 0.0f;
	animData.isLoop = isLoop;
	animData.isEnd = false;
	MV1SetAttachAnimBlendRate(_model, animData.attachNo, 0.0f);
}

void Animator::UpdateAnim(AnimData& data)
{
	// �A�j���[�V�������A�^�b�`����Ă��Ȃ��ꍇreturn
	if (data.attachNo == -1) return;
	// �A�j���[�V������i�߂�
	data.frame += kAnimSpeed;

	// ���ݍĐ����̃A�j���[�V�����̑����Ԃ��擾����
	const float totalTime = data.totalFrame;
	
	// �A�j���[�V�����̐ݒ�ɂ���ă��[�v�����邩�Ō�̃t���[���Ŏ~�߂邩�𔻒�
	if (data.isLoop)
	{
		// �A�j���[�V���������[�v������
		while (data.frame > totalTime)
		{
			data.frame -= totalTime;
		}
	}
	else
	{
		// �Ō�̃t���[���Œ�~������
		if (data.frame > totalTime)
		{
			data.frame = totalTime;
			data.isEnd = true;
		}
	}

	// �i�s�������A�j���[�V���������f���ɓK�p����
	MV1SetAttachAnimTime(_model, data.attachNo, data.frame);
}

void Animator::UpdateAnimBlendRate()
{
	// �����ɃA�j���[�V�������ݒ肳��Ă��Ȃ��ꍇ�͕ω������Ȃ�
	if (FindAnimData(_blendingAnimName).attachNo == -1) return;
	if (FindAnimData(_currentAnimName).attachNo == -1) return;

	// _blendRate��0.0f -> 1.0f�ɕω�������
	_blendRate += 1.0f / kAnimBlendFrame;
	
	// �u�����h���I��������
	if (_blendRate >= 1.0f) {
		// ���݂̃A�j���[�V�������u�����h���̃A�j���[�V�����ɒu��������
		//_currentAnimName = _blendingAnimName;

		_blendRate = 1.0f;
	}

	MV1SetAttachAnimBlendRate(_model, FindAnimData(_currentAnimName).attachNo, 1.0f - _blendRate);
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_blendingAnimName).attachNo, _blendRate);
}

void Animator::ChangeAnim(const std::wstring animName, bool isLoop = false)
{
	// ���łɖړI�̃A�j���[�V�����փu�����h���Ȃ�return
	if (animName == _blendingAnimName) return;

	// �u�����h�Ɏg�p���Ȃ��Â��A�j���[�V�����̏�����
	FindAnimData(_currentAnimName).frame = 0.0f;
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_currentAnimName).attachNo, 0.0f);
	
	// ���݃��C���ōĐ����̃A�j���[�V������؂�ւ���
	_currentAnimName = _blendingAnimName;
	_blendingAnimName = animName;

	// �V���ɃA�j���[�V�������A�^�b�`����
	AttachAnim(_blendingAnimName, isLoop);

	// �u�����h�䗦������
	_blendRate = 0.0f;

	// �u�����h�䗦���A�j���[�V�����ɓK�p
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_currentAnimName).attachNo, 1.0f - _blendRate);
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_blendingAnimName).attachNo, _blendRate);
}

Animator::AnimData& Animator::FindAnimData(const std::wstring animName)
{
	// �A�j���[�V�������Ō���
	for (auto& data : _animDataList) {
		if (animName == data.animName) {
			return data;
		}
	}

	assert(false && "�w��̖��O�̃A�j���[�V�������o�^����Ă��Ȃ�����");
	AnimData animData;
	return animData;
}
