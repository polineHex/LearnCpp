#include "DebugWindow.h"

#include <imgui.h>
#include <raylib.h>

#include "Globals.h"

#include "Entity/Enemy.h"

namespace game::Rendering
{

void DebugWindow::Draw(flecs::world& ecs)
{
	const EnemyDataSingleton* enemyData = ecs.get<EnemyDataSingleton>();
	ImGui::Text("WAVE_DURATION and PAUSE_DURATION = 2 and 5");
	ImGui::Text("Time in progress = %f ", GetTime());
	ImGui::Text("Wave in progress = %i ", enemyData->waveInProgressDebug);
	ImGui::Text("Time from last wave ended = %f ", enemyData->currentWaveDurationDebug);
	ImGui::Text("Time from last spawn = %f ", enemyData->currentSpawnDurationDebug);

	
	
	if (ImGui::RadioButton("60", &gFps, 60))
		SetTargetFPS(gFps);
	ImGui::SameLine();

	if (ImGui::RadioButton("144", &gFps, 144))
		SetTargetFPS(gFps);
	ImGui::SameLine();

	if (ImGui::RadioButton("Unlimited", &gFps, -1))
		SetTargetFPS(gFps);

}

} // namespace game::Rendering