#include "ExplodeSmokeParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

ExplodeSmokeParticle::ExplodeSmokeParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

ExplodeSmokeParticleData ExplodeSmokeParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	ExplodeSmokeParticleData p;
	auto rand = RandomGenerator::GetInstance();
	
	// 位置（Yにオフセットを加える）
	Float3 offset = rand->RandomValue(kMinOffset, kMaxOffset);
	p.transform.translate_ = pos + offset;
	// 回転
	p.transform.rotate_ = {0.0f, 0.0f, 0.0f};
	// スケール
	p.transform.scale_ = kInitialScale;
	// 速度ベクトル
	p.velocity = rand->RandomValue(kMinVelocity, kMaxVelocity);
	// 色
	p.color = kInitialColor;
	// 生存時間
	p.lifeTime = rand->RandomValue(kMinLifeTime, kMaxLifeTime);
	// 経過時間
	p.currentTime = 0.0f;
	// 初期スケール
	p.initScale = p.transform.scale_;
	// 上昇速度
	p.ascendSpeed = rand->RandomValue(kMinAscendSpeed, kMaxAscendSpeed);
	// 回転速度
	p.rotationSpeed = rand->RandomValue(kMinRotationSpeed, kMaxRotationSpeed);

	return p;
}

void ExplodeSmokeParticle::UpdateParticle(ExplodeSmokeParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);
	auto rand = RandomGenerator::GetInstance();

	///
	///	移動
	///

	// 横方向の速度を減衰
	float horizontalDamping = Easing::EaseInQuad(1.0f - t);
	Float3 horizontalVelocity = {p.velocity.x * horizontalDamping, 0.0f, p.velocity.z * horizontalDamping};

	// 上昇速度を徐々に増加
	float verticalVelocity = p.ascendSpeed * t;

	// 速度を合成
	Float3 currentVelocity = horizontalVelocity + Float3{0.0f, verticalVelocity, 0.0f};

	p.transform.translate_ += currentVelocity * dt;

	///
	///	回転
	///

	float damping = Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate_ += p.rotationSpeed * damping * dt;

	///
	///	拡大
	///

	float startScale = p.initScale.x;
	float scale = Easing::EaseInQuad(t) * (kEndScale - startScale) + startScale;
	p.transform.scale_ = {scale, scale, scale};

	///
	///	色の変更
	///

	Float4 color;
	if (t < kColorPhase1End) {
		// 黄->赤（0~0.25）
		float localT = t / kColorPhase1Duration; // 0~1に正規化
		color.x = Easing::Lerp(kColorYellow.x, kColorRed.x, localT);
		color.y = Easing::Lerp(kColorYellow.y, kColorRed.y, localT);
		color.z = Easing::Lerp(kColorYellow.z, kColorRed.z, localT);
		color.w = 1.0f;
	} else if (t < kColorPhase2End) {
		// 赤->灰（0.25~0.5）
		float localT = (t - kColorPhase2Duration) / kColorPhase2Duration; // 0~1に正規化
		color.x = Easing::Lerp(kColorRed.x, kColorGray.x, localT);
		color.y = Easing::Lerp(kColorRed.y, kColorGray.y, localT);
		color.z = Easing::Lerp(kColorRed.z, kColorGray.z, localT);
		color.w = 1.0f;
	} else {
		// 0.5以降は灰で固定
		float localT = (t - kColorPhase3Duration) / kColorPhase3Duration;
		color.x = kColorGray.x;
		color.y = kColorGray.y;
		color.z = kColorGray.z;
		// 透明化
		color.w = 1.0f - localT;
	}

	p.color = color;
}
