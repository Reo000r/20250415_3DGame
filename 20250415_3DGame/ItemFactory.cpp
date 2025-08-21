#include "ItemFactory.h"
#include "ItemBase.h"
#include "ItemHeal.h"
#include "ItemScoreBoost.h"
#include "ItemStrength.h"
#include "Player.h"
#include "PlayerBuffManager.h"
#include "Calculation.h"

#include <DxLib.h>
#include <cassert>
#include <string>

namespace {
	// ���f���t�@�C���̃p�X�������ŊǗ�
	const std::unordered_map<BuffType, std::wstring> kModelPaths = {
		{ BuffType::Heal,		L"data/model/item/Item_BottleA.mv1" },
		{ BuffType::ScoreBoost, L"data/model/item/Item_Coin.mv1" },
		{ BuffType::Strength,	L"data/model/item/Item_Food.mv1" },
	};
	
	// ���ʒ萔
	constexpr float kItemScale = 2.0f;
	const float kColRadius = 50.0f * kItemScale;				// �����蔻��̔��a
	const float kSpawnDepthY = -kColRadius - (10.0f * kItemScale);// �������̐[��
	const float kTotalAnimFrame = 60.0f * 0.5f;					// ����/���ŃA�j���[�V�����t���[��
	const float kModelRotSpeed = Calc::ToRadian(2.0f);			// ��]���x
	const Vector3 kColTransOffset = Vector3(0, kColRadius, 0);
	const Vector3 kModelSize = Vector3(1, 1, 1) * kItemScale;
	const Vector3 kModelAngle = Vector3(Calc::ToRadian(30.0f), 0, 0);

	// �q�[���p
	const Vector3 kModelHealTransOffset = Vector3(0, -kColRadius, 0);
	const float kHealTotalFrame = 60.0f * 1.0f;					// ���ʎ���
	const float kHealAmount = (Player::GetMaxHitPoint() * 0.4f) / kHealTotalFrame;	// 1f������̉񕜗�
	
	// �X�R�A�����p
	const Vector3 kModelScoreBoostTransOffset = Vector3(0, -kColRadius, 0);
	const float kScoreBoostTotalFrame = 60.0f * 10.0f;	// ���ʎ���
	const float kScoreBoostMulAmount = 1.5f;			// �X�R�A�����{��

	// �U���͑����p
	const Vector3 kModelStrengthTransOffset = Vector3(0, -kColRadius, 0);
	const float kStrengthTotalFrame = 60.0f * 10.0f;	// ���ʎ���
	constexpr float kStrengthMulAmount = 1.5f;			// �U���͑����{��
}

// static�����o�[�ϐ��̎��̂��`
std::unordered_map<BuffType, int> ItemFactory::_modelHandles;

void ItemFactory::LoadResources()
{
	// �S�Ẵ��f����ǂݍ��݁A�n���h����ۑ�����
	for (const auto& pair : kModelPaths) {
		const BuffType& type = pair.first;
		const std::wstring& path = pair.second;
		int handle = MV1LoadModel(path.c_str());
		assert(handle >= 0 && "���f���̓ǂݍ��݂Ɏ��s");
		_modelHandles[type] = handle;
	}
}

void ItemFactory::ReleaseResources()
{
	// �ۑ�����Ă���S�Ẵ��f���n���h�����������
	for (const auto& pair : _modelHandles) {
		MV1DeleteModel(pair.second);
	}
	_modelHandles.clear();
}

std::shared_ptr<ItemBase> ItemFactory::CreateAndRegister(
	BuffType type,
	const Vector3& position, 
	std::weak_ptr<PlayerBuffManager> manager,
	std::weak_ptr<Physics> physics)
{
	std::shared_ptr<ItemBase> newItem = nullptr;

	// �������̃��f���n���h�������݂��邩�`�F�b�N
	auto it = _modelHandles.find(type);
	assert(it != _modelHandles.end() && "�v�����ꂽ�^�C�v�̃��f�����ǂݍ��܂�Ă��Ȃ�");

	// ���f���𕡐����ăn���h�����擾
	// (second��int�^�A���f���n���h�����ۑ�����Ă���)
	int duplicatedHandle = MV1DuplicateModel(it->second);
	assert(duplicatedHandle != -1 && "���f���̕����Ɏ��s");

	// ��ނɉ����Đ�������
	switch (type) {
	case BuffType::Heal:
	{
		// �X�e�[�^�X�ݒ�
		BuffData data;
		data.type = type;
		data.activeFrame = kHealTotalFrame;
		data.amount = kHealAmount;
		data.isActive = true;
		auto itemHeal = std::make_shared<ItemHeal>(data, duplicatedHandle, manager);
		
		// �h�����Init���Ăяo��
		itemHeal->Init(kModelHealTransOffset);
		newItem = itemHeal;
		newItem->Init(
			kColRadius,			// �����蔻�蔼�a
			kColTransOffset,	// �ʒu�␳
			kModelSize,			// �g�k�␳
			kModelAngle);		// �p�x�␳
		break;
	}
	case BuffType::ScoreBoost:
	{
		// �X�e�[�^�X�ݒ�
		BuffData data;
		data.type = type;
		data.activeFrame = kScoreBoostTotalFrame;
		data.amount = kScoreBoostMulAmount;
		data.isActive = true;
		auto itemScoreBoost = std::make_shared<ItemScoreBoost>(data, duplicatedHandle, manager);

		// �h�����Init���Ăяo��
		itemScoreBoost->Init(kModelScoreBoostTransOffset);
		newItem = itemScoreBoost;
		newItem->Init(
			kColRadius,			// �����蔻�蔼�a
			kColTransOffset,	// �ʒu�␳
			kModelSize,			// �g�k�␳
			kModelAngle);		// �p�x�␳
		break;
	}
	case BuffType::Strength:
	{
		// �X�e�[�^�X�ݒ�
		BuffData data;
		data.type = type;
		data.activeFrame = kStrengthTotalFrame;
		data.amount = kStrengthMulAmount;
		data.isActive = true;
		auto itemStrength = std::make_shared<ItemStrength>(data, duplicatedHandle, manager);

		// �h�����Init���Ăяo��
		itemStrength->Init(kModelStrengthTransOffset);
		newItem = itemStrength;
		newItem->Init(
			kColRadius,			// �����蔻�蔼�a
			kColTransOffset,	// �ʒu�␳
			kModelSize,			// �g�k�␳
			kModelAngle);		// �p�x�␳
		break;
	}
	default:
		assert(false && "�s���ȓG�^�C�v���w�肳�ꂽ");
		MV1DeleteModel(duplicatedHandle); // �s�v�ɂȂ����n���h�������
		return nullptr;
	}

	// ���������G�̏�����
	if (newItem) {
		// ��������
		newItem->Spawn(position, kSpawnDepthY, kTotalAnimFrame, kModelRotSpeed);
		// �����蔻��o�^
		newItem->EntryPhysics(physics);
	}
	else {
		// �����Ɏ��s�����ꍇ���n���h�������
		MV1DeleteModel(duplicatedHandle);
	}

	return newItem;
}

float ItemFactory::GetStrengthMulAmount()
{
	return kStrengthMulAmount;
}
