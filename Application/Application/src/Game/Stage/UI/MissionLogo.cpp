#include "MissionLogo.h"

// Engine
#include <TextureManager.h>
#include <TimeManager.h>

// Application
#include <src/Game/Stage/StageManager.h>

void MissionLogo::Initialize() {
	// スプライト基盤機能生成
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(Cygnus::DirectXBase::GetInstance());

	// スプライト生成
	uint32_t textureKill = Cygnus::TextureManager::Load("UI/objective1.png");
	spriteKill_ = std::make_unique<Cygnus::Sprite>();
	spriteKill_->Initialize(spriteCommon_.get(), textureKill);
	spriteKill_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteKill_->SetPosition({ 640.0f, 360.0f });

	uint32_t textureDestroy = Cygnus::TextureManager::Load("UI/objective2.png");
	spriteDestroy_ = std::make_unique<Cygnus::Sprite>();
	spriteDestroy_->Initialize(spriteCommon_.get(), textureDestroy);
	spriteDestroy_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteDestroy_->SetPosition({ 640.0f, 360.0f });

	uint32_t textureBoss = Cygnus::TextureManager::Load("UI/objective3.png");
	spriteBoss_ = std::make_unique<Cygnus::Sprite>();
	spriteBoss_->Initialize(spriteCommon_.get(), textureBoss);
	spriteBoss_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteBoss_->SetPosition({ 640.0f, 360.0f });

	uint32_t textureClear = Cygnus::TextureManager::Load("UI/objectiveClear.png");
	spriteClear_ = std::make_unique<Cygnus::Sprite>();
	spriteClear_->Initialize(spriteCommon_.get(), textureClear);
	spriteClear_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteClear_->SetPosition({ 640.0f, 360.0f });
}

void MissionLogo::Update() {
	// 再生中でなければ描画スキップ
	if (currentState_ == AnimationState::None) return;

	// タイマー更新
	timer_ -= Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	if (timer_ <= 0.0f) {
		currentState_ = AnimationState::None;
	}

	spriteKill_->Update();
	spriteDestroy_->Update();
	spriteBoss_->Update();
	spriteClear_->Update();
}

void MissionLogo::DrawUI() {
	// 再生中でなければ描画スキップ
	if (currentState_ == AnimationState::None) return;

	// ステージ目標のテロップ
	if (currentState_ == AnimationState::StartMission) {
		// 目標によって変更
		switch (stageType_)
		{
		case StageType::killAllEnemies:
			spriteKill_->Draw();
			break;
		case StageType::DestroyAllGeneratos:
			spriteDestroy_->Draw();
			break;
		case StageType::BossBattle:
			spriteBoss_->Draw();
			break;
		}

	// ステージクリア時のテロップ
	} else if (currentState_ == AnimationState::ObjectiveClear) {
		// ボス戦では表示しない
		if(stageType_ == StageType::BossBattle) return;

		spriteClear_->Draw();
	}
}

void MissionLogo::Start(AnimationState state, StageType type) {
	currentState_ = state;
	stageType_ = type;
	timer_ = kDisplayDuration; // 表示時間のセット
}
