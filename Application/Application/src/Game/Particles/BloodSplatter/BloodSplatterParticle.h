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
struct BloodSplatterParticleData {
	Transform transform; /* 位置 */
	Float3 velocity;     /* 速度ベクトル */
	Float4 color;        /* 色 */
	float lifeTime;      /* 生存時間 */
	float currentTime;   /* 経過時間 */

	Float3 initScale;    /* 初期スケール */
	bool isStop = false; /* 動きが停止したかのフラグ */
	float stopTime;      /* 動きを止めた時点での経過時間を記録 */
};

// =========================================================
// 血が飛び散るパーティクルクラス
// プレイヤー死亡時に発生
// =========================================================
class BloodSplatterParticle : public BaseParticleEffect<BloodSplatterParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	BloodSplatterParticle(ModelManager::ModelData& model);

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
	BloodSplatterParticleData CreateParticle(const Float3& pos, const Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(BloodSplatterParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kMinScale = 0.1f;                           /* 最小スケール */
	static constexpr float kMaxScale = 0.25f;                          /* 最大スケール */
	static constexpr Float3 kMinVelocity = {-12.0f, 2.0f, -12.0f};     /* 最小速度ベクトル */
	static constexpr Float3 kMaxVelocity = {12.0f, 4.0f, 12.0f};       /* 最大速度ベクトル */
	static constexpr Float4 kInitialColor = {0.25f, 0.0f, 0.0f, 1.0f}; /* 初期色 */
	static constexpr float kMinLifeTime = 3.5f;                        /* 最小生存時間（秒） */
	static constexpr float kMaxLifeTime = 4.5f;                        /* 最大生存時間（秒） */

	static constexpr Float3 kGravity = {0.0f, -9.8f, 0.0f}; /* 重力加速度 */
	static constexpr float kGravityMultiplier = 5.0f;       /* 重力倍率 */
	static constexpr float kGroundOffsetMultiplier = 2.0f;  /* 床のオフセット倍率 */
	static constexpr float kBounceFactor = 0.3f;            /* 反発係数 */
	static constexpr float kFriction = 0.9f;                /* 摩擦係数 */
	static constexpr float kMinBounceVelocity = 1.0f;       /* 跳ねる最小速度 */
};
