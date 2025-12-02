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
struct TeleporterRingParticleData {
	Transform transform;		/* 位置 */
	Float3 velocity;			/* 速度ベクトル */
	Float4 color;				/* 色 */
	float lifeTime;				/* 生存時間 */
	float currentTime;			/* 経過時間 */
};

// =========================================================
// テレポーターリングパーティクルクラス
// 有効状態のテレポーターから発生
// =========================================================
class TeleporterRingParticle : public BaseParticleEffect<TeleporterRingParticleData>
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	TeleporterRingParticle(ModelManager::ModelData& model);

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
	TeleporterRingParticleData CreateParticle(const Float3& pos, const Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(TeleporterRingParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kDefaultScale = {4.0f, 4.0f, 4.0f};			/* デフォルトスケール */
	static constexpr Float3 kDefaultVelocity = {0.0f, 2.5f, 0.0f};		/* デフォルト速度ベクトル */
	static constexpr Float4 kDefaultColor = {0.0f, 0.5f, 1.0f, 1.0f};	/* デフォルトカラー */
	static constexpr float kLifeTime = 3.0f;							/* 生存時間 */
	static constexpr float kInitialAlpha = kDefaultColor.w;				/* 初期アルファ値 */
};

