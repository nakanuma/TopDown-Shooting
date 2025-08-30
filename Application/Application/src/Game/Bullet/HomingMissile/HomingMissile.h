#pragma once

// Application
#include <src/Game/Bullet/Base/Bullet.h>

class Player;

/// <summary>
/// 追尾ミサイル（ボスの第一形態が使用）
/// </summary>
class HomingMissile : public Bullet, public ICollisionCallback {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// プレイヤーのセット
	/// </summary>
	void SetPlayer(Player* player) { targetPlayer_ = player; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

private:
	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// コライダーサイズ
	Float3 colliderSize_ = { 0.5f, 0.5f, 2.5f };

	// 経過時間
	float elapsedTime_ = 0.0f;
	// 生存時間
	const float kMaxLifeTime = 5.0f; // 5秒で消える

	// 旋回速度
	const float kTurnSpeed = 0.06f;

	// プレイヤーのポインタ
	Player* targetPlayer_;
};

