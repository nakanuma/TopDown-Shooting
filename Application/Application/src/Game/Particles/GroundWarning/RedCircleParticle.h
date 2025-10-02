#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct RedCircleParticleData {
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	Float3 targetScale;
};

/// <summary>
/// ボスの警告攻撃の赤い円
/// </summary>
class RedCircleParticle : public BaseParticleEffect<RedCircleParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RedCircleParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	RedCircleParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(RedCircleParticleData& p, float dt) override;
};

