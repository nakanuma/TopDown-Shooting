#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct SmokeParticleData {
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
/// タイトルのタンクから出る煙に使用
/// </summary>
class SmokeParticle : public BaseParticleEffect<SmokeParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SmokeParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	SmokeParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(SmokeParticleData& p, float dt) override;
};

