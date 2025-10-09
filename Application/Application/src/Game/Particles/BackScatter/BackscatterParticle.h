#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct BackscatterParticleData {
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	Float3 initScale;
};

/// <summary>
/// 後ろ方向に飛散するパーティクル。障害物や敵に弾が当たったときに発生
/// </summary>
class BackscatterParticle : public BaseParticleEffect<BackscatterParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	BackscatterParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	BackscatterParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(BackscatterParticleData& p, float dt) override;
};

