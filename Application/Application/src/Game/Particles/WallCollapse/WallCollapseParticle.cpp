#include "WallCollapseParticle.h"

// Engine
#include <RandomGenerator.h>
#include <Easing.h>

WallCollapseParticle::WallCollapseParticle(ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

WallCollapseParticleData WallCollapseParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	WallCollapseParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置（壊れる壁の範囲内に生成されるようランダムなオフセットを加える）
	Float3 spawnRange = {11.5f, 2.5f, 0.5f};
	Float3 offset = {
		rand->RandomValue(-spawnRange.x, spawnRange.x),
		rand->RandomValue(-spawnRange.y, spawnRange.y),
		rand->RandomValue(-spawnRange.z, spawnRange.z),
	};
	p.transform.translate_ = pos + offset;
	// 回転
	p.transform.rotate_ = { 0.0f, 0.0f, 0.0f };
	// スケール
	float scale = rand->RandomValue(0.2f, 0.8f);
	p.transform.scale_ = {scale, scale, scale};
	// 速度ベクトル
	Float3 dir = offset;
	dir += rand->RandomValue({-0.01f, 0.0f, -0.01f}, {0.01f, 0.3f, 0.01f}); // 中心から外側へ向かうオフセット
	dir = Float3::Normalize(dir);
	float speed = rand->RandomValue(15.0f, 35.0f);
	p.velocity = dir * speed;
	// 色
	p.color = { 0.53f, 0.53f, 0.53f, 1.0f };
	// 生存時間
	p.lifeTime = rand->RandomValue(2.0f, 3.0f);
	// 経過時間
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
	p.transform.translate_ += p.velocity * dt;
}