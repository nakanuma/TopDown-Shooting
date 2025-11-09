#include "DeathCrossParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

DeathCrossParticle::DeathCrossParticle(ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = { true, true, true };
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

DeathCrossParticleData DeathCrossParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	DeathCrossParticleData p;

	// 位置
	p.transform.translate = pos;
	// 回転
	p.transform.rotate = { 0.0f, 0.0f, angle };
	// スケール
	p.transform.scale = { 3.5f, 3.5f, 1.0f };
	// 速度ベクトル
	p.velocity = { 0.0f, 0.0f, 0.0f };
	// 色
	p.color = { 0.5f, 0.1f, 0.05f, 1.0f };
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = 0.75f;

	return p;
}

void DeathCrossParticle::UpdateParticle(DeathCrossParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	///
	/// 縮小
	///  

	// Yスケール
	const float startScaleY = 3.5f;
	const float endScaleY = 0.4f;
	const float scaleDuration = 0.25f;

	float scaleT = std::clamp(p.currentTime / scaleDuration, 0.0f, 1.0f);
	float easeT = Easing::EaseOutQuart(scaleT);

	p.transform.scale.y = Easing::Lerp(startScaleY, endScaleY, easeT);

	///
	/// 透明化
	///

	float alpha = 1.0f - Easing::EaseInQuart(t);
	p.color.w = alpha;
}
