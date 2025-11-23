#include "BloodScatterParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

BloodScatterParticle::BloodScatterParticle(ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices_.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices_.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

BloodScatterParticleData BloodScatterParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	BloodScatterParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置
	p.transform.translate_ = pos;
	// スケール
	p.transform.scale_ = kScale;
	// 速度ベクトル
	Float3 baseDir = Float3::Normalize(velocity) * -1.0f;                                                                         // 引数で受け取った方向と逆向きにする
	Float3 randDir = rand->RandomValue({ -kDirectionSpread, 0.0f, -kDirectionSpread }, { kDirectionSpread, 0.0f, kDirectionSpread }); // 方向をバラつかせるためのオフセット
	p.velocity = Float3::Normalize(baseDir + randDir) * rand->RandomValue(kMinSpeed, kMaxSpeed);
	// 回転（進行方向を向くように）
	Float3 dir = Float3::Normalize(p.velocity);
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

void BloodScatterParticle::UpdateParticle(BloodScatterParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 移動
	float moveFactor = Easing::EaseOutQuart(1.0f - t);
	p.transform.translate_ += (p.velocity * moveFactor * dt);

	// 縮小
	float easeT = Easing::EaseInQuart(t);
	p.transform.scale_.z = p.initScale.z * (1.0f - easeT);
}
