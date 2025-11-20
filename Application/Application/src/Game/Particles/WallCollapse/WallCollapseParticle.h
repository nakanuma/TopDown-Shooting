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
struct WallCollapseParticleData {
	Transform transform;			/* 位置 */
	Float3 velocity;				/* 速度ベクトル */
	Float4 color;					/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */
};

// =========================================================
// 壁崩壊時パーティクルクラス
// ゲームスタート時演出で使用
// =========================================================
class WallCollapseParticle : public BaseParticleEffect<WallCollapseParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	WallCollapseParticle(ModelManager::ModelData& model);

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
	WallCollapseParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(WallCollapseParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kSpawnRange = { 11.5f, 2.5f, 0.5f };			/* スポーン範囲 */
	static constexpr float kMinScale = 0.2f;								/* 最小スケール */
	static constexpr float kMaxScale = 0.8f;								/* 最小スケール */
	static constexpr Float3 kMinVelocity = { -0.01f, 0.0f, -0.01f };		/* 最小速度ベクトル */
	static constexpr Float3 kMaxVelocity = { 0.01f, 0.3f, 0.01f };			/* 最大速度ベクトル */
	static constexpr float kMinSpeed = 15.0f;								/* 最小速度 */
	static constexpr float kMaxSpeed = 35.0f;								/* 最小速度 */
	static constexpr Float4 kInitialColor = { 0.53f, 0.53f, 0.53f, 1.0f };	/* 初期色（灰色） */
	static constexpr float kMinLifeTime = 2.0f;								/* 最小生存時間（秒） */
	static constexpr float kMaxLifeTime = 3.0f;								/* 最大生存時間（秒） */

	static constexpr Float3 kGravity = { 0.0f, -9.8f, 0.0f };				/* 重力加速度 */
};

