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
struct SparkParticleData {
	Cygnus::Transform transform;		/* 位置 */
	Cygnus::Float3 velocity;			/* 速度ベクトル */
	Cygnus::Float4 color;				/* 色 */
	float lifeTime;						/* 生存時間 */
	float currentTime;					/* 経過時間 */

	Cygnus::Float3 initScale;			/* 初期スケール */
};

// =========================================================
// 火花パーティクルクラス
// タイトルのタンクから発生
// =========================================================
class SparkParticle : public Cygnus::BaseParticleEffect<SparkParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	SparkParticle(Cygnus::ModelManager::ModelData& model);

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
	SparkParticleData CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(SparkParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitialScale = { 0.04f, 0.04f, 0.8f };		/* 初期スケール */
	static constexpr float kDirectionSpread = 0.9f;								/* 方向のバラつき */
	static constexpr float kMinSpeed = 16.0f;									/* 最小速度 */
	static constexpr float kMaxSpeed = 24.0f;									/* 最大速度 */
	static constexpr Cygnus::Float4 kInitialColor = { 1.0f, 1.0f, 1.0f, 1.0f }; /* 初期色（白） */
	static constexpr float kMinLifeTime = 0.3f;									/* 最小生存時間 */
	static constexpr float kMaxLifeTime = 0.5f;									/* 最大生存時間 */

	static constexpr Cygnus::Float3 kGravity = { 0.0f, -9.8f, 0.0f }; /* 重力加速度 */

	static constexpr float kColorPhase1End = 0.5f;		/* 色変更フェーズ1終了（白->橙） */
	static constexpr float kColorPhase1Duration = 0.5f; /* 色変更フェーズ1の期間 */
	static constexpr float kColorPhase2Duration = 0.5f; /* 色変更フェーズ2の期間 */

	static constexpr Cygnus::Float4 kColorWhite = { 1.0f, 1.0f, 1.0f, 1.0f };	/* 白色 */
	static constexpr Cygnus::Float4 kColorOrange = { 1.0f, 0.5f, 0.0f, 1.0f };	/* 橙色 */
	static constexpr Cygnus::Float4 kColorRed = { 1.0f, 0.0f, 0.0f, 1.0f };		/* 赤色 */
};
