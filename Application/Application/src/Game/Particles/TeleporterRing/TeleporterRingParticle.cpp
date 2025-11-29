#include "TeleporterRingParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

TeleporterRingParticle::TeleporterRingParticle(ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

TeleporterRingParticleData TeleporterRingParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	TeleporterRingParticleData p;

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, 0.0f};
	// スケール
	p.transform.scale_ = kDefaultScale;
	// 速度ベクトル
	p.velocity = kDefaultVelocity;
	// 色
	p.color = kDefaultColor;
	// 生存時間
	p.lifeTime = kLifeTime;
	// 経過時間
	p.currentTime = 0.0f;

	return p;
}

void TeleporterRingParticle::UpdateParticle(TeleporterRingParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 移動
	p.transform.translate_ += p.velocity * dt;

	// 透明化
	float alpha = kInitialAlpha - t;
	p.color.w = alpha;
}
