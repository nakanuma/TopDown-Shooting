#include "StageManager.h"

// Application
#include <src/Game/Enemy/EnemyManager.h>
#include <src/Game/PowerGenerator/PowerGeneratorManager.h>
#include <src/Game/Teleporter/TeleporterManager.h>

void StageManager::Initialize(EnemyManager* enemyMng, PowerGeneratorManager* powerGeneratorMng, TeleporterManager* teleporterMng) {
	enemyManager_ = enemyMng;
	powerGeneratorManager_ = powerGeneratorMng;
	teleporterManager_ = teleporterMng;
}

void StageManager::PrepareNextState(int32_t floor) {
	currentFloor_ = floor; // 階層を設定
	isObjectiveCleared_ = false;

	// 階層毎にタイプを設定
	if(currentFloor_ == 1) {
		currentStageType_ = StageType::killAllEnemies;
	} else if (currentFloor_ == 2) {
		currentStageType_ = StageType::DestroyAllGeneratos;
	} else if (currentFloor_ == 3) {
		currentStageType_ = StageType::BossBattle;
	}
}

void StageManager::Update() {
	// 目標達成済みなら更新スキップ
	if(isObjectiveCleared_) return;

	bool cleared = false;

	switch(currentStageType_) {
	case StageType::killAllEnemies:
		// 全ての敵が撃破されていたらクリア判定
		if(enemyManager_->IsAllEnemiesDead()) cleared = true;
		break;
	case StageType::DestroyAllGeneratos:
		// 全ての発電機が破壊されていたらクリア判定
		if(powerGeneratorManager_->IsAllDestroyed()) cleared = true;
		break;
	case StageType::BossBattle:
		// ボスが死亡したらクリア判定
		if(enemyManager_->GetBoss()->IsDying()) cleared = true;
		break;
	}

	if(cleared) {
		isObjectiveCleared_ = true; // 目標達成したことを記録
		teleporterManager_->EnableNextTeleporter(); // 次へのテレポーターを有効化
	}
}

void StageManager::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("StageManager");

	// 現在階層
	ImGui::Text("CurrentFloor : %d", currentFloor_);

	// ステージタイプ表示
	const char* typeStr = "Unknown";
	switch(currentStageType_) {
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