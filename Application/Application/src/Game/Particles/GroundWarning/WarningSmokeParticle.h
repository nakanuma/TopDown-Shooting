#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct WarningSmokeParticleData {
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
/// ボスの地面警告攻撃の爆発後煙パーティクル
/// </summary>
class WarningSmokeParticle : public BaseParticleEffect<WarningSmokeParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WarningSmokeParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	WarningSmokeParticleData CreateParticle(const Float3& pos, const Float3& velocity) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(WarningSmokeParticleData& p, float dt) override;
};

