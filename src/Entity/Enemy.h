#pragma once

#include <flecs.h>


namespace game
{

struct EnemyDataSingleton
{

	int waveInProgressDebug;
	float currentWaveDurationDebug;
	float currentSpawnDurationDebug;

};

struct EnemyTag {};
struct EnemyPrefab {};

namespace enemy
{

//TODO: get all this in a file and read from it
const int ENEMY_WIDTH = 16;
const int ENEMY_HEIGHT = 16;
const float ENEMY_SPEED = 150.0f;
const int ENEMY_MAX_HEALTH = 100;
const float PAUSE_DURATION = 5.0f;
const float WAVE_DURATION = 2.0f;
const float ENEMIES_SPAWN_INTERVAL = 0.5f;

void InitEnemy(flecs::world& ecs);

}// namespace enemy	
}// namespace game
