#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/Base/Bullet.h>

// =========================================================
// 地面警告攻撃クラス
// ボスの第一形態が使用。弾として扱う
// =========================================================
class GroundWarning : public Bullet, public ICollisionCallback {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 弾の初期化処理を行います。
	/// </summary>
	/// <param name="position">初期位置</param>
	/// <param name="direciton">方向</param>
	/// <param name="model">モデルデータ</param>
	void Initialize(const Float3& position, const Float3& direction, ModelManager::ModelData* model) override;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Collider* other) override;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kRadius = 5.0f;      /* 弾の半径 */
	static constexpr float kMaxLifeTime = 1.0f; /* 生存時間（秒） */
	static constexpr int32_t kDamage = 20;		/* 攻撃力 */
	static constexpr float kHitDelay = 0.8f;    /* 衝突判定が有効化されるまでの遅延時間 */

	static constexpr Float3 kParticleOffset = {0.0f, 1.5f, 0.0f}; /* パーティクル発生位置のオフセット */
	static constexpr int32_t kExplodeSmokeCount = 15;             /* 煙パーティクルの発生数 */
	static constexpr int32_t kExplodeScatterCount = 25;           /* 飛散パーティクルの発生数 */

	// =========================================================
	// Member Variables
	// =========================================================
	float elapsedTime_ = 0.0f;     /* 経過時間 */
	bool colliderEnabled_ = false; /* コライダー有効化フラグ */
};
