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
struct ImpactSmokeParticleData {
	Cygnus::Transform transform;	/* 位置 */
	Cygnus::Float3 velocity;		/* 速度ベクトル */
	Cygnus::Float4 color;			/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */

	Cygnus::Float3 initScale;		/* 初期スケール */
	Cygnus::Float3 rotationSpeed;	/* 回転速度 */
};

// =========================================================
// 弾衝突時の煙パーティクルクラス
// 弾が障害物に当たったときに発生
// =========================================================
class ImpactSmokeParticle : public Cygnus::BaseParticleEffect<ImpactSmokeParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	ImpactSmokeParticle(Cygnus::ModelManager::ModelData& model);

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
	ImpactSmokeParticleData CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(ImpactSmokeParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kMinOffset = {-0.25f, -0.25f, -0.25f};		/* 最小位置オフセット */
	static constexpr Cygnus::Float3 kMaxOffset = {0.25f, 0.25f, 0.25f};			/* 最大位置オフセット */
	static constexpr float kMinScale = 0.2f;									/* 最小スケール */
	static constexpr float kMaxScale = 0.4f;									/* 最大スケール */
	static constexpr float kDirectionSpread = 1.0f;								/* 方向のバラつき */
	static constexpr float kMinSpeed = 1.0f;									/* 最小速度 */
	static constexpr float kMaxSpeed = 6.0f;									/* 最大速度 */
	static constexpr Cygnus::Float4 kInitialColor = {0.5f, 0.5f, 0.5f, 1.0f};	/* 初期色（灰） */
	static constexpr float kMinLifeTime = 0.25f;								/* 最小生存時間 */
	static constexpr float kMaxLifeTime = 0.5f;									/* 最大生存時間 */
	static constexpr Cygnus::Float3 kMinRotationSpeed = {-1.0f, -1.0f, -1.0f};	/* 最小回転速度 */
	static constexpr Cygnus::Float3 kMaxRotationSpeed = {1.0f, 1.0f, 1.0f};		/* 最大回転速度 */

	static constexpr float kEndScale = 0.5f; /* 最終スケール */
};
