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
struct RedCircleParticleData {
	Transform transform;		/* 位置 */
	Float3 velocity;			/* 速度ベクトル */
	Float4 color;				/* 色 */
	float lifeTime;				/* 生存時間 */
	float currentTime;			/* 経過時間 */

	Float3 targetScale;			/* 目標スケール */
};

// =========================================================
// 赤い円パーティクルクラス
// ボスの警告攻撃で発生
// =========================================================
class RedCircleParticle : public BaseParticleEffect<RedCircleParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	RedCircleParticle(ModelManager::ModelData& model);

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
	RedCircleParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(RedCircleParticleData& p, float dt) override;
};
