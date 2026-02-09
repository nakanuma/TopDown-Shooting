#include "EnemyVisualEffects.h"

// Engine
#include <TimeManager.h>
#include <Easing.h>

void EnemyVisualEffects::Initialize(Cygnus::Object3D* targetObject) { targetObject_ = targetObject; }

void EnemyVisualEffects::Update() {
	// 発光演出中でなければスキップ
	if (!isHitBlink_ || !targetObject_) return;

	hitBlinkTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float t;

	switch (hitBlinkPhase_)
	{
	case HitBlinkPhase::BlinkIn:
		if (hitBlinkTimer_ < kHitBlinkDuration) {
			t = std::clamp(hitBlinkTimer_ / kHitBlinkDuration, 0.0f, 1.0f);
			// プレイヤーを発光させる
			targetObject_->materialCB_.data_->emissiveIntensity = Cygnus::Easing::EaseOutQuad(t);
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
			targetObject_->materialCB_.data_->emissiveIntensity = 1.0f - Cygnus::Easing::EaseInQuad(t);
		} else {
			// 終了したら待機フェーズへ
			hitBlinkPhase_ = HitBlinkPhase::Wait;
			hitBlinkTimer_ = 0.0f;
			targetObject_->materialCB_.data_->emissiveIntensity = 0.0f;
			isHitBlink_ = false;
		}
		break;
	}
}

void EnemyVisualEffects::TriggerHitBlink() {
	// 発光開始のため各種パラメーターをセット
	targetObject_->materialCB_.data_->emissiveColor = kHitBlinkColor;
	isHitBlink_ = true;
	hitBlinkPhase_ = HitBlinkPhase::BlinkIn;
	hitBlinkTimer_ = 0.0f;
}