#pragma once

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
	// Constants
	// =========================================================
	static constexpr float kRadius = 0.2f;      /* 弾の半径 */
	static constexpr float kMaxLifeTime = 1.0f; /* 生存時間（秒）*/
	static constexpr int32_t kDamage = 5;       /* 攻撃力 */
	static constexpr float kSpeed = 1.8f;       /* 弾速 */
};