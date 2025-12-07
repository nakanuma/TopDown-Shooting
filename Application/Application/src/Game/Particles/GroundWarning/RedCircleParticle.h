#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct RedCircleParticleData {
	Cygnus::Transform transform;	/* 位置 */
	Cygnus::Float3 velocity;		/* 速度ベクトル */
	Cygnus::Float4 color;			/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */

	Cygnus::Float3 targetScale; /* 目標スケール */
};

// =========================================================
// 赤い円パーティクルクラス
// ボスの警告攻撃で発生
// =========================================================
class RedCircleParticle : public Cygnus::BaseParticleEffect<RedCircleParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	RedCircleParticle(Cygnus::ModelManager::ModelData& model);

protected:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// パーティクル固有の生成処理を行います。
	/// </summary>
	/// <param name="pos">初期位置</param>
	/// <param name="velocity">速度ベクトル</param>
	/// <param name="angle">初期回転角</param>
	/// <returns>パーティクルデータ</returns>
	RedCircleParticleData CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(RedCircleParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float4 kInitialColor = {1.0f, 0.0f, 0.0f, 1.0f};	/* 初期色（赤） */
	static constexpr float kLifeTime = 1.0f;									/* 生存時間（秒） */
	static constexpr Cygnus::Float3 kTargetScale = {5.0f, 5.0f, 5.0f};			/* 目標スケール */
};
