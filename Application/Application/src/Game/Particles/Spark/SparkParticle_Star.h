#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>
#include <Engine/Model/ModelManager.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct SparkParticleStarData
{
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;
};

/// <summary>
/// 火花パーティクル（星型）
/// </summary>
class SparkParticle_Star : public BaseParticleEffect<SparkParticleStarData> 
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SparkParticle_Star(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	SparkParticleStarData CreateParticle(const Float3& pos) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(SparkParticleStarData& p, float dt) override;
};
