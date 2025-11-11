#include "MuzzleFlashParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

MuzzleFlashParticle::MuzzleFlashParticle(ModelManager::ModelData& model)
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

MuzzleFlashParticleData MuzzleFlashParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	MuzzleFlashParticleData p;
	auto rand = RandomGenerator::GetInstance();

	float randAngle = rand->RandomValue(0.0f, PIf * 2.0f);

	float scaleX = rand->RandomValue(0.4f, 0.5f);
	float scaleY = rand->RandomValue(0.8f, 1.0f);

	// 位置
	p.transform.translate = pos;
	// 回転
	p.transform.rotate = { 0.0f, 0.0f, randAngle };
	// スケール
	p.transform.scale = { scaleX, scaleY, 1.0f };
	// 速度ベクトル
	p.velocity = {0.0f, 0.0f, 0.0f};
	// 色
	p.color = { 1.0f, 0.5f, 0.2f, 1.0f };
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = rand->RandomValue(0.05f, 0.05f);
	// 初期スケール
	p.initScale = p.transform.scale;

	return p;
}

void MuzzleFlashParticle::UpdateParticle(MuzzleFlashParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 拡縮
	float easeT = Easing::EaseOutExpo(1.0f - t);
	p.transform.scale.x = Easing::Lerp(p.initScale.x, p.initScale.x * 1.4f, easeT); // 横に引き伸ばす
	p.transform.scale.y = Easing::Lerp(p.initScale.y, p.initScale.y * 0.4f, easeT); // 縦に潰す
}