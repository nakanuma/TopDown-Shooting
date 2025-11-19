#pragma once

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
struct MuzzleFlashParticleData {
	Transform transform; /* 位置 */
	Float3 velocity;     /* 速度ベクトル */
	Float4 color;        /* 色 */
	float lifeTime;      /* 生存時間 */
	float currentTime;   /* 経過時間 */

	Float3 initScale; /* 初期スケール */
};

// =========================================================
// マズルフラッシュパーティクルクラス
// 射撃時に銃からに発生
// =========================================================
class MuzzleFlashParticle : public BaseParticleEffect<MuzzleFlashParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	MuzzleFlashParticle(ModelManager::ModelData& model);

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
	MuzzleFlashParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(MuzzleFlashParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kMinScaleX = 0.4f;                         /* 最小Xスケール */
	static constexpr float kMaxScaleX = 0.5f;                         /* 最大Xスケール */
	static constexpr float kMinScaleY = 0.8f;                         /* 最小Yスケール */
	static constexpr float kMaxScaleY = 1.0f;                         /* 最大Yスケール */
	static constexpr float kInitialScaleZ = 1.0f;                     /* 初期Zスケール */
	static constexpr Float4 kInitialColor = {1.0f, 0.5f, 0.2f, 1.0f}; /* 初期色 */
	static constexpr float kLifeTime = 0.05f;                         /* 生存時間（秒） */

	static constexpr float kScaleXMultipiler = 1.4f; /* X方向の拡大倍率 */
	static constexpr float kScaleYMultiplier = 0.4f; /* Y方向の縮小倍率 */
};
