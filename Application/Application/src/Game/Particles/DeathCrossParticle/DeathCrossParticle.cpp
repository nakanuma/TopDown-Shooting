#include "DeathCrossParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

DeathCrossParticle::DeathCrossParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {true, true, true};
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

DeathCrossParticleData DeathCrossParticle::CreateParticle(const Float3& pos, const Float3& velocity, float angle) {
	DeathCrossParticleData p;

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, angle};
	// スケール
	p.transform.scale_ = kInitialScale;
	// 速度ベクトル
	p.velocity = {0.0f, 0.0f, 0.0f};
	// 色
	p.color = kInitialColor;
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = kLifeTime;

	return p;
}

void DeathCrossParticle::UpdateParticle(DeathCrossParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	///
	/// 縮小
	///

	// Yスケール
	float scaleT = std::clamp(p.currentTime / kScaleDuration, 0.0f, 1.0f);
	float easeT = Easing::EaseOutQuart(scaleT);

	p.transform.scale_.y = Easing::Lerp(kInitialScale.y, kEndScaleY, easeT);

	///
	/// 透明化
	///

	float alpha = 1.0f - Easing::EaseInQuart(t);
	p.color.w = alpha;
}
