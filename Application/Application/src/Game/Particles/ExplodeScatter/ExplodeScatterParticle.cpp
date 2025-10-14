#include "ExplodeScatterParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

ExplodeScatterParticle::ExplodeScatterParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

ExplodeScatterParticleData ExplodeScatterParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	ExplodeScatterParticleData p;
	auto rand = RandomGenerator::GetInstance();

	p.transform.translate = pos;
	p.transform.rotate = {0.0f, 0.0f, 0.0f};
	p.transform.scale = rand->RandomValue({0.2f, 0.2f, 0.2f}, {0.4f, 0.4f, 0.4f});
	p.velocity = rand->RandomValue({-12.0f, 0.0f, -12.0f}, {12.0f, 8.0f, 12.0f});
	p.color = {1.0f, 1.0f, 0.0f, 1.0f};
	p.currentTime = 0.0f;
	p.lifeTime = rand->RandomValue(0.8f, 1.2f);

	p.initScale = p.transform.scale;
	p.rotationSpeed = rand->RandomValue({-3.0f, -3.0f, -3.0f}, {3.0f, 3.0f, 3.0f});

	return p;
}

void ExplodeScatterParticle::UpdateParticle(ExplodeScatterParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	///
	///	移動
	///

	// 重力の影響を受ける
	Float3 gravity = {0.0f, -9.8f, 0.0f};
	p.velocity += gravity * dt;

	p.transform.translate += p.velocity * dt;

	///
	///	回転
	///

	float damping = Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate += p.rotationSpeed * damping * dt;

	///
	///	縮小
	///
	if (t > 0.8f) {                       // 4/5に到達したら
		float localT = (t - 0.8f) / 0.2f; // 0~1に正規化
		float easeT = Easing::EaseInQuad(localT);
		p.transform.scale = p.initScale * (1.0f - easeT);
	}

	///
	///	色変更
	///

	if (t > 0.5f) {
		// 黄->橙
		float localT = (t - 0.5f) / 0.5f; // 0~1に正規化
		p.color.y = Easing::Lerp(1.0f, 0.5f, localT);
	}
}
