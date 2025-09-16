#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct ExplodeSmokeParticleData {
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	Float3 initScale;
	float ascendSpeed;
	Float3 rotationSpeed;
};

/// <summary>
/// 爆発後煙パーティクル
/// </summary>
class ExplodeSmokeParticle : public BaseParticleEffect<ExplodeSmokeParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ExplodeSmokeParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	ExplodeSmokeParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(ExplodeSmokeParticleData& p, float dt) override;
};

