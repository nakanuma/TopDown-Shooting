#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct MissileSmokeParticleData {
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	Float3 initScale;
};

/// <summary>
/// ボスの追尾ミサイルの尾から出る煙パーティクル
/// </summary>
class MissileSmokeParticle : public BaseParticleEffect<MissileSmokeParticleData>
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	MissileSmokeParticle(ModelManager::ModelData& model);

protected:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	MissileSmokeParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(MissileSmokeParticleData& p, float dt) override;
};

