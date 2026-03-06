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
struct GunOverheatSmokeParticleData {
	Cygnus::Transform transform; /* 位置 */
	Cygnus::Float3 velocity;     /* 速度ベクトル */
	Cygnus::Float4 color;        /* 色 */
	float lifeTime;              /* 生存時間 */
	float currentTime;           /* 経過時間 */

	Cygnus::Float3 initScale;     /* 初期スケール */
	float ascendSpeed;            /* 上昇速度 */
	Cygnus::Float3 rotationSpeed; /* 回転速度 */
};

// =========================================================
// オーバーヒート時の銃からでる煙パーティクルクラス
// =========================================================
class GunOverheatSmokeParticle : public Cygnus::BaseParticleEffect<GunOverheatSmokeParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	GunOverheatSmokeParticle(Cygnus::ModelManager::ModelData& model);

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
	GunOverheatSmokeParticleData CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(GunOverheatSmokeParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitialScale = {0.25f, 0.25f, 0.25f};	   /* 初期スケール */
	static constexpr Cygnus::Float3 kMinVelocity = {-1.0f, -5.0f, -1.0f};      /* 最小速度ベクトル */
	static constexpr Cygnus::Float3 kMaxVelocity = {1.0f, 5.0f, 1.0f};         /* 最大速度ベクトル */
	static constexpr Cygnus::Float4 kInitialColor = {1.0f, 1.0f, 0.0f, 1.0f};  /* 初期色（黄） */
	static constexpr float kMinLifeTime = 2.0f;                                /* 最小生存時間（秒） */
	static constexpr float kMaxLifeTime = 3.0f;                                /* 最大生存時間（秒） */
	static constexpr float kMinAscendSpeed = 4.0f;                             /* 最小上昇速度 */
	static constexpr float kMaxAscendSpeed = 12.0f;                            /* 最大上昇速度 */
	static constexpr Cygnus::Float3 kMinRotationSpeed = {-3.0f, -3.0f, -3.0f}; /* 最小回転速度 */
	static constexpr Cygnus::Float3 kMaxRotationSpeed = {3.0f, 3.0f, 3.0f};    /* 最大回転速度 */

	static constexpr float kColorPhase1End = 0.25f;      /* 色変更フェーズ1終了（黄->赤） */
	static constexpr float kColorPhase2End = 0.5f;       /* 色変更フェーズ2終了（赤->灰） */
	static constexpr float kColorPhase1Duration = 0.25f; /* 色変更フェーズ1の時間 */
	static constexpr float kColorPhase2Duration = 0.25f; /* 色変更フェーズ2の時間 */
	static constexpr float kColorPhase3Duration = 0.5f;  /* 色変更フェーズ3の時間 */

	static constexpr Cygnus::Float4 kColorYellow = {1.0f, 1.0f, 0.0f, 1.0f}; /* 黄色 */
	static constexpr Cygnus::Float4 kColorRed = {1.0f, 0.0f, 0.0f, 1.0f};    /* 赤色 */
	static constexpr Cygnus::Float4 kColorGray = {0.1f, 0.1f, 0.1f, 1.0f};   /* 灰色 */
};
