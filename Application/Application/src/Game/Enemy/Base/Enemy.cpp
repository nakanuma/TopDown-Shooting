#include "Enemy.h"

// Engine
#include <TimeManager.h>
#include <Easing.h>

void Enemy::SetupHPBarSprite(std::unique_ptr<Cygnus::Sprite>& spritePtr, const Cygnus::Float4& color) {
	uint32_t texture = Cygnus::TextureManager::Load("white.png");
	spritePtr = std::make_unique<Cygnus::Sprite>();
	spritePtr->Initialize(spriteCommon_.get(), texture);
	spritePtr->SetSize(kHPBarSize);
	spritePtr->SetColor(color);
}

void Enemy::SetupReloadSprite(std::unique_ptr<Cygnus::Sprite>& spritePtr) {
	uint32_t texture = Cygnus::TextureManager::Load("white.png");
	spritePtr = std::make_unique<Cygnus::Sprite>();
	spritePtr->Initialize(spriteCommon_.get(), texture);
	spritePtr->SetSize(kReloadSize);
}

void Enemy::HandleHitBlink(){
	// 発光演出中でなければスキップ
	if (!isHitBlink_) return;

	hitBlinkTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float t;

	switch (hitBlinkPhase_)
	{
	case HitBlinkPhase::BlinkIn:
		if (hitBlinkTimer_ < kHitBlinkDuration) {
			t = std::clamp(hitBlinkTimer_ / kHitBlinkDuration, 0.0f, 1.0f);
			// プレイヤーを発光させる
			objectEnemy_->materialCB_.data_->emissiveIntensity = Cygnus::Easing::EaseOutQuad(t);
		} else {
			// 終了したら次のフェーズへ
			hitBlinkPhase_ = HitBlinkPhase::BlinkOut;
			hitBlinkTimer_ = 0.0f;
		}
		break;
	case HitBlinkPhase::BlinkOut:
		if (hitBlinkTimer_ < kHitBlinkDuration) {
			t = std::clamp(hitBlinkTimer_ / kHitBlinkDuration, 0.0f, 1.0f);
			// プレイヤーを減光させる
			objectEnemy_->materialCB_.data_->emissiveIntensity = 1.0f - Cygnus::Easing::EaseInQuad(t);
		} else {
			// 終了したら待機フェーズへ
			hitBlinkPhase_ = HitBlinkPhase::Wait;
			hitBlinkTimer_ = 0.0f;
			objectEnemy_->materialCB_.data_->emissiveIntensity = 0.0f;
			isHitBlink_ = false;
		}
		break;
	}
}
