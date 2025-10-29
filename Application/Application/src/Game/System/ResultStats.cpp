#include "ResultStats.h"

// Engine
#include <ImguiWrapper.h>

ResultStats* ResultStats::GetInstance() {
	static ResultStats instance;
	return &instance;
}

void ResultStats::Clear() {
	// 全ての戦績をクリア
	totalShots_ = 0;
	hitShots_ = 0;
	totalDamage_ = 0;
	defeated_ = 0;
	clearTime_ = 0.0f;
}

float ResultStats::GetHitRate() const {
	if (totalShots_ == 0) {
		return 0.0f;
	}

	// 0~100で命中率を返すよう調整
	return static_cast<float>(hitShots_) / static_cast<float>(totalShots_) * 100.0f;
}

void ResultStats::Debug() {
	ImGui::Begin("ResultStats");

	ImGui::Text("totalShots : %d", totalShots_);
	ImGui::Text("hitShots : %d", hitShots_);
	ImGui::Text("totalDamage : %d", totalDamage_);
	ImGui::Text("defeated : %d", defeated_);
	ImGui::Text("clearTime : %.2f", clearTime_);

	ImGui::Text("hitRate : %.2f", GetHitRate());

	ImGui::End();
}