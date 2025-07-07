#include "Animator.h"

#include <DxLib.h>
#include <cassert>

namespace {
	// アニメーションのブレンド時間(frame)
	constexpr float kAnimBlendFrame = 8.0f;
	// アニメーションの再生速度(60fなら1.0、30fなら0.5が等速)
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
	assert(model >= 0 && "モデルハンドルが正しくない");
	_model = model;
}

void Animator::Update()
{
	// アニメーションの再生
	UpdateAnim(FindAnimData(_blendingAnimName));
	// 同じアニメーションでないかつ
	// 再生終了フラグが立っていないなら
	if (_currentAnimName != _blendingAnimName &&
		!FindAnimData(_currentAnimName).isEnd) {
		// 再生
		UpdateAnim(FindAnimData(_currentAnimName));
	}
	// ブレンド比率更新
	UpdateAnimBlendRate();

#ifdef _DEBUG	// 複数所持は対応していない
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
	// _blendingAnimのみを使用するため
	_blendRate = 1.0f;
	_currentAnimName = _blendingAnimName;
}

void Animator::SetAnimData(const std::wstring animName, const bool isLoop)
{
	// すでに同じアニメーションが登録されていないか確認
	for (const auto& anim : _animDataList) {
		if (animName == anim.animName) {
			assert(false && "同一のアニメーションを登録しようとしている");
			return;
		}
	}
	AnimData animData;
	animData.animIndex = MV1GetAnimIndex(_model, animName.c_str());
	animData.attachNo = -1;		// 実際に使う際に更新する
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

	// モデルにアニメーションをアタッチ
	animData.attachNo = MV1AttachAnim(_model, animData.animIndex, -1, false);
	assert(animData.attachNo >= 0 && "アニメーションのアタッチ失敗");
	animData.frame = 0.0f;
	animData.isLoop = isLoop;
	animData.isEnd = false;
	MV1SetAttachAnimBlendRate(_model, animData.attachNo, 0.0f);
}

void Animator::UpdateAnim(AnimData& data)
{
	// アニメーションがアタッチされていない場合return
	if (data.attachNo == -1) return;
	// アニメーションを進める
	data.frame += kAnimSpeed;

	// 現在再生中のアニメーションの総時間を取得する
	const float totalTime = data.totalFrame;
	
	// アニメーションの設定によってループさせるか最後のフレームで止めるかを判定
	if (data.isLoop)
	{
		// アニメーションをループさせる
		while (data.frame > totalTime)
		{
			data.frame -= totalTime;
		}
	}
	else
	{
		// 最後のフレームで停止させる
		if (data.frame > totalTime)
		{
			data.frame = totalTime;
			data.isEnd = true;
		}
	}

	// 進行させたアニメーションをモデルに適用する
	MV1SetAttachAnimTime(_model, data.attachNo, data.frame);
}

void Animator::UpdateAnimBlendRate()
{
	// 両方にアニメーションが設定されていない場合は変化させない
	if (FindAnimData(_blendingAnimName).attachNo == -1) return;
	if (FindAnimData(_currentAnimName).attachNo == -1) return;

	// _blendRateを0.0f -> 1.0fに変化させる
	_blendRate += 1.0f / kAnimBlendFrame;
	
	// ブレンドが終了したら
	if (_blendRate >= 1.0f) {
		// 現在のアニメーションをブレンド中のアニメーションに置き換える
		//_currentAnimName = _blendingAnimName;

		_blendRate = 1.0f;
	}

	MV1SetAttachAnimBlendRate(_model, FindAnimData(_currentAnimName).attachNo, 1.0f - _blendRate);
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_blendingAnimName).attachNo, _blendRate);
}

void Animator::ChangeAnim(const std::wstring animName, bool isLoop = false)
{
	// すでに目的のアニメーションへブレンド中ならreturn
	if (animName == _blendingAnimName) return;

	// ブレンドに使用しない古いアニメーションの初期化
	FindAnimData(_currentAnimName).frame = 0.0f;
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_currentAnimName).attachNo, 0.0f);
	
	// 現在メインで再生中のアニメーションを切り替える
	_currentAnimName = _blendingAnimName;
	_blendingAnimName = animName;

	// 新たにアニメーションをアタッチする
	AttachAnim(_blendingAnimName, isLoop);

	// ブレンド比率初期化
	_blendRate = 0.0f;

	// ブレンド比率をアニメーションに適用
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_currentAnimName).attachNo, 1.0f - _blendRate);
	MV1SetAttachAnimBlendRate(_model, FindAnimData(_blendingAnimName).attachNo, _blendRate);
}

Animator::AnimData& Animator::FindAnimData(const std::wstring animName)
{
	// アニメーション名で検索
	for (auto& data : _animDataList) {
		if (animName == data.animName) {
			return data;
		}
	}

	assert(false && "指定の名前のアニメーションが登録されていなかった");
	AnimData animData;
	return animData;
}
