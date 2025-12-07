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
struct DeathCrossParticleData {
	Cygnus::Transform transform;	/* 位置 */
	Cygnus::Float3 velocity;		/* 速度ベクトル */
	Cygnus::Float4 color;			/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */
};

// =========================================================
// 死亡時のクロスパーティクルクラス
// 敵死亡時に発生
// =========================================================
class DeathCrossParticle : public Cygnus::BaseParticleEffect<DeathCrossParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	DeathCrossParticle(Cygnus::ModelManager::ModelData& model);

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
	DeathCrossParticleData CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(DeathCrossParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitialScale = {3.5f, 3.5f, 1.0f};			/* 初期スケール */
	static constexpr Cygnus::Float4 kInitialColor = {0.5f, 0.1f, 0.05f, 1.0f};	/* 初期色 */
	static constexpr float kLifeTime = 0.75f;									/* 生存時間 */

	static constexpr float kEndScaleY = 0.4f;      /* 最終Yスケール */
	static constexpr float kScaleDuration = 0.25f; /* スケール変更の時間 */
};
