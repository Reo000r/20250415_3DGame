#include "SceneGamePlay.h"
#include "SceneResult.h"
#include "SceneController.h"
#include "Camera.h"
#include "Player.h"
#include "Ground.h"
#include "Skydome.h"
#include "Physics.h"
#include "DebugDraw.h"
#include "WaveManager.h"
#include "EnemyManager.h"
#include "WaveAnnouncer.h"
#include "EnemyFactory.h"
#include "StatusUI.h"
#include "GameManager.h"

#include "Statistics.h"
#include "Input.h"

#include <memory>
#include <DxLib.h>
#include <cassert>

SceneGamePlay::SceneGamePlay() :
	_frame(Statistics::kFadeInterval),
	_nextScene(std::make_shared<SceneResult>()),
	_physics(std::make_shared<Physics>()),
	_camera(std::make_shared<Camera>()),
	_player(std::make_shared<Player>()),
	_ground(std::make_shared<Ground>()),
	_skydome(std::make_unique<Skydome>()),
	_waveManager(std::make_shared<WaveManager>()),
	_enemyManager(std::make_shared<EnemyManager>()),
	_waveAnnouncer(std::make_shared<WaveAnnouncer>()),
	_statusUI(std::make_unique<StatusUI>()),
	_nowUpdateState(&SceneGamePlay::FadeinUpdate),
	_nowDrawState(&SceneGamePlay::FadeDraw)
{
}

SceneGamePlay::~SceneGamePlay()
{
	// 敵のモデルを解放する
	EnemyFactory::ReleaseResources();
}

void SceneGamePlay::Init()
{
	// 敵のモデルを読み込む
	EnemyFactory::LoadResources();

	// 初期化処理
	_camera->Init(_player);
	_player->Init(_camera, _physics);
	_skydome->Init(_camera);

	_enemyManager->Init(_player, _physics);
	_waveManager->Init(_enemyManager, _waveAnnouncer);
	_statusUI->Init(_player, _waveManager, _enemyManager);

	GameManager::GetInstance().Init(_player, _waveManager);
}

void SceneGamePlay::Update()
{
	(this->*_nowUpdateState)();
}

void SceneGamePlay::Draw()
{
	(this->*_nowDrawState)();
}

void SceneGamePlay::FadeinUpdate()
{
	_frame--;

	// フェードが終わったらシーン遷移
	if (_frame <= 0) {
		_nowUpdateState = &SceneGamePlay::NormalUpdate;
		_nowDrawState = &SceneGamePlay::NormalDraw;
	}
}

void SceneGamePlay::NormalUpdate()
{
	// 更新
	_camera->Update();
	_skydome->Update();

	_player->Update();

	_statusUI->Update();
	_enemyManager->Update();
	_waveAnnouncer->Update();
	_waveManager->Update();

	// 物理演算更新
	_physics->Update();

	// クリア条件を満たしたら
	if (_waveManager->IsClear()
#ifdef _DEBUG
		|| Input::GetInstance().IsTrigger("next")	// 決定を押したら
#endif // _DEBUG
		) {
		_nowUpdateState = &SceneGamePlay::FadeoutUpdate;
		_nowDrawState = &SceneGamePlay::FadeDraw;
		_frame = 0;
	}
}

void SceneGamePlay::FadeoutUpdate()
{
	_frame++;

	if (_frame >= Statistics::kFadeInterval) {
		SceneController::GetInstance().ChangeScene(_nextScene);
		return;  // 自分が死んでいるのでもし
		// 余計な処理が入っているとまずいのでreturn;
	}
}

void SceneGamePlay::FadeDraw()
{
	_skydome->Draw();
	_ground->Draw();

	_camera->Draw();
	_player->Draw();

	_enemyManager->Draw();
	_statusUI->Draw();
	_waveAnnouncer->Draw();

#ifdef _DEBUG
	DebugDraw::GetInstance().Draw();
	DebugDraw::GetInstance().Clear();

	DrawFormatString(0, 0, 0xffffff, L"Scene GamePlay");
#endif

	// フェードイン/アウトの処理
	// フェード割合の計算(0.0-1.0)
	float rate = static_cast<float>(_frame) / static_cast<float>(Statistics::kFadeInterval);
	SetDrawBlendMode(DX_BLENDMODE_MULA, static_cast<int>(255 * rate));
	DrawBox(0, 0, Statistics::kScreenWidth, Statistics::kScreenHeight, 0x000000, true);
	// BlendModeを使った後はNOBLENDにしておくことを忘れず
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}

void SceneGamePlay::NormalDraw()
{
	_skydome->Draw();
	_ground->Draw();
	
	_camera->Draw();
	_player->Draw();

	_enemyManager->Draw();
	_statusUI->Draw();
	_waveAnnouncer->Draw();

#ifdef _DEBUG
	DebugDraw::GetInstance().Draw();
	DebugDraw::GetInstance().Clear();

	DrawFormatString(0, 0, 0xffffff, L"Scene GamePlay");
#endif
}