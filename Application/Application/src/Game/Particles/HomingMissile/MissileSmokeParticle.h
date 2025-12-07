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
	Cygnus::Transform transform; /* 位置 */
	Cygnus::Float3 velocity;     /* 速度ベクトル */
	Cygnus::Float4 color;        /* 色 */
	float lifeTime;				 /* 生存時間 */
	float currentTime;			 /* 経過時間 */

	Cygnus::Float3 initScale; /* 経過時間 */
};

// =========================================================
// ミサイルの煙パーティクルクラス
// ボスの追尾ミサイルから発生
// =========================================================
class MissileSmokeParticle : public Cygnus::BaseParticleEffect<MissileSmokeParticleData> {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	MissileSmokeParticle(Cygnus::ModelManager::ModelData& model);

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
	MissileSmokeParticleData CreateParticle(const Cygnus::Float3& pos, const Cygnus::Float3& velocity, float angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(MissileSmokeParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kMinOffset = {-0.4f, -0.4f, -0.4f};			/* 最小位置オフセット */
	static constexpr Cygnus::Float3 kMaxOffset = {0.4f, 0.4f, 0.4f};			/* 最大位置オフセット */
	static constexpr Cygnus::Float3 kInitialScale = {0.2f, 0.2f, 0.2f};			/* 初期スケール */
	static constexpr Cygnus::Float3 kMinVelocity = {-1.0f, -1.0f, -1.0f};		/* 最小速度ベクトル */
	static constexpr Cygnus::Float3 kMaxVelocity = {1.0f, 1.0f, 1.0f};			/* 最小速度ベクトル */
	static constexpr Cygnus::Float4 kInitialColor = {1.0f, 1.0f, 1.0f, 1.0f};	/* 初期色（白） */
	static constexpr float kLifeTime = 0.8f;									/* 生存時間（秒） */

	static constexpr float kShrinkStartThreshold = 0.8f; /* 縮小開始のしきい値（生存時間の80％） */
	static constexpr float kShrinkDuration = 0.2f;       /* 縮小期間（生存時間の20％） */

	static constexpr float kColorPhase1End = 1.0f / 3.0f;      /* 色変更フェーズ1終了（白->橙） */
	static constexpr float kColorPhase2End = 2.0f / 3.0f;      /* 色変更フェーズ2終了（橙->赤） */
	static constexpr float kColorPhase1Duration = 1.0f / 3.0f; /* 色変更フェーズ1の期間 */
	static constexpr float kColorPhase2Duration = 1.0f / 3.0f; /* 色変更フェーズ2の期間 */
	static constexpr float kColorPhase3Duration = 1.0f / 3.0f; /* 色変更フェーズ3の期間 */

	static constexpr Cygnus::Float4 kColorWhite = {1.0f, 1.0f, 1.0f, 1.0f};  /* 白色 */
	static constexpr Cygnus::Float4 kColorOrange = {1.0f, 0.5f, 0.0f, 1.0f}; /* 橙色 */
	static constexpr Cygnus::Float4 kColorRed = {1.0f, 0.0f, 0.0f, 1.0f};    /* 赤色 */
	static constexpr Cygnus::Float4 kColorBlack = {0.0f, 0.0f, 0.0f, 1.0f};  /* 黒色 */
};
