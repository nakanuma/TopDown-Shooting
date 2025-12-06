#include "RedCircleParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

RedCircleParticle::RedCircleParticle(Cygnus::ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = Cygnus::BlendMode::Normal;
}

RedCircleParticleData RedCircleParticle::CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) {
	RedCircleParticleData p;

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = { Cygnus::PIf / 2.0f, 0.0f, 0.0f}; // 仰向けになるように
	// スケール
	p.transform.scale_ = {0.0f, 0.0f, 0.0f};
	// 速度ベクトル
	p.velocity = {0.0f, 0.0f, 0.0f};
	// 色
	p.color = kInitialColor;
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = kLifeTime;
	// 目標スケール
	p.targetScale = kTargetScale; // 地面警告攻撃の弾と同じサイズ
	return p;
}

void RedCircleParticle::UpdateParticle(RedCircleParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 拡大
	float easeT = Cygnus::Easing::EaseOutExpo(t);
	p.transform.scale_ = p.targetScale * easeT;
}
