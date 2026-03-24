#include "GameStateManager.h"

// C++
#include <cassert>

// Application
#include <src/Game/Scene/GamePlayScene.h>

void GameStateManager::RegisterState(const std::string& name, std::unique_ptr<IGameState> state) { states_[name] = std::move(state); }

void GameStateManager::Finalize() { 
	if (currentState_) {
		currentState_->Finalize();
	}
}

void GameStateManager::ChangeState(const std::string& name) {
	// 状態が登録されているか確認
	auto it = states_.find(name);
	assert(it != states_.end());

	// 現在の状態を変更
	currentState_ = it->second.get();
	currentStateName_ = name;

	// 新しい状態を初期化
	currentState_->Initialize();
}

void GameStateManager::Update() {
	if (currentState_) {
		currentState_->Update();
	}

	// 状態遷移処理
	
	// ゲーム開始時の遷移
	if (GetCurrentStateName() == "GameStart") {
		// ゲーム開始 -> ゲームプレイ
		if (GetCurrentState()->CanTransition()) {
			ChangeState("GamePlay");
			scene_->GetStageManager()->ShowInitialMissionLogo(); // 初回のみこの関数によってステージ目標演出開始を行う
		}
	}

	// ゲームプレイ時の遷移
	if (GetCurrentStateName() == "GamePlay") {
		if (GetCurrentState()->CanTransition()) {
			// ゲームプレイ -> ゲームオーバー
			if (GetCurrentState()->IsPlayerDead()) {
				ChangeState("GameOver");
			}
			// ゲームプレイ -> ゲームクリア
			else if (GetCurrentState()->IsBossDying()) {
				ChangeState("GameClear");
			}
			// ゲームプレイ -> ボス登場演出
			else if (GetCurrentState()->ShouldShowBossIntro()) {
				ChangeState("BossIntro");
			}
		}
	}

	// ボス登場演出時の遷移
	if (GetCurrentStateName() == "BossIntro") {
		// ボス登場演出が終了していたら通常ゲームプレイに戻す
		if (GetCurrentState()->CanTransition()) {
			ChangeState("GamePlay");
		}
	}
}

void GameStateManager::Draw() {
	if (currentState_) {
		currentState_->Draw();
	}
}

void GameStateManager::DrawShadow() {
	if (currentState_) {
		currentState_->DrawShadow();
	}
}

void GameStateManager::DrawShadowSkinning() {
	if (currentState_) {
		currentState_->DrawShadowSkinning();
	}
}

void GameStateManager::DrawUI() {
	if (currentState_) {
		currentState_->DrawUI();
	}
}

void GameStateManager::Debug() {
#ifdef USE_IMGUI
	if (currentState_) {
		currentState_->Debug();
	}
#endif
}