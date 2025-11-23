#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <deque>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/Base/Bullet.h>

// =========================================================
// プレイヤーの通常弾クラス
// =========================================================
class PlayerBullet : public Bullet, public ICollisionCallback {
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
	void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) override;

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

	/// <summary>
	/// 弾道の描画処理を行います。
	/// </summary>
	void DrawTrail();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kRadius = 0.2f;      /* 弾の半径 */
	static constexpr float kMaxLifeTime = 1.0f; /* 生存時間（秒）*/
	static constexpr int32_t kDamage = 10;      /* 攻撃力 */
	static constexpr float kSpeed = 4.0f;       /* 弾速 */

	static constexpr size_t kMaxTrailPoints = 10;                          /* 保持する履歴数 */
	static constexpr float kTrailLineWidth = 0.5f;                         /* トレイル線の幅 */
	static constexpr Float4 kTrailHeadColor = {1.0f, 1.0f, 0.33f, 1.0f};   /* トレイル線の先頭色 */
	static constexpr Float4 kTrailTailColor = {1.0f, 0.215f, 0.06f, 0.0f}; /* トレイル線の末尾色 */

	static constexpr float kRaycastThreshold = 0.5f; /* レイキャスト判定の閾値 */

	static constexpr int32_t kBloodSplatterCount = 2;	/* 血飛沫パーティクルの発生数 */
	static constexpr int32_t kBloodSmokeCount = 5;		/* 血煙パーティクルの発生数 */
	static constexpr int32_t kBackscatterCount = 4;		/* 後方飛散パーティクルの発生数 */
	static constexpr int32_t kImpactSmokeCount = 7;		/* 衝撃煙パーティクルの発生数 */
	static constexpr int32_t kBloodScatterCount = 2;	/* 後方血飛散パーティクルの発生数 */

	// =========================================================
	// Member Variables
	// =========================================================
	float elapsedTime_ = 0.0f;         /* 経過時間 */
	std::deque<Float3> trailPoints_{}; /* 過去フレームの位置を保持 */
	Float3 prevPosition_;              /* 前フレームの位置 */
};