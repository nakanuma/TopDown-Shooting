#include "WallCollapseParticle.h"

// Engine
#include <RandomGenerator.h>
#include <Easing.h>

WallCollapseParticle::WallCollapseParticle(Cygnus::ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = Cygnus::BlendMode::Normal;
}

WallCollapseParticleData WallCollapseParticle::CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle)
{
	WallCollapseParticleData p;
	auto rand = Cygnus::RandomGenerator::GetInstance();

	// 位置（壊れる壁の範囲内に生成されるようランダムなオフセットを加える）
	Cygnus::Float3 spawnRange = kSpawnRange;
	Cygnus::Float3 offset = {
		rand->RandomValue(-spawnRange.x, spawnRange.x),
		rand->RandomValue(-spawnRange.y, spawnRange.y),
		rand->RandomValue(-spawnRange.z, spawnRange.z),
	};
	p.transform.translate_ = pos + offset;
	// 回転
	p.transform.rotate_ = { 0.0f, 0.0f, 0.0f };
	// スケール
	float scale = rand->RandomValue(kMinScale, kMaxScale);
	p.transform.scale_ = { scale, scale, scale };
	// 速度ベクトル
	Cygnus::Float3 dir = offset;
	dir += rand->RandomValue(kMinVelocity, kMaxVelocity); // 中心から外側へ向かうオフセット
	dir = Cygnus::Float3::Normalize(dir);
	float speed = rand->RandomValue(kMinSpeed, kMaxSpeed);
	p.velocity = dir * speed;
	// 色
	p.color = kInitialColor;
	// 生存時間
	p.lifeTime = rand->RandomValue(kMinLifeTime, kMaxLifeTime);
	// 経過時間
	p.currentTime = 0.0f;

	return p;
}

void WallCollapseParticle::UpdateParticle(WallCollapseParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 速度更新
	p.velocity += kGravity * dt;
	// 位置更新
	p.transform.translate_ += p.velocity * dt;
}