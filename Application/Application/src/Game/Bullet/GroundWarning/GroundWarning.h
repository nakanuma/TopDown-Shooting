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

	/// <summary>
	/// コライダーの更新処理を行います。
	/// </summary>
	void UpdateCollider();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	float radius_ = 5.0f;						/* 弾の半径 */

	float hitDelay_ = 0.8f;						/* 衝突判定を有効化するまでの遅延時間 */
	bool colliderEnabled_ = false;				/* コライダー有効化フラグ */

	float elapsedTime_ = 0.0f;					/* 経過時間 */
	const float kMaxLifeTime = 1.0f;			/* 生存時間（秒） */
};

