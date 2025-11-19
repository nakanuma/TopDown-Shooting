#include "ExplodeScatterParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

ExplodeScatterParticle::ExplodeScatterParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

ExplodeScatterParticleData ExplodeScatterParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	ExplodeScatterParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, 0.0f};
	// スケール
	p.transform.scale_ = rand->RandomValue(kMinScale, kMaxScale);
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
	// 回転速度
	p.rotationSpeed = rand->RandomValue(kMinRotationSpeed, kMaxRotationSpeed);

	return p;
}

void ExplodeScatterParticle::UpdateParticle(ExplodeScatterParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	///
	///	移動
	///

	// 重力の影響を受ける
	p.velocity += kGravity * dt;

	p.transform.translate_ += p.velocity * dt;

	///
	///	回転
	///

	float damping = Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate_ += p.rotationSpeed * damping * dt;

	///
	///	縮小
	///
	if (t > kShrinkStartThreshold) {                                  // 4/5に到達したら
		float localT = (t - kShrinkStartThreshold) / kShrinkDuration; // 0~1に正規化
		float easeT = Easing::EaseInQuad(localT);
		p.transform.scale_ = p.initScale * (1.0f - easeT);
	}

	///
	///	色変更
	///

	if (t > kColorChangeStartThreshold) {
		// 黄->橙
		float localT = (t - kColorChangeStartThreshold) / kColorChangeDuration; // 0~1に正規化
		p.color.y = Easing::Lerp(kGreenStart, kGreenEnd, localT);
	}
}
