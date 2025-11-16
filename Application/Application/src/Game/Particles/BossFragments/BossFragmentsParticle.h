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
struct BossFragmentsParticleData {
	Transform transform;			/* 位置 */
	Float3 velocity;				/* 速度ベクトル */
	Float4 color;					/* 色 */
	float lifeTime;					/* 生存時間 */
	float currentTime;				/* 経過時間 */

	Float3 initScale;				/* 初期スケール */
	Float3 rotationSpeed;			/* 回転速度 */
};

// =========================================================
// ボスの破片パーティクルクラス
// ボスのインスタンスが削除される直前に1度のみ発生
// =========================================================
class BossFragmentsParticle : public BaseParticleEffect<BossFragmentsParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	BossFragmentsParticle(ModelManager::ModelData& model);

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
	BossFragmentsParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(BossFragmentsParticleData& p, float dt) override;
};