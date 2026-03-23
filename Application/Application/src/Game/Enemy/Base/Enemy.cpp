#include "Enemy.h"

// Engine
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <Engine/Sound/SoundManager.h>

// Application
#include <src/Game/Bullet/Base/Bullet.h>
#include <src/Game/System/ResultStats.h>

void Enemy::OnCollision(Cygnus::Collider* other)
{
	// vs PlayerBullet
	if(other->GetTag() == "PlayerBullet") {
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		if(bullet) {
			ApplyDamage(bullet->GetDamage());
		}
	}
}

void Enemy::ApplyDamage(int32_t damage)
{
	if(isDead_) return;

	// 被弾時の発光演出を開始
	visualEffect_->TriggerHitBlink();

	// 被ダメージ処理
	currentHP_ -= damage;
	ResultStats::GetInstance()->AddHit();          // 弾が命中したことを記録
	ResultStats::GetInstance()->AddDamage(damage); // 与えたダメージを記録

	// 死亡判定
	if (currentHP_ <= 0) {
		isDead_ = true;

		// 死亡時パーティクル発生
		Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate_, kDeathCrossCount, { 0.0f, 0.0f, 0.0f }, Cygnus::DegToRad(kDeathCrossAngle1)); // クロス片側
		Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate_, kDeathCrossCount, { 0.0f, 0.0f, 0.0f }, Cygnus::DegToRad(kDeathCrossAngle2)); // クロス片側

		ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録

		// 効果音発生
		Cygnus::SoundManager::GetInstance()->Play("enemy_dead", false, 0.25f);
	}
}

void Enemy::ResolveObstacleCollision(Cygnus::Collider* other, Cygnus::Transform& targetTransform)
{
	Cygnus::AABBCollider* myAABB = dynamic_cast<Cygnus::AABBCollider*>(collider_.get());
	Cygnus::AABBCollider* otherAABB = dynamic_cast<Cygnus::AABBCollider*>(other);

	// 押し戻し処理
	if (myAABB && otherAABB) {
		// 押し戻しベクトル取得
		Cygnus::Float3 pushVec = myAABB->GetPushBackVector(*otherAABB);
		// 位置を補正
		targetTransform.translate_ += pushVec;

		// コライダーも更新しておく
		Cygnus::Float3 currentMin = myAABB->GetMin();
		Cygnus::Float3 currentMax = myAABB->GetMax();
		myAABB->SetMin(currentMin + pushVec);
		myAABB->SetMax(currentMax + pushVec);
	}
}

void Enemy::OnDetected() {
	// 既に発見状態ならスキップ
	if (isDetectedPlayer_) return;

	isDetectedPlayer_ = true;	// 発見状態にする
	ui_->PlayDetectAnimation(); // UIアニメーションを行う
}