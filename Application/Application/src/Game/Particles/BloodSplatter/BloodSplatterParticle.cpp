#include "BloodSplatterParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

BloodSplatterParticle::BloodSplatterParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

BloodSplatterParticleData BloodSplatterParticle::CreateParticle(const Float3& pos, const Float3& velocity, float angle) {
	BloodSplatterParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, 0.0f};
	// スケール
	float scale = rand->RandomValue(kMinScale, kMaxScale);
	p.transform.scale_ = {scale, scale, scale};
	// 速度ベクトル
	p.velocity = rand->RandomValue(kMinVelocity, kMaxVelocity);
	// 色
	p.color = kInitialColor;
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = rand->RandomValue(kMinLifeTime, kMaxLifeTime);
	// 初期スケール
	p.initScale = p.transform.scale_;

	return p;
}

void BloodSplatterParticle::UpdateParticle(BloodSplatterParticleData& p, float dt) {
	// 速度の更新
	p.velocity += kGravity * dt * kGravityMultiplier;
	// 位置の更新
	p.transform.translate_ += p.velocity * dt;

	// 仮の床の高さ
	const float kGroundY = p.initScale.y * kGroundOffsetMultiplier;

	// 床に到達したら跳ねるように
	if (p.transform.translate_.y <= kGroundY) {
		p.transform.translate_.y = kGroundY; // 床を貫通しないよう制限

		// Y速度がある程度大きければ符号を反転しながら減衰させる
		if (std::abs(p.velocity.y) > kMinBounceVelocity) {
			p.velocity.y *= -kBounceFactor;
		} else {
			p.velocity.y = 0.0f;
		}

		// XとZの速度を徐々に減衰させて止める
		p.velocity.x *= kFriction;
		p.velocity.z *= kFriction;
	}

	// 動きが止まったことを知らせる
	if (p.velocity.y == 0.0f && !p.isStop) {
		p.isStop = true;
		p.stopTime = p.currentTime;
	}

	// スケールの縮小
	if (p.isStop) {
		// 両方から止まった瞬間の時間を引いて、正しく最初の生存時間で消えるよう調整
		float t = std::clamp((p.currentTime - p.stopTime) / (p.lifeTime - p.stopTime), 0.0f, 1.0f);
		float easeT = Easing::EaseInBack(t);
		p.transform.scale_ = p.initScale * (1.0f - easeT);
	}
}
