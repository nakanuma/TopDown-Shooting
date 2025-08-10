#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct WarningScatterParticleData {
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	Float3 initScale;
	Float3 rotationSpeed;
};

/// <summary>
/// ボスの地面警告攻撃の爆発後飛散パーティクル
/// </summary>
class WarningScatterParticle : public BaseParticleEffect<WarningScatterParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WarningScatterParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	WarningScatterParticleData CreateParticle(const Float3& pos, const Float3& velocity) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(WarningScatterParticleData& p, float dt) override;
};

