#include "ShellEjectionParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

ShellEjectionParticle::ShellEjectionParticle(ModelManager::ModelData& model)
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

ShellEjectionParticleData ShellEjectionParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	ShellEjectionParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置
	p.transform.translate_ = pos;
	// 回転
	p.transform.rotate_ = { 0.0f, 0.0f, 0.0f };
	// スケール（縦長の形状）
	p.transform.scale_ = kInitialScale;
	// 速度ベクトル
	Float3 rightDir = { cosf(angle), 0.0f, -sinf(angle) };
	Float3 upDir = { 0.0f, 1.0f, 0.0f };
	p.velocity =
		rightDir * rand->RandomValue(kMinRightSpeed, kMaxRightSpeed) + // 右方向
		upDir * rand->RandomValue(kMinUpSpeed, kMaxUpSpeed); // 上方向
	// 色
	p.color = kInitialColor;
	// 経過時間
	p.currentTime = 0.0f;
	// 生存時間
	p.lifeTime = kLifeTime;
	// 回転速度
	p.rotationSpeed = rand->RandomValue(kMinRotationSpeed, kMaxRotationSpeed);

	return p;
}

void ShellEjectionParticle::UpdateParticle(ShellEjectionParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 速度更新
	p.velocity.y += kGravity * dt;

	// 位置更新
	p.transform.translate_ += p.velocity * dt;

	// 回転
	p.transform.rotate_ += p.rotationSpeed * dt;
}
