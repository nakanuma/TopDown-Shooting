#include "GamePlayState.h"

// Engine
#include <Camera.h>

// Application
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/System/ResultStats.h>

GamePlayState::GamePlayState(GamePlayScene* scene) { scene_ = scene; }

void GamePlayState::Initialize() {}

void GamePlayState::Update() {
	// プレイヤーが生きている場合、通常ゲーム用のカメラ制御を行う
	if (!scene_->GetPlayer()->IsDead()) {
		// 追従カメラの更新
		scene_->GetFollowCamera()->Update();
		// 追従カメラ + カメラシェイクを現在カメラに適用
		scene_->GetCamera()->transform_.translate_ = scene_->GetFollowCamera()->GetCameraPosition() + CameraShake::GetInstance()->GetOffset();
	}


	// プレイヤーが死亡したらゲームオーバー状態遷移フラグを立てる
	if (scene_->GetPlayer()->IsDead()) {
		shouldTransitionToGameOver_ = true;
	}
	// ボスが死亡したらゲームクリア状態遷移フラグを立てる
	if (scene_->GetEnemyManager()->GetBoss()->IsDying()) {
		shouldTransitionToGameClear_ = true;
	}


	scene_->GetPlayer()->Update(true); // プレイヤーは操作可能
	scene_->GetEnemyManager()->Update();

	scene_->GetField()->Update();
	scene_->GetObstacleManager()->Update(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Update();

	BulletManager::GetInstance()->Update();

	// クリアタイム（経過時間）の記録
	ResultStats::GetInstance()->AddTime();
}

void GamePlayState::Draw() {
	scene_->GetPlayer()->Draw();
	scene_->GetEnemyManager()->Draw();

	scene_->GetField()->Draw();
	scene_->GetObstacleManager()->Draw(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Draw();

	BulletManager::GetInstance()->Draw();
}

void GamePlayState::DrawShadow() {
	scene_->GetPlayer()->DrawShadow();
	scene_->GetEnemyManager()->DrawShadow();

	scene_->GetObstacleManager()->DrawShadow(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->DrawShadow();
}

void GamePlayState::DrawShadowSkinning() { scene_->GetPlayer()->DrawShadowSkinning(); }

void GamePlayState::DrawUI() {
	scene_->GetPlayer()->DrawUI();
	scene_->GetEnemyManager()->DrawUI();
}

void GamePlayState::Debug() {}

bool GamePlayState::CanTransition() const {
	// ゲームオーバーまたはゲームクリアへの遷移条件
	return shouldTransitionToGameOver_ || shouldTransitionToGameClear_;
}

bool GamePlayState::IsPlayerDead() const { return scene_->GetPlayer()->IsDead(); }

bool GamePlayState::IsBossDying() const { return scene_->GetEnemyManager()->GetBoss()->IsDying(); }
