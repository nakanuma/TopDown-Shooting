#include "SparkParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

SparkParticle::SparkParticle(Cygnus::ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = Cygnus::BlendMode::Normal;
}

SparkParticleData SparkParticle::CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) {
	SparkParticleData p;
	auto rand = Cygnus::RandomGenerator::GetInstance();

	// 位置
	p.transform.translate_ = pos;
	// スケール
	p.transform.scale_ = kInitialScale;
	// 速度ベクトル
	Cygnus::Float3 baseDir = Cygnus::Float3::Normalize(velocity) * -1.0f; // 引数で受け取った方向と逆向きにする
	Cygnus::Float3 randDir = rand->RandomValue({ -kDirectionSpread, 0.0f, -kDirectionSpread }, { kDirectionSpread, 0.0f, kDirectionSpread }); // 方向をバラつかせるためのオフセット
	p.velocity = Cygnus::Float3::Normalize(baseDir + randDir) * rand->RandomValue(kMinSpeed, kMaxSpeed);
	// 回転（進行方向を向くように）
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(p.velocity);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	p.transform.rotate_ = { -pitch, -yaw, 0.0f };
	// 色
	p.color = kInitialColor;
	// 生存時間
	p.lifeTime = rand->RandomValue(kMinLifeTime, kMaxLifeTime);
	// 経過時間
	p.currentTime = 0.0f;
	// 初期スケール
	p.initScale = p.transform.scale_;

	return p;
}

void SparkParticle::UpdateParticle(SparkParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 重力加速度を加える
	p.velocity += kGravity * dt;

	// 移動
	float moveFactor = Cygnus::Easing::EaseOutQuart(1.0f - t);
	p.transform.translate_ += (p.velocity * moveFactor * dt);

	// 縮小
	float easeT = Cygnus::Easing::EaseInQuad(t);
	p.transform.scale_.z = p.initScale.z * (1.0f - easeT);

	// 色
	Cygnus::Float4 color;
	if (t < kColorPhase1End) {
		// 前半 : 白->橙
		float localT = t / kColorPhase1Duration; // 0~1に正規化
		color.x = Cygnus::Easing::Lerp(kColorWhite.x, kColorOrange.x, localT);
		color.y = Cygnus::Easing::Lerp(kColorWhite.y, kColorOrange.y, localT);
		color.z = Cygnus::Easing::Lerp(kColorWhite.z, kColorOrange.z, localT);
	} else {
		// 後半 : 橙->赤
		float localT = (t - kColorPhase1End) / kColorPhase2Duration; // 0~1に正規化
		color.x = Cygnus::Easing::Lerp(kColorOrange.x, kColorRed.x, localT);
		color.y = Cygnus::Easing::Lerp(kColorOrange.y, kColorRed.y, localT);
		color.z = Cygnus::Easing::Lerp(kColorOrange.z, kColorRed.z, localT);
	}
	color.w = 1.0f;

	p.color = color;
}
