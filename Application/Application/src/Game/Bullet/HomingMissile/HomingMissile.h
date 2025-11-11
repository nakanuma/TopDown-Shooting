#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/Base/Bullet.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// 追尾ミサイルクラス
// ボスの第一形態が使用。弾として扱う
// =========================================================
class HomingMissile : public Bullet, public ICollisionCallback {
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

	/// <summary>
	/// プレイヤーのポインタを設定します。
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player) { targetPlayer_ = player; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	const Float3 kColliderSize = {0.5f, 0.5f, 2.5f};		/* コライダーサイズ */

	float elapsedTime_ = 0.0f;								/* 経過時間 */
	const float kMaxLifeTime = 5.0f;						/* 生存時間（秒） */
	const float kTurnSpeed = 0.06f;							/* 旋回速度 */

	Player* targetPlayer_ = nullptr;						/* プレイヤーのポインタ */
};
