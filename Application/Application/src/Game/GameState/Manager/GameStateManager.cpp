#include "GameStateManager.h"

// C++
#include <cassert>

void GameStateManager::RegisterState(const std::string& name, std::unique_ptr<IGameState> state)
{
	states_[name] = std::move(state);
}

void GameStateManager::ChangeState(const std::string& name)
{
	// 状態が登録されているか確認
	auto it = states_.find(name);
	assert(it != states_.end());

	// 現在の状態を変更
	currentState_ = it->second.get();
	currentStateName_ = name;

	// 新しい状態を初期化
	currentState_->Initialize();
}

void GameStateManager::Update()
{
	if (currentState_) {
		currentState_->Update();
	}

	// 状態遷移処理（後で整理したい）

	// ゲーム開始->ゲームプレイ
	if (GetCurrentStateName() == "GameStart") {
		if (GetCurrentState()->CanTransition()) {
			ChangeState("GamePlay");
		}
	}

	// ゲームプレイ->ゲームオーバー

	// ゲームプレイ->ゲームクリア

}

void GameStateManager::Draw()
{
	if (currentState_) {
		currentState_->Draw();
	}
}

void GameStateManager::DrawShadow()
{
	if (currentState_) {
		currentState_->DrawShadow();
	}
}

void GameStateManager::DrawShadowSkinning()
{
	if (currentState_) {
		currentState_->DrawShadowSkinning();
	}
}

void GameStateManager::DrawUI()
{
	if (currentState_) {
		currentState_->DrawUI();
	}
}

void GameStateManager::Debug()
{
	if (currentState_) {
		currentState_->Debug();
	}
}