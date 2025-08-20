#include "ItemFactory.h"
#include "ItemBase.h"
#include "ItemHeal.h"
#include "Player.h"
#include "Calculation.h"

#include <DxLib.h>
#include <cassert>
#include <string>

namespace {
	// ���f���t�@�C���̃p�X�������ŊǗ�
	const std::unordered_map<ItemType, std::wstring> kModelPaths = {
		{ ItemType::Heal,		L"data/model/item/Item_BottleA.mv1" },
		{ ItemType::ScoreBoost, L"data/model/item/Item_Coin.mv1" },
		{ ItemType::Strength,	L"data/model/item/Item_Food.mv1" },
	};
	
	// ���ʒ萔
	constexpr float kItemScale = 4.0f;
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
	const float kHealAmount = (Player::GetMaxHitPoint() * 0.4f) / kHealTotalFrame;	// f������̉񕜗�

	// �U���͑����p
	const Vector3 kModelStrengthTransOffset = Vector3(0, -kColRadius, 0);
	const float kStrengthTotalFrame = 60.0f * 10.0f;		// ���ʎ���
	constexpr float kStrengthMulAmount = 1.5f;			// �U���͑����{��
}

// static�����o�[�ϐ��̎��̂��`
std::unordered_map<ItemType, int> ItemFactory::_modelHandles;

void ItemFactory::LoadResources()
{
	// �S�Ẵ��f����ǂݍ��݁A�n���h����ۑ�����
	for (const auto& pair : kModelPaths) {
		const ItemType& type = pair.first;
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
	ItemType type, 
	const Vector3& position, 
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
	case ItemType::Heal:
	{
		auto itemHeal = std::make_shared<ItemHeal>(duplicatedHandle);
		// �X�e�[�^�X�ݒ�
		BuffStats stats;
		stats.activeFrame = kHealTotalFrame;
		stats.amount = kHealAmount;
		stats.isActive = true;

		// �h�����Init���Ăяo��
		itemHeal->Init(kModelHealTransOffset, stats);
		newItem = itemHeal;
		newItem->Init(
			kColRadius,			// �����蔻�蔼�a
			kColTransOffset,	// �ʒu�␳
			kModelSize,			// �g�k�␳
			kModelAngle);		// �p�x�␳
		break;
	}
	case ItemType::ScoreBoost:
	{
		//newItem = std::make_shared<ItemScoreBoost>(duplicatedHandle);
		break;
	}
	case ItemType::Strength:
	{
		//newItem = std::make_shared<ItemStrength>(duplicatedHandle);
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
