#include "CircleParticle_Expand.h"

// C++
#include <numbers>

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Util/RandomGenerator.h>

// ---------------------------------------------------------
// コンストラクタ
// ---------------------------------------------------------
CircleParticle_Expand::CircleParticle_Expand(ModelManager::ModelData& model) 
{
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = {true, true, true};
	// ブレンドモード設定
	blendMode_ = BlendMode::Add;
}

// ---------------------------------------------------------
// パーティクル固有の生成処理
// ---------------------------------------------------------
CircleParticleExpandData CircleParticle_Expand::CreateParticle(const Float3& pos) 
{ 
	CircleParticleExpandData p;
	p.transform.translate = pos;
	p.transform.rotate = {0.0f, 0.0f, 0.0f};
	p.transform.scale = {0.5f, 0.5f, 0.5f};
	p.velocity = {0.0f, 0.0f, 0.0f};
	p.color = {1.0f, 0.5f, 0.0f, 1.0f};
	p.lifeTime = 0.25f;
	p.currentTime = 0.0f;
	p.initScale = p.transform.scale;

	return p;
}

// ---------------------------------------------------------
// パーティクル固有の更新処理
// ---------------------------------------------------------
void CircleParticle_Expand::UpdateParticle(CircleParticleExpandData& p, float dt) 
{ 
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f); 
	
	// 拡大（初期値->初期値の500%）
	p.transform.scale = Float3::Lerp(p.initScale, p.initScale * 5.0f, Easing::EaseInQuad(t));

	// 透明化（1.0->0.0）
	p.color.w = Easing::Lerp(1.0f, 0.0f, Easing::EaseInExpo(t));
}
