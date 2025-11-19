#include "BossFragmentsParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

BossFragmentsParticle::BossFragmentsParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

BossFragmentsParticleData BossFragmentsParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	BossFragmentsParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置（ボスの範囲内に生成されるようランダムなオフセットを加える）
	Float3 offset = {
	    rand->RandomValue(-kSpawnRange.x, kSpawnRange.x),
	    rand->RandomValue(-kSpawnRange.y, kSpawnRange.y),
	    rand->RandomValue(-kSpawnRange.z, kSpawnRange.z),
	};
	p.transform.translate_ = pos + offset;
	// 回転
	p.transform.rotate_ = rand->RandomValue({0.0f, 0.0f, 0.0f}, {PIf * 2.0f, PIf * 2.0f, PIf * 2.0f});
	// スケール
	Float3 scale = rand->RandomValue(kMinScale, kMaxScale);
	p.transform.scale_ = scale;
	// 速度ベクトル
	Float3 dir = offset;
	dir += rand->RandomValue(kMinVelocityOffset, kMaxVelocityOffset); // 中心から外側へ向かうオフセット
	dir = Float3::Normalize(dir);
	float speed = rand->RandomValue(kMinSpeed, kMaxSpeed);
	p.velocity = dir * speed;
	// 色
	p.color = kInitialColor;
	// 生存時間
	p.lifeTime = rand->RandomValue(kMinLifeTime, kMaxLifeTime);
	// 経過時間
	p.currentTime = 0.0f;

	// 初期スケール
	p.initScale = p.transform.scale_;
	// 回転速度
	p.rotationSpeed = rand->RandomValue(kMinRotationSpeed, kMaxRotationSpeed);

	return p;
}

void BossFragmentsParticle::UpdateParticle(BossFragmentsParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 速度更新
	p.velocity += kGravity * dt;
	// 位置更新
	p.transform.translate_ += p.velocity * dt;
	// 回転
	p.transform.rotate_ += p.rotationSpeed * dt;
	// 縮小
	p.transform.scale_ = p.initScale * (1.0f - Easing::EaseInExpo(t));
}