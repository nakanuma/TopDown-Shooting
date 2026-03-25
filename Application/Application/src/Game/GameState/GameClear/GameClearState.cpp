#include "GameClearState.h"

// Application
#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Transition/FadeTransition.h>

GameClearState::GameClearState(GamePlayScene* scene) {
	scene_ = scene;
}

void GameClearState::Initialize() {
	gameClearSequence_ = std::make_unique<GameClearSequence>();
	gameClearSequence_->Initialize(scene_->GetSpriteCommon());
	gameClearSequence_->SetBoss(scene_->GetEnemyManager()->GetBoss());

	gameClearSequence_->Start(); // 初期化時に演出開始

	scene_->GetPlayer()->SetInvincible(true); // クリア時にはプレイヤーを無敵状態にする
}

void GameClearState::Finalize() { scene_->GetEnemyManager()->Finalize(); }

void GameClearState::Update() { 
	gameClearSequence_->Update();


	// ゲームクリア演出制御クラスがカメラ操作をしていない場合には、通常ゲーム用のカメラ制御を行う
	if (!gameClearSequence_->IsControllingCamera()) {
		// 追従カメラの更新
		scene_->GetFollowCamera()->Update();
		// 追従カメラ + カメラシェイクを現在カメラに適用
		scene_->GetCamera()->transform_.translate_ = scene_->GetFollowCamera()->GetCameraPosition() + CameraShake::GetInstance()->GetOffset();
	}

	// ゲームクリアが演出したらゴールテレポーターを有効化
	if (gameClearSequence_->IsFinished()) {
		scene_->GetTeleportManager()->EnableGoalTeleporter();
	}


	scene_->GetPlayer()->Update(true); // プレイヤーは操作可能
	scene_->GetEnemyManager()->Update();

	scene_->GetField()->Update();
	scene_->GetObstacleManager()->Update(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Update();

	BulletManager::GetInstance()->Update();

	FadeTransition::GetInstance()->Update();
}

void GameClearState::Draw() {
	scene_->GetPlayer()->Draw();
	scene_->GetEnemyManager()->Draw();

	scene_->GetField()->Draw();
	scene_->GetObstacleManager()->Draw(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Draw();

	BulletManager::GetInstance()->Draw();
}

void GameClearState::DrawShadow() {
	scene_->GetPlayer()->DrawShadow();
	scene_->GetEnemyManager()->DrawShadow();

	scene_->GetObstacleManager()->DrawShadow(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->DrawShadow();
}

void GameClearState::DrawShadowSkinning() {
	scene_->GetPlayer()->DrawShadowSkinning();
	scene_->GetEnemyManager()->DrawShadowSkinning();
}

void GameClearState::DrawUI() { 
	gameClearSequence_->DrawUI(); 

	if (gameClearSequence_->IsFinished()) {
		scene_->GetPlayer()->DrawUI();
	}

	FadeTransition::GetInstance()->Draw();
}

void GameClearState::Debug() { 
	gameClearSequence_->Debug(); 
}

bool GameClearState::CanTransition() const { 
	return false; // 遷移先は無いのでfalse
}

bool GameClearState::IsPlayerDead() const { return scene_->GetPlayer()->IsDead(); }

bool GameClearState::IsBossDying() const { return scene_->GetEnemyManager()->GetBoss()->IsDying(); }

bool GameClearState::ShouldShowBossIntro() const { return false; }
