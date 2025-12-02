#include "MuzzleFlashParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

MuzzleFlashParticle::MuzzleFlashParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {true, true, true};
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

MuzzleFlashParticleData MuzzleFlashParticle::CreateParticle(const Float3& pos, const Float3& velocity, float angle) {
	MuzzleFlashParticleData p;
	auto rand = RandomGenerator::GetInstance();

	float randAngle = rand->RandomValue(0.0f, PIf * 2.0f);

	float scaleX = rand->RandomValue(kMinScaleX, kMaxScaleX);
	float scaleY = rand->RandomValue(kMinScaleY, kMaxScaleY);

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, randAngle};
	// スケール
	p.transform.scale_ = {scaleX, scaleY, kInitialScaleZ};
	// 速度ベクトル
	p.velocity = {0.0f, 0.0f, 0.0f};
	// 色
	p.color = kInitialColor;
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = kLifeTime;
	// 初期スケール
	p.initScale = p.transform.scale_;

	return p;
}

void MuzzleFlashParticle::UpdateParticle(MuzzleFlashParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 拡縮
	float easeT = Easing::EaseOutExpo(1.0f - t);
	p.transform.scale_.x = Easing::Lerp(p.initScale.x, p.initScale.x * kScaleXMultipiler, easeT); // 横に引き伸ばす
	p.transform.scale_.y = Easing::Lerp(p.initScale.y, p.initScale.y * kScaleYMultiplier, easeT); // 縦に潰す
}