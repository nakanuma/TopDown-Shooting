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
		Cygnus::Float4 c0 = Cygnus::Float4::Lerp(kTrailTailColor, kTrailHeadColor, t0); // この線分での末尾の色
		Cygnus::Float4 c1 = Cygnus::Float4::Lerp(kTrailTailColor, kTrailHeadColor, t1); // この線分での先頭の色

		// トレイル線の登録
		Cygnus::LineDrawer::GetInstance()->RegisterTracer(trailPoints_[i - 1], trailPoints_[i], kTrailLineWidth, c1, c0);
	}
}

void Bullet::FinishLifeCycle() {
	// 弾の死亡
	isDead_ = true;

	// コライダー破棄
	if (collider_) {
		Cygnus::CollisionManager::GetInstance()->Unregister(collider_.get());
	}
}

void Bullet::EmitBloodHitParticles(const Cygnus::Float3& position, const Cygnus::Float3& velocity) {
	// 血飛沫パーティクル
	Cygnus::ParticleEffectManager::GetInstance()->Emit("bloodSplatter", position, kBloodSplatterCount);
	// 血煙パーティクル
	Cygnus::ParticleEffectManager::GetInstance()->Emit("bloodSmoke", position, kBloodSmokeCount, velocity);
	// 後方血飛散パーティクル
	Cygnus::ParticleEffectManager::GetInstance()->Emit("bloodScatter", position, kBloodScatterCount, velocity);
}

void Bullet::EmitHardHitParticles(const Cygnus::Float3& position, const Cygnus::Float3& velocity) {
	// 後方飛散パーティクル
	Cygnus::ParticleEffectManager::GetInstance()->Emit("backscatter", position, kBackscatterCount, velocity);
	// 衝撃煙パーティクル
	Cygnus::ParticleEffectManager::GetInstance()->Emit("impactSmoke", position, kImpactSmokeCount, velocity);
}
