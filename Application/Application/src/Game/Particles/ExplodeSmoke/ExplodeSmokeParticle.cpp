#include "ExplodeSmokeParticle.h"

// Engine
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Math/Easing.h>

// ---------------------------------------------------------
// コンストラクタ
// ---------------------------------------------------------
ExplodeSmokeParticle::ExplodeSmokeParticle(ModelManager::ModelData& model)
{
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = { false, false, false };
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

// ---------------------------------------------------------
// パーティクル固有の生成処理
// ---------------------------------------------------------
ExplodeSmokeParticleData ExplodeSmokeParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle)
{
	ExplodeSmokeParticleData p;
	auto rand = RandomGenerator::GetInstance();

	Float3 offset = rand->RandomValue({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
	p.transform.translate = pos + offset;
	p.transform.rotate = { 0.0f, 0.0f, 0.0f };
	p.transform.scale = { 0.5f, 0.5f, 0.5f };
	p.velocity = rand->RandomValue({ -5.0f, -5.0f, -5.0f }, { 5.0f, 5.0f, 5.0f });
	p.color = { 1.0f, 1.0f, 0.0f, 1.0f };
	p.lifeTime = rand->RandomValue(2.0f, 3.0f);
	p.currentTime = 0.0f;
	
	p.initScale = p.transform.scale;
	p.ascendSpeed = rand->RandomValue(4.0f, 12.0f);
	p.rotationSpeed = rand->RandomValue({ -3.0f, -3.0f, -3.0f }, { 3.0f, 3.0f, 3.0f });

	return p;
}

// ---------------------------------------------------------
// パーティクル固有の更新処理
// ---------------------------------------------------------
void ExplodeSmokeParticle::UpdateParticle(ExplodeSmokeParticleData& p, float dt)
{
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);
	auto rand = RandomGenerator::GetInstance();

	///
	///	移動
	/// 
	
	// 横方向の速度を減衰
	float horizontalDamping = Easing::EaseInQuad(1.0f - t);
	Float3 horizontalVelocity = { p.velocity.x * horizontalDamping, 0.0f, p.velocity.z * horizontalDamping };

	// 上昇速度を徐々に増加
	float verticalVelocity = p.ascendSpeed * t;

	// 速度を合成
	Float3 currentVelocity = horizontalVelocity + Float3{ 0.0f, verticalVelocity, 0.0f };

	p.transform.translate += currentVelocity * dt;

	///
	///	回転
	/// 

	float damping = Easing::EaseOutQuad(1.0f - t);
	p.transform.rotate += p.rotationSpeed * damping * dt;

	///
	///	拡大
	/// 

	float startScale = p.initScale.x;
	float endScale = 1.0f;
	float scale = Easing::EaseInQuad(t) * (endScale - startScale) + startScale;
	p.transform.scale = { scale, scale, scale };

	///
	///	色の変更
	/// 
	
	Float4 color;
	if (t < 0.25f) {
		// 黄->赤（0~0.25）
		float localT = t / 0.25f; // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 1.0f, localT);
		color.y = Easing::Lerp(1.0f, 0.0f, localT);
		color.z = Easing::Lerp(0.0f, 0.0f, localT);
		color.w = 1.0f;
	} else if (t < 0.5f){
		// 赤->灰（0.25~0.5）
		float localT = (t - 0.25f) / 0.25f; // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 0.1f, localT);
		color.y = Easing::Lerp(0.0f, 0.1f, localT);
		color.z = Easing::Lerp(0.0f, 0.1f, localT);
		color.w = 1.0f;
	} else {
		// 0.5以降は灰で固定
		float localT = (t - 0.5f) / 0.5f;
		color.x = 0.1f;
		color.y = 0.1f;
		color.z = 0.1f;
		// 透明化
		color.w = 1.0f - localT;
	}

	p.color = color;
}
