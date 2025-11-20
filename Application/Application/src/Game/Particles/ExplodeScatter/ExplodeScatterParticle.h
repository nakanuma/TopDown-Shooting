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
struct ExplodeScatterParticleData {
	Transform transform; /* 位置 */
	Float3 velocity;     /* 速度ベクトル */
	Float4 color;        /* 色 */
	float lifeTime;      /* 生存時間 */
	float currentTime;   /* 経過時間 */

	Float3 initScale;     /* 初期スケール */
	Float3 rotationSpeed; /* 回転速度 */
};

// =========================================================
// 爆発後飛散パーティクルクラス
// ボスの地面警告攻撃時に発生
// =========================================================
class ExplodeScatterParticle : public BaseParticleEffect<ExplodeScatterParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	ExplodeScatterParticle(ModelManager::ModelData& model);

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
	ExplodeScatterParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(ExplodeScatterParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kMinScale = {0.2f, 0.2f, 0.2f};            /* 最小スケール */
	static constexpr Float3 kMaxScale = {0.4f, 0.4f, 0.4f};            /* 最大スケール */
	static constexpr Float3 kMinVelocity = {-12.0f, 0.0f, -12.0f};     /* 最小速度ベクトル */
	static constexpr Float3 kMaxVelocity = {12.0f, 8.0f, 12.0f};       /* 最大速度ベクトル */
	static constexpr Float4 kInitialColor = {1.0f, 1.0f, 0.0f, 1.0f};  /* 初期色（黄） */
	static constexpr float kMinLifeTime = 0.8f;                        /* 最小生存時間 */
	static constexpr float kMaxLifeTime = 1.2f;                        /* 最大生存時間 */
	static constexpr Float3 kMinRotationSpeed = {-3.0f, -3.0f, -3.0f}; /* 最小回転速度 */
	static constexpr Float3 kMaxRotationSpeed = {3.0f, 3.0f, 3.0f};    /* 最大回転速度 */

	static constexpr Float3 kGravity = {0.0f, -9.8f, 0.0f};   /* 重力加速度 */
	static constexpr float kShrinkStartThreshold = 0.8f;      /* 縮小開始のしきい値（生存時間の80％時点） */
	static constexpr float kShrinkDuration = 0.2f;            /* 縮小期間（生存時間の20％） */
	static constexpr float kColorChangeStartThreshold = 0.5f; /* 色変更開始のしきい値（生存時間の50％時点） */
	static constexpr float kColorChangeDuration = 0.5f;       /* 色変更期間（生存時間の50％） */
	static constexpr float kGreenStart = 1.0f;                /* 緑成分の開始時値 */
	static constexpr float kGreenEnd = 0.5f;                  /* 緑成分の終了時値 */
};
