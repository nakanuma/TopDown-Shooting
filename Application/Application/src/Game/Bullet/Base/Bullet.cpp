#include "Bullet.h"

// Engine
#include <LineDrawer.h>
#include <ParticleEffect/ParticleEffectManager.h>

void Bullet::DrawTrail() {
	// [0]と[1], [1]と[2]... といったように全てのポイントを繋ぐ線を作る
	for (size_t i = 1; i < trailPoints_.size(); ++i) {
		// 線分の位置に応じた割合を計算
		float t0 = static_cast<float>(i - 1) / (trailPoints_.size());
		float t1 = static_cast<float>(i) / (trailPoints_.size() - 1);

		// 線の両端の色を補間
		Float4 c0 = Float4::Lerp(kTrailTailColor, kTrailHeadColor, t0); // この線分での末尾の色
		Float4 c1 = Float4::Lerp(kTrailTailColor, kTrailHeadColor, t1); // この線分での先頭の色

		// トレイル線の登録
		LineDrawer::GetInstance()->RegisterTracer(trailPoints_[i - 1], trailPoints_[i], kTrailLineWidth, c1, c0);
	}
}

void Bullet::FinishLifeCycle() {
	// 弾の死亡
	isDead_ = true;

	// コライダー破棄
	if (collider_) {
		CollisionManager::GetInstance()->Unregister(collider_.get());
	}
}

void Bullet::EmitBloodHitParticles(const Float3& position, const Float3& velocity) {
	// 血飛沫パーティクル
	ParticleEffectManager::GetInstance()->Emit("bloodSplatter", position, kBloodSplatterCount);
	// 血煙パーティクル
	ParticleEffectManager::GetInstance()->Emit("bloodSmoke", position, kBloodSmokeCount, velocity);
	// 後方血飛散パーティクル
	ParticleEffectManager::GetInstance()->Emit("bloodScatter", position, kBloodScatterCount, velocity);
}

void Bullet::EmitHardHitParticles(const Float3& position, const Float3& velocity) {
	// 後方飛散パーティクル
	ParticleEffectManager::GetInstance()->Emit("backscatter", position, kBackscatterCount, velocity);
	// 衝撃煙パーティクル
	ParticleEffectManager::GetInstance()->Emit("impactSmoke", position, kImpactSmokeCount, velocity);
}
