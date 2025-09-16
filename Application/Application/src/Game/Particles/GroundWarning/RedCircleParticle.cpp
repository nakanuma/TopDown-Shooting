#include "RedCircleParticle.h"

// Engine
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Math/Easing.h>

// ---------------------------------------------------------
// コンストラクタ
// ---------------------------------------------------------
RedCircleParticle::RedCircleParticle(ModelManager::ModelData& model)
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

// ---------------------------------------------------------
// パーティクル固有の生成処理
// ---------------------------------------------------------
RedCircleParticleData RedCircleParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	RedCircleParticleData p;

	p.transform.translate = pos;
	p.transform.rotate = { PIf / 2.0f, 0.0f, 0.0f }; // 仰向けになるように
	p.transform.scale = { 0.0f, 0.0f, 0.0f };
	p.velocity = { 0.0f, 0.0f, 0.0f };
	p.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	p.currentTime = 0.0f;
	p.lifeTime = 1.0f;

	p.targetScale = { 5.0f, 5.0f, 5.0f }; // 地面警告攻撃の弾と同じサイズ
	return p;
}

// ---------------------------------------------------------
// パーティクル固有の更新処理
// ---------------------------------------------------------
void RedCircleParticle::UpdateParticle(RedCircleParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 拡大
	float easeT = Easing::EaseOutExpo(t);
	p.transform.scale = p.targetScale * easeT;
}
