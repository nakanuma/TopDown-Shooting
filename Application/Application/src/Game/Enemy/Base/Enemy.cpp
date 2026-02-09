#include "Enemy.h"

// Engine
#include <TimeManager.h>
#include <Easing.h>
#include <BehaviourTree/BehaviorNode.h>

void Enemy::SetNodeStatus(const void* nodePtr, Cygnus::BehaviorStatus status)
{
	nodeStatusMap_[nodePtr] = status;

}

Cygnus::BehaviorStatus Enemy::GetNodeStatus(const void* nodePtr) const
{
	auto it = nodeStatusMap_.find(nodePtr);

	return (it != nodeStatusMap_.end()) ? it->second : Cygnus::BehaviorStatus::Running;
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
