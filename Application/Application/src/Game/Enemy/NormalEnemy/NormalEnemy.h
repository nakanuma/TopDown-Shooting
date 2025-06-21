#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// 全体ステート
/// </summary>
enum class EnemyState {
	Alert,   // 警戒中
	Move,   // 接近中
	Attack, // 攻撃中
};

/// <summary>
/// 警戒時サブステート
/// </summary>
enum class AlertSubState {
	Rotate, // 回転
	WaitAfterRotate, // 回転後待機
	MoveForward, // 直進
	WaitAfterMove // 直進後待機
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
	/// 弾の更新処理
	/// </summary>
	void UpdateBullets() override;

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

	// 全体ステート
	EnemyState state_ = EnemyState::Alert;

	/*待機ステート関連*/
	AlertSubState alertSubState_ = AlertSubState::Rotate;

	// 警戒ステート時タイマー
	float alertStateTimer_ = 0.0f;
	// 待機時間（ランダム）
	float waitDuration_ = 0.0f;
	const float kMinWaitTime = 2.0f;
	const float kMaxWaitTime = 4.0f;
	// 回転時間（ランダム）
	float alertRotateDuration_ = 0.0f;
	const float kMinRotateTime = 1.0f;
	const float kMaxRotateTime = 2.0f;
	// 回転速度
	float rotationSpeed_ = 0.01f;
	// 回転方向（trueなら右回り、falseなら左回り）
	float isRotatingRight_ = true;
	// 移動時間（ランダム）
	float moveForwardDuration_ = 0.0f;
	const float kMinMoveTime = 0.8f;
	const float kMaxMoveTime = 1.2f;



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
