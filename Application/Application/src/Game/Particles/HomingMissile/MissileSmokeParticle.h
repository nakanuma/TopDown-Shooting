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
struct MissileSmokeParticleData {
	Transform transform;			/* 位置 */
	Float3 velocity;				/* 速度ベクトル */
	Float4 color;					/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */

	Float3 initScale;				/* 経過時間 */			
};

// =========================================================
// ミサイルの煙パーティクルクラス
// ボスの追尾ミサイルから発生
// =========================================================
class MissileSmokeParticle : public BaseParticleEffect<MissileSmokeParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	MissileSmokeParticle(ModelManager::ModelData& model);

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
	MissileSmokeParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(MissileSmokeParticleData& p, float dt) override;
};
