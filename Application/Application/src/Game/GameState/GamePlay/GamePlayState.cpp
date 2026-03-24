#include "GamePlayState.h"

// Engine
#include <Camera.h>

// Application
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/System/ResultStats.h>
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Waypoint/WaypointManager.h>

GamePlayState::GamePlayState(GamePlayScene* scene) { scene_ = scene; }

void GamePlayState::Initialize() {
	// 全てのフラグをリセット
	shouldTransitionToGameOver_ = false;
	shouldTransitionToGameClear_ = false;
	shouldTransitionToBossInrto_ = false;
}

void GamePlayState::Finalize() { scene_->GetEnemyManager()->Finalize(); }

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
	auto boss = scene_->GetEnemyManager()->GetBoss();
	if (boss != nullptr) {
		if (boss->IsDying()) {
			shouldTransitionToGameClear_ = true;
		}
	}
	// ボス登場演出の開始
	// : まだ再生していないかつ、プレイヤーがイベントトリガーに触れた際
	if (!scene_->HasBossIntroPlayed() && scene_->GetEventManager()->CheckTrigger("BOSS_INTRO")) {
		shouldTransitionToBossInrto_ = true;
		scene_->SetBossIntroPlayed(true); // 既にボス登場演出を再生したことをゲームシーンに知らせる（再度判定防止用）
	}

	scene_->GetPlayer()->Update(true); // プレイヤーは操作可能
	scene_->GetEnemyManager()->Update();

	scene_->GetField()->Update();
	scene_->GetObstacleManager()->Update(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Update();
	scene_->GetPowerGeneratorManager()->Update();

	BulletManager::GetInstance()->Update();

	// クリアタイム（経過時間）の記録
	ResultStats::GetInstance()->AddTime();

	FadeTransition::GetInstance()->Update();
}

void GamePlayState::Draw() {
	scene_->GetPlayer()->Draw();
	scene_->GetEnemyManager()->Draw();

	scene_->GetField()->Draw();
	scene_->GetObstacleManager()->Draw(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->Draw();
	scene_->GetPowerGeneratorManager()->Draw();

	BulletManager::GetInstance()->Draw();
}

void GamePlayState::DrawShadow() {
	scene_->GetPlayer()->DrawShadow();
	scene_->GetEnemyManager()->DrawShadow();

	scene_->GetObstacleManager()->DrawShadow(scene_->GetPlayer()->GetTranslate());
	scene_->GetTeleportManager()->DrawShadow();
	scene_->GetPowerGeneratorManager()->DrawShadow();
}

void GamePlayState::DrawShadowSkinning() {
	scene_->GetPlayer()->DrawShadowSkinning();
	scene_->GetEnemyManager()->DrawShadowSkinning();
}

void GamePlayState::DrawUI() {
	scene_->GetPlayer()->DrawUI();
	scene_->GetEnemyManager()->DrawUI();
	scene_->GetPowerGeneratorManager()->DrawUI();

	FadeTransition::GetInstance()->Draw();
}

void GamePlayState::Debug() {
#ifdef USE_IMGUI
	scene_->GetEnemyManager()->Debug();

	scene_->GetPlayer()->Debug();

	scene_->GetPowerGeneratorManager()->Debug();
#endif
}

bool GamePlayState::CanTransition() const {
	// ゲームオーバー || ゲームクリア || ボス登場演出  への遷移条件
	return shouldTransitionToGameOver_ || shouldTransitionToGameClear_ || shouldTransitionToBossInrto_;
}

bool GamePlayState::IsPlayerDead() const { return scene_->GetPlayer()->IsDead(); }

bool GamePlayState::IsBossDying() const { return scene_->GetEnemyManager()->GetBoss()->IsDying(); }

bool GamePlayState::ShouldShowBossIntro() const { return shouldTransitionToBossInrto_; }
