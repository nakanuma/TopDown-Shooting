#include "GameStartState.h"

// Application
#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/Transition/SplitBlockTransition.h>

GameStartState::GameStartState(GamePlayScene* scene) { scene_ = scene; }

void GameStartState::Initialize() {
	gameStartSequence_ = std::make_unique<GameStartSequence>();
	gameStartSequence_->Initialize(scene_->GetSpriteCommon());
}

void GameStartState::Finalize() { scene_->GetEnemyManager()->Finalize(); }

void GameStartState::Update() {
	gameStartSequence_->Update();

	scene_->GetPlayer()->Update(false); // プレイヤーは操作不可
	scene_->GetEnemyManager()->Update();

	scene_->GetField()->Update();
	scene_->GetObstacleManager()->Update(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Update();

	SplitBlockTransition::GetInstance()->Update();
}

void GameStartState::Draw() {
	gameStartSequence_->Draw();

	scene_->GetPlayer()->Draw();
	scene_->GetEnemyManager()->Draw();

	scene_->GetField()->Draw();
	scene_->GetObstacleManager()->Draw(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Draw();
}

void GameStartState::DrawShadow() {
	gameStartSequence_->DrawShadow();

	scene_->GetPlayer()->DrawShadow();
	scene_->GetEnemyManager()->DrawShadow();

	scene_->GetObstacleManager()->DrawShadow(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->DrawShadow();
}

void GameStartState::DrawShadowSkinning() { 
	scene_->GetPlayer()->DrawShadowSkinning(); 
	scene_->GetEnemyManager()->DrawShadowSkinning();
}

void GameStartState::DrawUI() {
	scene_->GetEnemyManager()->DrawUI();

	gameStartSequence_->DrawUI();

	SplitBlockTransition::GetInstance()->Draw();
}

void GameStartState::Debug() {
#ifdef _DEBUG
	gameStartSequence_->Debug();
#endif
}

bool GameStartState::CanTransition() const {
	// ゲーム開始演出が終了したら遷移可能
	return gameStartSequence_->IsFinished();
}

bool GameStartState::IsPlayerDead() const { return scene_->GetPlayer()->IsDead(); }

bool GameStartState::IsBossDying() const { return scene_->GetEnemyManager()->GetBoss()->IsDying(); }
