#include "SparkParticle_Shrink.h"

// C++
#include <numbers>

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

// ---------------------------------------------------------
// コンストラクタ
// ---------------------------------------------------------
SparkParticle_Shrink::SparkParticle_Shrink(ModelManager::ModelData& model) 
{ 
	object_.model_ = &model; 
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

// ---------------------------------------------------------
// パーティクル固有の生成処理
// ---------------------------------------------------------
SparkParticleShrinkData SparkParticle_Shrink::CreateParticle(const Float3& pos) 
{ 
	float randAngle = RandomGenerator::GetInstance()->RandomValue(0.0f, std::numbers::pi_v<float> * 2.0f); // 角度。0 ~ 360度
	float randSpeed = RandomGenerator::GetInstance()->RandomValue(0.1f, 0.2f); // 速さ
	float randLifeTime = RandomGenerator::GetInstance()->RandomValue(0.2f, 0.4f); // 生存時間

	SparkParticleShrinkData p;
	p.transform.translate = pos;
	p.transform.rotate = {randAngle + 2.0f * std::numbers::pi_v<float> / 2.0f, 0.0f, 0.0f};
	p.transform.scale = {1.6f, 0.2f, 1.0f};
	p.velocity = {std::cosf(randAngle) * randSpeed, std::sinf(randAngle) * randSpeed, 0.0f};
	p.color = {1.0f, 0.5f, 0.2f, 1.0f};
	p.lifeTime = randLifeTime;
	p.currentTime = 0.0f;

	p.initScale = p.transform.scale;

	return p;
}

// ---------------------------------------------------------
// パーティクル固有の更新処理
// ---------------------------------------------------------
void SparkParticle_Shrink::UpdateParticle(SparkParticleShrinkData& p, float dt) 
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 減速
	Float3 easedMove = p.velocity * (1.0f - t) * 2.0f;
	p.transform.translate += easedMove;

	// 縮小（初期値->初期値の10%）
	p.transform.scale.x = Easing::Lerp(p.initScale.x, p.initScale.x * 0.1f, Easing::EaseInQuad(t));

	// 透明化（1.0->0.0）
	p.color.w = Easing::Lerp(1.0f, 0.0f, Easing::EaseInExpo(t));
}
