#include "SparkParticle_Star.h"

// C++
#include <numbers>

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

// ---------------------------------------------------------
// コンストラクタ
// ---------------------------------------------------------
SparkParticle_Star::SparkParticle_Star(ModelManager::ModelData& model) 
{
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = {true, true, true};
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

// ---------------------------------------------------------
// パーティクル固有の生成処理
// ---------------------------------------------------------
SparkParticleStarData SparkParticle_Star::CreateParticle(const Float3& pos) 
{
	Float3 randVelocity = RandomGenerator::GetInstance()->RandomValue({-0.2f, -0.2f, 0.0f}, {0.2f, 0.2f, 0.0f});
	float randLifeTime = RandomGenerator::GetInstance()->RandomValue(0.2f, 0.5f); // 生存時間

	SparkParticleStarData p;
	p.transform.translate = pos;
	p.transform.rotate = {0.0f, 0.0f, 0.0f};
	p.transform.scale = {2.0f, 2.0f, 1.0f};
	p.velocity = randVelocity;
	p.color = {1.0f, 0.5f, 0.2f, 1.0f};
	p.lifeTime = randLifeTime;
	p.currentTime = 0.0f;

	return p;
}

// ---------------------------------------------------------
// パーティクル固有の更新処理
// ---------------------------------------------------------
void SparkParticle_Star::UpdateParticle(SparkParticleStarData& p, float dt) 
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 減速
	Float3 easedMove = p.velocity * (1.0f - t) * 2.0f;
	p.transform.translate += easedMove;

	// 透明化
	p.color.w = Easing::Lerp(1.0f, 0.0f, Easing::EaseInExpo(t));
}
