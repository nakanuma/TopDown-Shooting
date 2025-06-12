#pragma once

// Engine
#include <Engine/Math/MyMath.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>
#include <Engine/Model/ModelManager.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct CircleParticleExpandData
{
	Transform transform;
	Float3 velocity;
	Float4 color;
	float lifeTime;
	float currentTime;

	// その他固有パラメーター追加
	Float3 initScale;
};

/// <summary>
/// 円パーティクル（拡大）
/// </summary>
class CircleParticle_Expand : public BaseParticleEffect<CircleParticleExpandData> 
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	CircleParticle_Expand(ModelManager::ModelData& model);

private:
	/// <summary>
	/// パーティクル固有の生成処理
	/// </summary>
	CircleParticleExpandData CreateParticle(const Float3& pos) override;

	/// <summary>
	/// パーティクル固有の更新処理
	/// </summary>
	void UpdateParticle(CircleParticleExpandData& p, float dt) override;
};
