#include "StageManager.h"

// Application
#include <src/Game/Enemy/EnemyManager.h>
#include <src/Game/PowerGenerator/PowerGeneratorManager.h>
#include <src/Game/Teleporter/TeleporterManager.h>
#include <src/Game/Transition/FadeTransition.h>

void StageManager::Initialize(EnemyManager* enemyMng, PowerGeneratorManager* powerGeneratorMng, TeleporterManager* teleporterMng) {
	enemyManager_ = enemyMng;
	powerGeneratorManager_ = powerGeneratorMng;
	teleporterManager_ = teleporterMng;

	// ステージ開始/終了ロゴクラスの生成
	missionLogo_ = std::make_unique<MissionLogo>();
	missionLogo_->Initialize();
}

void StageManager::PrepareNextStage(int32_t floor, int32_t typeIndex, bool startLogoAnim) {
	currentFloor_ = floor; // 階層を設定
	isObjectiveCleared_ = false;

	// 受け取ったステージ目標を設定
	currentStageType_ = static_cast<StageType>(typeIndex);

	// ステージ開始ロゴのアニメーション開始（フェードが終了したら）
	if (startLogoAnim) {
		missionLogo_->Start(MissionLogo::AnimationState::StartMission, currentStageType_, 0.5f); // フェードと被らないよう遅延時間を設定
	}
}

void StageManager::Update() {
	// ステージ開始/終了ロゴクラスの更新（目標達成後にも更新を行うため最初に更新）
	missionLogo_->Update();

	// 目標達成済みなら更新スキップ
	if (isObjectiveCleared_) return;

	bool cleared = false;

	switch (currentStageType_) {
	case StageType::killAllEnemies:
		// 全ての敵が撃破されていたらクリア判定
		if (enemyManager_->IsAllEnemiesDead()) cleared = true;
		break;
	case StageType::DestroyAllGeneratos:
		// 全ての発電機が破壊されていたらクリア判定
		if (powerGeneratorManager_->IsAllDestroyed()) cleared = true;
		break;
	case StageType::BossBattle:
		// ボスが死亡したらクリア判定
		if (enemyManager_->GetBoss()->IsDying()) cleared = true;
		break;
	}

	// クリア判定
	if (cleared) {
		isObjectiveCleared_ = true; // 目標達成したことを記録
		teleporterManager_->EnableNextTeleporter(); // 次へのテレポーターを有効化

		// 目標達成ロゴのアニメーション開始
		missionLogo_->Start(MissionLogo::AnimationState::ObjectiveClear, currentStageType_);
	}
}

void StageManager::DrawUI() {
	// ステージ開始/終了ロゴクラスの描画
	missionLogo_->DrawUI();
}

void StageManager::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("StageManager");

	// 現在階層
	ImGui::Text("CurrentFloor : %d", currentFloor_);

	// ステージタイプ表示
	const char* typeStr = "Unknown";
	switch (currentStageType_) {
	case StageType::killAllEnemies: typeStr = "KillAllEnemies"; break;
	case StageType::DestroyAllGeneratos: typeStr = "DestroyAllGeneratos"; break;
	case StageType::BossBattle: typeStr = "BossBattle"; break;
	}
	ImGui::Text("CurrentStageType : %s", typeStr);

	// 目標達成フラグ
	ImGui::Checkbox("isObjectiveCleared", &isObjectiveCleared_);

	ImGui::End();
#endif
}