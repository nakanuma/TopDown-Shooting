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
// 敵の通常弾クラス
// =========================================================
class EnemyBullet : public Bullet, public ICollisionCallback {
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

	/// <summary>
	/// 弾道の描画処理を行います。
	/// </summary>
	void DrawTrail();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	const float kRadius = 0.2f;					/* 弾の半径 */

	float elapsedTime_ = 0.0f;					/* 経過時間 */
	const float kMaxLifeTime = 1.0f;			/* 生存時間（秒） */

	// ----- Trail -----
	std::deque<Float3> trailPoints_{};			/* 過去フレームの位置を保持 */
	const size_t kMaxTrailPoints = 10;			/* 保持する履歴数 */
};