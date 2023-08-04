#include "DebugWindow.h"

#include <imgui.h>
#include <raylib.h>

#include "Globals.h"

#include "Entity/Enemy.h"

namespace game::Rendering
{

void DebugWindow::Draw()
{
	ImGui::Text("WAVE_DURATION and PAUSE_DURATION = 2 and 5");
	ImGui::Text("Time in progress = %f ", GetTime());
	ImGui::Text("Wave in progress = %f ", gWaveInProgressDebug);
	ImGui::Text("Time from last wave ended = %f ", gCurrentWaveDurationDebug);
	ImGui::Text("Time from last spawn = %f ", gCurrenSpawnDurationDebug);

	
	
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