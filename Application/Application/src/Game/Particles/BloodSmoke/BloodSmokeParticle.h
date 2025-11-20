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
struct BloodSmokeParticleData {
	Transform transform; /* 位置 */
	Float3 velocity;     /* 速度ベクトル */
	Float4 color;        /* 色 */
	float lifeTime;      /* 生存時間 */
	float currentTime;   /* 経過時間 */

	Float3 initScale;     /* 初期スケール */
	Float3 rotationSpeed; /* 回転速度 */
};

// =========================================================
// 弾衝突時の血煙パーティクルクラス
// 弾が敵に当たったときに発生
// =========================================================
class BloodSmokeParticle : public BaseParticleEffect<BloodSmokeParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	BloodSmokeParticle(ModelManager::ModelData& model);

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
	BloodSmokeParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(BloodSmokeParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kPositionOffsetMin = {-0.25f, -0.25f, -0.25f}; /* 位置オフセット最小値 */
	static constexpr Float3 kPositionOffsetMax = {0.25f, 0.25f, 0.25f};    /* 位置オフセット最大値 */
	static constexpr float kMinScale = 0.2f;                               /* 最小スケール */
	static constexpr float kMaxScale = 0.4f;                               /* 最大スケール */
	static constexpr float kDirectionSpread = 1.0f;                        /* 方向のバラつき */
	static constexpr float kMinSpeed = 1.0f;                               /* 最小スピード */
	static constexpr float kMaxSpeed = 6.0f;                               /* 最大スピード */
	static constexpr Float4 kInitialColor = {0.25f, 0.0f, 0.0f, 0.75f};    /* 初期色 */
	static constexpr float kMinLifeTime = 0.5f;                            /* 最小生存時間（秒） */
	static constexpr float kMaxLifeTime = 0.75f;                           /* 最大生存時間（秒） */
	static constexpr Float3 kRotationSpeedMin = {-1.0f, -1.0f, -1.0f};     /* 最小回転速度 */
	static constexpr Float3 kRotationSpeedMax = {1.0f, 1.0f, 1.0f};        /* 最大回転速度 */

	static constexpr float kEndScale = 0.5f;      /* 最終スケール */
	static constexpr float kInitialAlpha = 0.75f; /* 初期アルファ値 */
};