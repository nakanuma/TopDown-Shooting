#include "RedCircleParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

RedCircleParticle::RedCircleParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

RedCircleParticleData RedCircleParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	RedCircleParticleData p;

	// 位置
	p.transform.translate = pos;
	// 回転
	p.transform.rotate = {PIf / 2.0f, 0.0f, 0.0f}; // 仰向けになるように
	// スケール
	p.transform.scale = {0.0f, 0.0f, 0.0f};
	// 速度ベクトル
	p.velocity = {0.0f, 0.0f, 0.0f};
	// 色
	p.color = {1.0f, 0.0f, 0.0f, 1.0f};
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = 1.0f;
	// 目標スケール
	p.targetScale = {5.0f, 5.0f, 5.0f}; // 地面警告攻撃の弾と同じサイズ
	return p;
}

void RedCircleParticle::UpdateParticle(RedCircleParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 拡大
	float easeT = Easing::EaseOutExpo(t);
	p.transform.scale = p.targetScale * easeT;
}
