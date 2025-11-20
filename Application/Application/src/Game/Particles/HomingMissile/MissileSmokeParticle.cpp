#include "MissileSmokeParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

MissileSmokeParticle::MissileSmokeParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

MissileSmokeParticleData MissileSmokeParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	MissileSmokeParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置（オフセットを加える）
	Float3 offset = rand->RandomValue(kMinOffset, kMaxOffset);
	p.transform.translate_ = pos + offset;
	// 回転
	p.transform.rotate_ = rand->RandomValue({0.0f, 0.0f, 0.0f}, {PIf * 2.0f, PIf * 2.0f, PIf * 2.0f});
	// スケール
	p.transform.scale_ = kInitialScale;
	// 速度ベクトル
	p.velocity = rand->RandomValue(kMinVelocity, kMaxVelocity);
	// 色
	p.color = kInitialColor;
	// 生存時間
	p.lifeTime = kLifeTime;
	// 経過時間
	p.currentTime = 0.0f;
	// 初期スケール
	p.initScale = p.transform.scale_;

	return p;
}

void MissileSmokeParticle::UpdateParticle(MissileSmokeParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 移動
	p.transform.translate_ += (p.velocity * dt);

	// 縮小
	if (t > kShrinkStartThreshold) {                                  // 4/5に到達したら
		float localT = (t - kShrinkStartThreshold) / kShrinkDuration; // 0~1に正規化
		float easeT = Easing::EaseInQuad(localT);
		p.transform.scale_ = p.initScale * (1.0f - easeT);
	}

	// 色
	Float4 color;
	if (t < kColorPhase1End) {
		// 白->橙
		float localT = t / kColorPhase1Duration; // 0~1に正規化
		color.x = Easing::Lerp(kColorWhite.x, kColorOrange.x, localT);
		color.y = Easing::Lerp(kColorWhite.y, kColorOrange.y, localT);
		color.z = Easing::Lerp(kColorWhite.z, kColorOrange.z, localT);
	} else if (t < kColorPhase2End) {
		// 橙->赤
		float localT = (t - kColorPhase1Duration) / kColorPhase2Duration; // 0~1に正規化
		color.x = Easing::Lerp(kColorOrange.x, kColorRed.x, localT);
		color.y = Easing::Lerp(kColorOrange.y, kColorRed.y, localT);
		color.z = Easing::Lerp(kColorOrange.z, kColorRed.z, localT);
	} else {
		// 赤->黒
		float localT = (t - kColorPhase2End) / kColorPhase3Duration; // 0~1に正規化
		color.x = Easing::Lerp(kColorRed.x, kColorBlack.x, localT);
		color.y = Easing::Lerp(kColorRed.y, kColorBlack.y, localT);
		color.z = Easing::Lerp(kColorRed.z, kColorBlack.z, localT);
	}
	color.w = 1.0f;

	p.color = color;
}
