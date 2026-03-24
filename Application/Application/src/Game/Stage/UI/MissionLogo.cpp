#include "MissionLogo.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <TimeManager.h>
#include <Easing.h>

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

	baseSize_ = spriteClear_->GetSize(); // objectiveSpriteは全て同じサイズで統一
}

void MissionLogo::Update() {
	// 再生中でなければ描画スキップ
	if (currentState_ == AnimationState::None) return;

	// 遅延時間更新
	if(startDelayTimer_ > 0.0f) {
		startDelayTimer_ -= Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		return;	// 遅延時間中には以下の更新をスキップ
	}

	// タイマー更新
	timer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	if(timer_ > kDisplayDuration) {
		currentState_ = AnimationState::None;
		return;
	}

	// アニメーション対象のスプライトを特定
	Cygnus::Sprite* target = nullptr;
	if(currentState_ == AnimationState::StartMission) {
		// ステージ目標に合わせる
		if(stageType_ == StageType::killAllEnemies) {
			target = spriteKill_.get();
		} else if (stageType_ == StageType::DestroyAllGeneratos) {
			target = spriteDestroy_.get();
		} else if (stageType_ == StageType::BossBattle) {
			target = spriteBoss_.get();
		}
	} else if (currentState_ == AnimationState::ObjectiveClear) {
		// 目標達成時スプライトを取得（ボス戦は特定の演出があるため除外）
		if(stageType_ != StageType::BossBattle) {
			target = spriteClear_.get();
		}
	}

	if(!target) return;

	// アニメーション計算

	// フェードイン
	if(timer_ < kFadeDuration) {
		float t = std::clamp(timer_ / kFadeDuration, 0.0f, 1.0f);
		float easeT = Cygnus::Easing::EaseOutBack(t);

		// 拡大
		target->SetSize(Cygnus::Float2::Lerp(baseSize_ * kScaleStart, baseSize_, easeT));
		// 透明->不透明
		color_.w = Cygnus::Easing::Lerp(0.0f, 1.0f, t);
		target->SetPosition(kBasePos);

	// フェードアウト
	} else if (timer_ > (kDisplayDuration - kFadeDuration)) {
		float fadeOutTimer = timer_ - (kDisplayDuration - kFadeDuration);
		float t = std::clamp(fadeOutTimer / kFadeDuration, 0.0f, 1.0f);
		
		// 不透明->透明
		color_.w = Cygnus::Easing::Lerp(1.0f, 0.0f, t);
		// 上にスライド
		float currentY = Cygnus::Easing::Lerp(kBasePos.y, kBasePos.y - kSlideDistance, t);
		target->SetPosition({kBasePos.x, currentY});
		target->SetSize(baseSize_);

	// 表示維持
	} else {
		color_.w = 1.0f;
		target->SetPosition(kBasePos);
		target->SetSize(baseSize_);
	}

	target->SetColor(color_);
	target->Update();
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
		if (stageType_ == StageType::BossBattle) return;

		spriteClear_->Draw();
	}
}

void MissionLogo::Start(AnimationState state, StageType type, float delayTime) {
	currentState_ = state;
	stageType_ = type;
	timer_ = 0.0f; // タイマーリセット
	startDelayTimer_ = delayTime; // 最初の遅延時間を設定
}
