#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>
#include <Engine/Model/ModelManager.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct SparkParticleShrinkData
{
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	// その他固有パラメーター
	Float3 initScale;
};

/// <summary>
/// 火花パーティクル（縮小）
/// </summary>
class SparkParticle_Shrink : public BaseParticleEffect<SparkParticleShrinkData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SparkParticle_Shrink(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	SparkParticleShrinkData CreateParticle(const Float3& pos) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(SparkParticleShrinkData& p, float dt) override;
};
