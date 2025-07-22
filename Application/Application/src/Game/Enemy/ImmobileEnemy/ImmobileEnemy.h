#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// 全体ステート
/// </summary>
enum class ImmobileEnemyState {
	Alert,   // 警戒中
	Move,   // 接近中
	Attack, // 攻撃中
};

/// <summary>
/// 警戒時サブステート
/// </summary>
enum class ImmobileAlertSubState {
	Rotate, // 回転
	WaitAfterRotate, // 回転後待機
	MoveForward, // 直進
	WaitAfterMove // 直進後待機
};

/// <summary>
/// 固定敵
/// </summary>
class ImmobileEnemy : public Enemy, public ICollisionCallback{
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
	/// <param name="other"></param>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// 弾モデルのセット
	/// </summary>
	void SetBulletModel(ModelManager::ModelData* model) { modelEnemyBullet_ = model; }

	/// <summary>
	/// 現在のステートを取得
	/// </summary>
	ImmobileEnemyState GetState() const { return state_; }

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
	ImmobileEnemyState state_ = ImmobileEnemyState::Alert;

	// 移動速度
	float moveSpeed_ = 0.0f;


	/*待機ステート関連*/
	ImmobileAlertSubState alertSubState_ = ImmobileAlertSubState::WaitAfterMove;

	// 警戒ステート時タイマー
	float alertStateTimer_ = 0.0f;
	// 待機時間（ランダム）
	float waitDuration_ = 0.0f;
	const float kMinWaitTime = 1.5f;
	const float kMaxWaitTime = 2.0f;
	// 回転時間（ランダム）
	float alertRotateDuration_ = 0.0f;
	const float kMinRotateTime = 0.8f;
	const float kMaxRotateTime = 2.0f;
	// 回転速度
	float rotationSpeed_ = 0.01f;
	// 回転方向（trueなら右回り、falseなら左回り）
	float isRotatingRight_ = true;
	// 回転後に直進する確率
	const float kMoveForwardProbability = 0.25f; // 25%
	// 直進時間（ランダム）
	float moveForwardDuration_ = 0.0f;
	const float kMinMoveTime = 0.8f;
	const float kMaxMoveTime = 1.2f;
	// 索敵距離
	float detectionRange_ = 15.0f;


	/* 移動ステート関連 */

	// 移動ステート時タイマー
	float moveStateTimer_ = 0.0f;
	// 視線が障害物に遮られている時間を計測
	float obstacleLostTimer_ = 0.0f;
	// 警戒ステートへ戻るまでの時間上限
	const float kLostTime = 2.0f;
	// 適正戦闘範囲
	const float kOptimalCombatRange = 10.0;


	/* 攻撃ステート関連 */

	// 攻撃ステート時タイマー
	float attackStateTimer_ = 0.0f;
	// 残弾数
	/*int32_t bulletRemaining_ = 5;*/
	// 弾の上限
	const int32_t kMaxBullet = 5;
	//// リロード時フラグ
	//bool isReloading_ = false;
	//// リロード時タイマー
	//float reloadTimer_ = 0.0f;
	// リロードにかかる時間
	const float kReloadTime = 1.5f;
	// 次の発射までの待ち時間
	float nextShotInterval_ = 0.0f;
	const float kMinShotInterval = 0.3f;
	const float kMaxShotInterval = 0.4f;

	// 弾の拡散角
	float bulletSpreadAngle_ = 0.1f;
	// その回で撃つ予定の弾数（ランダム）
	int32_t bulletsToShot_ = 0;
	const int32_t kMinShotThisTime = 2;
	const int32_t kMaxShotThisTime = 3;
	// その回で撃った弾数
	int32_t bulletsShotInThisAttack_ = 0;


	// デバッグで一時的にpublicへ
public:
	// リロード時フラグ
	bool isReloading_ = false;
	// リロード時タイマー
	float reloadTimer_ = 0.0f;
	// 残弾数
	int32_t bulletRemaining_ = 5;
};
