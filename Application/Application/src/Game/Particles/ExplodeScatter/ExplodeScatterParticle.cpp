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
	p.transform.scale_ = rand->RandomValue({0.2f, 0.2f, 0.2f}, {0.4f, 0.4f, 0.4f});
	// 速度ベクトル
	p.velocity = rand->RandomValue({-12.0f, 0.0f, -12.0f}, {12.0f, 8.0f, 12.0f});
	// 色
	p.color = {1.0f, 1.0f, 0.0f, 1.0f};
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = rand->RandomValue(0.8f, 1.2f);
	// 初期スケール
	p.initScale = p.transform.scale_;
	// 回転速度
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

	p.transform.translate_ += p.velocity * dt;

	///
	///	回転
	///

	float damping = Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate_ += p.rotationSpeed * damping * dt;

	///
	///	縮小
	///
	if (t > 0.8f) {                       // 4/5に到達したら
		float localT = (t - 0.8f) / 0.2f; // 0~1に正規化
		float easeT = Easing::EaseInQuad(localT);
		p.transform.scale_ = p.initScale * (1.0f - easeT);
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
