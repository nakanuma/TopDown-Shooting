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
struct BossFragmentsParticleData {
	Transform transform; /* 位置 */
	Float3 velocity;     /* 速度ベクトル */
	Float4 color;        /* 色 */
	float lifeTime;      /* 生存時間 */
	float currentTime;   /* 経過時間 */

	Float3 initScale;     /* 初期スケール */
	Float3 rotationSpeed; /* 回転速度 */
};

// =========================================================
// ボスの破片パーティクルクラス
// ボスのインスタンスが削除される直前に1度のみ発生
// =========================================================
class BossFragmentsParticle : public BaseParticleEffect<BossFragmentsParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	BossFragmentsParticle(ModelManager::ModelData& model);

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
	BossFragmentsParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(BossFragmentsParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kSpawnRange = {5.0f, 2.5f, 5.0f};          /* スポーン範囲 */
	static constexpr Float3 kMinScale = {0.1f, 0.05f, 0.1f};           /* 最小スケール */
	static constexpr Float3 kMaxScale = {1.0f, 0.1f, 1.0f};            /* 最大スケール */
	static constexpr Float3 kMinVelocityOffset = {-0.5f, 0.5f, -0.5f}; /* 最小速度オフセット */
	static constexpr Float3 kMaxVelocityOffset = {0.5f, 1.0f, 0.5f};   /* 最大速度オフセット */
	static constexpr float kMinSpeed = 15.0f;                          /* 最小速度 */
	static constexpr float kMaxSpeed = 20.0f;                          /* 最大速度 */
	static constexpr Float4 kInitialColor = {0.0f, 0.0f, 0.0f, 1.0f};  /* 初期色（黒） */
	static constexpr float kMinLifeTime = 1.0f;                        /* 最小生存時間 */
	static constexpr float kMaxLifeTime = 2.0f;                        /* 最大生存時間 */
	static constexpr Float3 kMinRotationSpeed = {-3.0f, -3.0f, -3.0f}; /* 最小回転速度 */
	static constexpr Float3 kMaxRotationSpeed = {3.0f, 3.0f, 3.0f};    /* 最大回転速度 */

	static constexpr Float3 kGravity = {0.0f, -9.8f, 0.0f}; /* 重力加速度 */
};