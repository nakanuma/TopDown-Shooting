#include "GameOverState.h"

// Application
#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/Transition/FadeTransition.h>

GameOverState::GameOverState(GamePlayScene* scene) { scene_ = scene; }

void GameOverState::Initialize() {
	gameOverSequence_ = std::make_unique<GameOverSequence>();
	gameOverSequence_->Initialize(scene_->GetSpriteCommon());
	gameOverSequence_->SetPlayer(scene_->GetPlayer());

	gameOverSequence_->Start(scene_->GetPlayer()->GetTranslate()); // 初期化時に演出開始
}

void GameOverState::Finalize() { scene_->GetEnemyManager()->Finalize(); }

void GameOverState::Update() {
	gameOverSequence_->Update();

	scene_->GetEnemyManager()->Update();

	scene_->GetField()->Update();
	scene_->GetObstacleManager()->Update(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Update();

	FadeTransition::GetInstance()->Update();
}

void GameOverState::Draw() {
	scene_->GetEnemyManager()->Draw();

	scene_->GetField()->Draw();
	scene_->GetObstacleManager()->Draw(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Draw();
}

void GameOverState::DrawShadow() {
	scene_->GetEnemyManager()->DrawShadow();

	scene_->GetObstacleManager()->DrawShadow(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->DrawShadow();
}

void GameOverState::DrawShadowSkinning() { 
	scene_->GetEnemyManager()->DrawShadowSkinning(); 
}

void GameOverState::DrawUI() {
	gameOverSequence_->DrawUI();

	FadeTransition::GetInstance()->Draw();
}

void GameOverState::Debug() {
#ifdef _DEBUG
	gameOverSequence_->Debug();
#endif
}

bool GameOverState::CanTransition() const {
	return false; // 遷移先は無いのでfalse
}

bool GameOverState::IsPlayerDead() const { return scene_->GetPlayer()->IsDead(); }

bool GameOverState::IsBossDying() const { return scene_->GetEnemyManager()->GetBoss()->IsDying(); }

bool GameOverState::ShouldShowBossIntro() const { return false; }
