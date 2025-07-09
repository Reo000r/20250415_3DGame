#include "SkyDome.h"
#include "Camera.h"

#include <DxLib.h>
#include <cassert>

Skydome::Skydome() :
	_handle(-1),
	_camera()
{
}

Skydome::~Skydome()
{
	MV1DeleteModel(_handle);
}

void Skydome::Init(std::weak_ptr<Camera> camera)
{
	_camera = camera;

	// �X�J�C�h�[���ǂݍ���
	_handle = MV1LoadModel(L"data/skydome/Sky_Daylight01.mv1");
	assert(_handle >= 0);
	// ����
	MV1SetScale(_handle, VGet(5.0f, 5.0f, 5.0f));
	MV1SetPosition(_handle, _camera.lock()->GetPos());
}

void Skydome::Update()
{
	MV1SetPosition(_handle, _camera.lock()->GetPos());
}

void Skydome::Draw()
{
	MV1DrawModel(_handle);
}
