#include "ResultStats.h"

// Engine
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// インスタンス取得
// ---------------------------------------------------------
ResultStats* ResultStats::GetInstance() {
	static ResultStats instance;
	return &instance;
}

// ---------------------------------------------------------
// 戦績をクリア
// ---------------------------------------------------------
void ResultStats::Clear() {
	totalShots_ = 0;
	hitShots_ = 0;
	totalDamage_ = 0;
	defeated_ = 0;
	clearTime_ = 0.0f;
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void ResultStats::Debug() {
	ImGui::Begin("ResultStats");

	ImGui::Text("totalShots : %d", totalShots_);
	ImGui::Text("hitShots : %d", hitShots_);
	ImGui::Text("totalDamage : %d", totalDamage_);
	ImGui::Text("defeated : %d", defeated_);
	ImGui::Text("clearTime : %.2f", clearTime_);

	ImGui::End();
}