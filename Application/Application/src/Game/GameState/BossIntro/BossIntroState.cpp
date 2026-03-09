#include "BossIntroState.h"

// Application
#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/Transition/FadeTransition.h>

BossIntroState::BossIntroState(GamePlayScene* scene) { scene_ = scene; }

void BossIntroState::Initialize() { 
	bossIntroSequence_ = std::make_unique<BossIntroSequence>(); 
	bossIntroSequence_->Initialize(scene_->GetSpriteCommon(), scene_->GetPlayer(), scene_);

	bossIntroSequence_->Start(); // 登場演出開始
}

void BossIntroState::Finalize() { scene_->GetEnemyManager()->Finalize(); }

void BossIntroState::Update() { 
	bossIntroSequence_->Update(); 

	scene_->GetPlayer()->Update(false); // プレイヤーは操作不可能
	scene_->GetEnemyManager()->Update();

	scene_->GetField()->Update();
	scene_->GetObstacleManager()->Update(scene_->GetPlayer()->GetTranslate());

	FadeTransition::GetInstance()->Update();
}

void BossIntroState::Draw() { 
	bossIntroSequence_->Draw();

	scene_->GetPlayer()->Draw();
	scene_->GetEnemyManager()->Draw(); 

	scene_->GetField()->Draw();
	scene_->GetObstacleManager()->Draw(scene_->GetPlayer()->GetTranslate());
}

void BossIntroState::DrawShadow() {
	bossIntroSequence_->DrawShadow();

	scene_->GetPlayer()->DrawShadow();
	scene_->GetEnemyManager()->DrawShadow(); 

	scene_->GetObstacleManager()->DrawShadow(scene_->GetPlayer()->GetTranslate());
}

void BossIntroState::DrawShadowSkinning() { 
	scene_->GetPlayer()->DrawShadowSkinning(); 
	scene_->GetEnemyManager()->DrawShadowSkinning(); 
}

void BossIntroState::DrawUI() { 
	FadeTransition::GetInstance()->Draw(); 
}

void BossIntroState::Debug() {
	bossIntroSequence_->Debug();
}

bool BossIntroState::CanTransition() const { 
	// ボス登場演出が終了したら遷移可能
	return bossIntroSequence_->IsFinished(); 
}

bool BossIntroState::IsPlayerDead() const { return false; }

bool BossIntroState::IsBossDying() const { return false; }

bool BossIntroState::ShouldShowBossIntro() const { return false; }
