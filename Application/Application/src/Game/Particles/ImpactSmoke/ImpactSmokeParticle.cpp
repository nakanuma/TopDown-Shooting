#include "ImpactSmokeParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

ImpactSmokeParticle::ImpactSmokeParticle(ModelManager::ModelData& model)
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

ImpactSmokeParticleData ImpactSmokeParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	ImpactSmokeParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置
	Float3 offset = rand->RandomValue({-0.25f, -0.25f, -0.25f }, { 0.25f, 0.25f, 0.25f });
	p.transform.translate = pos + offset;
	// スケール
	float scale = rand->RandomValue(0.2f, 0.4f);
	p.transform.scale = { scale, scale, scale };
	// 回転
	p.transform.rotate = {0.0f, 0.0f, 0.0f};
	// 速度ベクトル
	Float3 baseDir = Float3::Normalize(velocity) * -1.0f; // 引数で受け取った方向と逆向きにする
	float diff = 1.0f;
	Float3 randDir = rand->RandomValue({ -diff, 0.0f, -diff }, { diff, 0.0f, diff }); // 方向をバラつかせるためのオフセット
	p.velocity = Float3::Normalize(baseDir + randDir) * rand->RandomValue(1.0f, 6.0f);
	// 色
	p.color = {0.5f, 0.5f, 0.5f, 1.0f };
	// 生存時間
	p.lifeTime = rand->RandomValue(0.25f, 0.5f);
	// 経過時間
	p.currentTime = 0.0f;
	// 初期スケール
	p.initScale = p.transform.scale;
	// 回転速度
	p.rotationSpeed = rand->RandomValue({ -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f });

	return p;
}

void ImpactSmokeParticle::UpdateParticle(ImpactSmokeParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	///
	///	移動
	///		

	float moveFactor = Easing::EaseOutQuad(1.0f - t);
	p.transform.translate += p.velocity * moveFactor * dt;

	///
	///	回転
	///

	float damping = Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate += p.rotationSpeed * damping * dt;

	///
	///	拡大
	///

	float startScale = p.initScale.x;
	float endScale = 0.5f;
	float scale = Easing::EaseInQuad(t) * (endScale - startScale) + startScale;
	p.transform.scale = { scale, scale, scale };

	///
	/// 透明化
	///
	
	float alpha = 1.0f - Easing::EaseInQuart(t);
	p.color.w = alpha;
}
