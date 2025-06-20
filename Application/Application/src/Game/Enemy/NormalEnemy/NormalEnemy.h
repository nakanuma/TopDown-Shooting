#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// ステート
/// </summary>
enum class EnemyState {
	Alert,   // 警戒中
	Move,   // 接近中
	Attack, // 攻撃中
};

/// <summary>
/// 通常敵
/// </summary>
class NormalEnemy : public Enemy, public ICollisionCallback {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, ModelManager::ModelData* model) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Player* player) override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UI描画処理
	/// </summary>
	void DrawUI() override;

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// 弾モデルのセット
	/// </summary>
	void SetBulletModel(ModelManager::ModelData* model) { modelEnemyBullet_ = model; }

	/// <summary>
	/// 現在のステートを取得
	/// </summary>
	EnemyState GetState() const { return state_; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

	/// <summary>
	/// 弾の更新処理
	/// </summary>
	void UpdateBullets();

	/// <summary>
	/// ステート管理
	/// </summary>
	void UpdateState(Player* player);

	/* 各State更新処理 */
	/// <summary>
	/// 警戒ステート更新処理
	/// </summary>
	void UpdateAlertState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer);

	/// <summary>
	/// 移動ステート更新処理
	/// </summary>
	void UpdateMoveState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer);

	/// <summary>
	/// 攻撃ステート更新処理
	/// </summary>
	void UpdateAttackState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer);

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	ModelManager::ModelData* modelEnemyBullet_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// ステート
	EnemyState state_ = EnemyState::Alert;

	// 索敵距離
	float detectionRange_ = 15.0f;
	// 攻撃距離
	float attackRange_ = 5.0f;
	// 移動速度
	float moveSpeed_ = 0.05f;

	// 攻撃速度関連
	float attackCooldown_ = 1.0f;
	float attackTimer_ = 0.0f;

	// 弾の拡散角
	float bulletSpreadAngle_ = 0.1f;
};
