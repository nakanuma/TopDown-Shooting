#include "SparkParticle.h"

// Engine
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Math/Easing.h>

SparkParticle::SparkParticle(ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

SparkParticleData SparkParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	SparkParticleData p;
	auto rand = RandomGenerator::GetInstance();

	p.transform.translate = pos;

	p.transform.scale = { 0.04f, 0.04f, 0.8f };

	// veliocity
	Float3 baseDir = Float3::Normalize(velocity) * -1.0f;
	float diff = 0.9f;
	Float3 randDir = rand->RandomValue({ -diff, 0.0f, -diff }, { diff, 0.0f, diff });
	p.velocity = Float3::Normalize(baseDir + randDir) * rand->RandomValue(16.0f, 24.0f);

	// rotate（進行方向を向くように）
	Float3 dir = Float3::Normalize(p.velocity);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	p.transform.rotate = { -pitch, -yaw, 0.0f };


	p.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	p.lifeTime = rand->RandomValue(0.3f, 0.5f);
	p.currentTime = 0.0f;

	p.initScale = p.transform.scale;

	return p;
}

void SparkParticle::UpdateParticle(SparkParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 重力加速度を加える
	const Float3 gravity = { 0.0f, -9.8f, 0.0f };
	p.velocity += gravity * dt;

	// 移動
	float moveFactor = Easing::EaseOutQuart(1.0f - t);
	p.transform.translate += (p.velocity * moveFactor * dt);

	// 縮小
	float easeT = Easing::EaseInQuad(t);
	p.transform.scale.z = p.initScale.z * (1.0f - easeT);

	// 色
	Float4 color;
	if (t < 0.5f) {
		// 前半 : 白->橙
		float localT = t / 0.5f; // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 1.0f, localT);
		color.y = Easing::Lerp(1.0f, 0.5f, localT);
		color.z = Easing::Lerp(1.0f, 0.0f, localT);
	} else {
		// 後半 : 橙->赤
		float localT = (t - 0.5f) / 0.5f; // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 1.0f, localT);
		color.y = Easing::Lerp(0.5f, 0.0f, localT);
		color.z = Easing::Lerp(0.0f, 0.0f, localT);
	}
	color.w = 1.0f;

	p.color = color;
}
