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
struct BloodScatterParticleData {
	Transform transform; /* 位置 */
	Float3 velocity;     /* 速度ベクトル */
	Float4 color;        /* 色 */
	float lifeTime;      /* 生存時間 */
	float currentTime;   /* 経過時間 */
	Float3 initScale;    /* 初期スケール */
};

// =========================================================
// 血が飛散するパーティクルクラス
// プレイヤーや敵に弾が当たったときに発生
// =========================================================
class BloodScatterParticle : public BaseParticleEffect<BloodScatterParticleData>
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	BloodScatterParticle(ModelManager::ModelData& model);

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
	BloodScatterParticleData CreateParticle(const Float3& pos, const Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(BloodScatterParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kScale = { 0.09f, 0.09f, 0.6f };				/* スケール */
	static constexpr float kDirectionSpread = 0.4f;							/* 方向のバラつき */
	static constexpr float kMinSpeed = 12.0f;								/* 最小スピード */
	static constexpr float kMaxSpeed = 24.0f;								/* 最大スピード */
	static constexpr Float4 kInitialColor = { 0.25f, 0.0f, 0.0f, 1.0f };	/* 初期色 */
	static constexpr float kMinLifeTime = 0.3f;								/* 最小生存時間（秒） */
	static constexpr float kMaxLifeTime = 0.5f;								/* 最大生存時間（秒） */
};