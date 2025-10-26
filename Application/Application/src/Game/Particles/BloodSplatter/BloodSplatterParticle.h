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
	Transform transform;			/* 位置 */
	Float3 velocity;				/* 速度ベクトル */
	Float4 color;					/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */

	Float3 initScale;				/* 初期スケール */
	bool isStop = false;			/* 動きが停止したかのフラグ */
	float stopTime;					/* 動きを止めた時点での経過時間を記録 */
};

// =========================================================
// 血が飛び散るパーティクルクラス
// プレイヤー死亡時に発生
// =========================================================
class BloodSplatterParticle : public BaseParticleEffect<BloodSplatterParticleData>
{
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
	BloodSplatterParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(BloodSplatterParticleData& p, float dt) override;
};

