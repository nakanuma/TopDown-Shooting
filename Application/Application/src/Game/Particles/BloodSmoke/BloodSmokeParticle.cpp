#include "BloodSmokeParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

BloodSmokeParticle::BloodSmokeParticle(Cygnus::ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = Cygnus::BlendMode::Normal;
}

BloodSmokeParticleData BloodSmokeParticle::CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) {
	BloodSmokeParticleData p;
	auto rand = Cygnus::RandomGenerator::GetInstance();

	// 位置
	Cygnus::Float3 offset = rand->RandomValue(kPositionOffsetMin, kPositionOffsetMax);
	p.transform.translate_ = pos + offset;
	// スケール
	float scale = rand->RandomValue(kMinScale, kMaxScale);
	p.transform.scale_ = {scale, scale, scale};
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, 0.0f};
	// 速度ベクトル
	Cygnus::Float3 baseDir = Cygnus::Float3::Normalize(velocity) * -1.0f;                                                                         // 引数で受け取った方向と逆向きにする
	Cygnus::Float3 randDir = rand->RandomValue({-kDirectionSpread, 0.0f, -kDirectionSpread}, {kDirectionSpread, 0.0f, kDirectionSpread}); // 方向をバラつかせるためのオフセット
	p.velocity = Cygnus::Float3::Normalize(baseDir + randDir) * rand->RandomValue(kMinSpeed, kMaxSpeed);
	// 色
	p.color = kInitialColor;
	// 生存時間
	p.lifeTime = rand->RandomValue(kMinLifeTime, kMaxLifeTime);
	// 経過時間
	p.currentTime = 0.0f;
	// 初期スケール
	p.initScale = p.transform.scale_;
	// 回転速度
	p.rotationSpeed = rand->RandomValue(kRotationSpeedMin, kRotationSpeedMax);

	return p;
}

void BloodSmokeParticle::UpdateParticle(BloodSmokeParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	///
	///	移動
	///

	float moveFactor = Cygnus::Easing::EaseOutQuad(1.0f - t);
	p.transform.translate_ += p.velocity * moveFactor * dt;

	///
	///	回転
	///

	float damping = Cygnus::Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate_ += p.rotationSpeed * damping * dt;

	///
	///	拡大
	///

	float startScale = p.initScale.x;
	float scale = Cygnus::Easing::EaseInQuad(t) * (kEndScale - startScale) + startScale;
	p.transform.scale_ = {scale, scale, scale};

	///
	/// 透明化
	///

	float alpha = kInitialAlpha - Cygnus::Easing::EaseInQuart(t);
	p.color.w = alpha;
}
