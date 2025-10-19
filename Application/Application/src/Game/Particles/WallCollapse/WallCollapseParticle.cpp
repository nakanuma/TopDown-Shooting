#include "WallCollapseParticle.h"

// Engine
#include <RandomGenerator.h>
#include <Easing.h>

WallCollapseParticle::WallCollapseParticle(ModelManager::ModelData& model)
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

WallCollapseParticleData WallCollapseParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	WallCollapseParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 範囲（壊れる壁とほぼ同じサイズ）
	Float3 spawnRange = {11.5f, 2.5f, 0.5f};
	Float3 offset = {
		rand->RandomValue(-spawnRange.x, spawnRange.x),
		rand->RandomValue(-spawnRange.y, spawnRange.y),
		rand->RandomValue(-spawnRange.z, spawnRange.z),
	};

	p.transform.translate = pos + offset;
	p.transform.rotate = { 0.0f, 0.0f, 0.0f };

	float scale = rand->RandomValue(0.2f, 0.8f);
	p.transform.scale = {scale, scale, scale};

	// 中心から外向きのベクトル
	Float3 dir = offset;
	dir += rand->RandomValue({-0.01f, 0.0f, -0.01f}, {0.01f, 0.3f, 0.01f});
	dir = Float3::Normalize(dir);

	// 初速度
	float speed = rand->RandomValue(15.0f, 35.0f);
	p.velocity = dir * speed;

	p.color = { 0.53f, 0.53f, 0.53f, 1.0f };
	p.lifeTime = rand->RandomValue(2.0f, 3.0f);
	p.currentTime = 0.0f;

	return p;
}

void WallCollapseParticle::UpdateParticle(WallCollapseParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 重力
	const Float3 gravity = {0.0f, -9.8f, 0.0f};
	// 速度更新
	p.velocity += gravity * dt;
	// 位置更新
	p.transform.translate += p.velocity * dt;
}