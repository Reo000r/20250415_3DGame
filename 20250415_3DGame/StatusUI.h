#pragma once
#include "Vector3.h"
#include <memory>
#include <map>

class Player;
class WaveManager;
class EnemyBase;

class StatusUI
{
public:
	StatusUI();
	~StatusUI();

	void Init(std::weak_ptr<Player> player, std::weak_ptr<WaveManager> waveManager) ;
	void Update();
	void Draw();

private:

	void DrawPlayerHp();
	void DrawEnemyHp(std::shared_ptr<EnemyBase> enemy);
	void DrawScore();

	std::weak_ptr<Player> _player;
	std::weak_ptr<WaveManager> _waveManager;

	int _scoreFontHandle;
};

