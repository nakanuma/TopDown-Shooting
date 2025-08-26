#pragma once

// Engine
#include <Engine/BehaviourTree/BehaviorTree.h>
#include <Engine/BehaviourTree/BehaviorTreeEditor.h>
#include <Engine/Util/ParameterSystem.h>

// Application
#include <src/Game/Enemy/Base/Enemy.h>
#include <src/Game/Waypoint/WaypointManager.h>

/// <summary>
/// 通常敵
/// </summary>
class NormalEnemy : public Enemy, public ICollisionCallback, public IConfigurable {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

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
	/// デバッグ表示
	/// </summary>
	void Debug();

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

	/// <summary>
	/// 経路探索で得たウェイポイント列に沿って移動（移動デバッグ用）
	/// </summary>
	void MoveAlongPath(const std::vector<Waypoint*>& path, float speed);

	/* 索敵時 */

	/// <summary>
	/// プレイヤーの視界チェック
	/// </summary>
	bool IsPlayerInSight();

	/// <summary>
	/// 索敵中の視界をデバッグ用に可視化（扇形）
	/// </summary>
	void DrawDebugSight();

	/// <summary>
	/// 一定範囲内をランダムに移動
	/// </summary>
	BehaviorStatus RandomPatrol();

	/// <summary>
	/// ランダムに回転
	/// </summary>
	BehaviorStatus RandomRotate();

	/* 攻撃時 */

	/// <summary>
	/// プレイヤーの方向を向く
	/// </summary>
	BehaviorStatus FacePlayer();

	/// <summary>
	/// 弾発射処理
	/// </summary>
	BehaviorStatus Shoot();

	/// <summary>
	/// プレイヤーへの移動（プレイヤー発見時かつ、視界が遮られている場合）
	/// </summary>
	BehaviorStatus MoveToPlayer();

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	ModelManager::ModelData* modelEnemyBullet_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------
	
	// 移動速度
	float speed_ = 10.0f;
	// プレイヤー発見フラグ
	bool isPlayerDetected_ = false;

	/* 索敵時に使用 */

	float searchRadius_ = 20.0f; // 索敵半径
	float searchFovDeg_ = 80.0f; // 索敵視野角

	Float3 spawnPosition_; // 初期スポーン地点
	float patrolRange_ = 12.0f; // スポーン地点からの最大移動範囲
	float minPatrolRange_ = 5.0f; // 現在地から最低限移動する範囲
	Waypoint* currentTargetWP_ = nullptr; // 現在の移動目標
	float patrolMoveSpeed_ = 5.0f; // 索敵時スピード

	float rotateTimer_ = 0.0f; // 回転用タイマー
	float rotateDirection_ = 0.0f; // 回転方向

	/* 攻撃時に使用 */

	float bulletSpreadAngle_ = 0.1f; // 弾の拡散角
	const uint32_t kMagazineSize = 12; // マガジン最大サイズ
	uint32_t currentAmmo_ = 0; // 現在の弾数
	uint32_t burstCount_ = 0; // 現在のバースト内で撃った弾数
	const uint32_t kBurstSize = 3; // 1回のバースト発射数
	const float kBurstInterval = 0.2f; // バースト内の発射間隔
	float burstCooldown_ = 0.0f; // バースト内のクールタイム
	const float kFireInterval = 0.6f; // バースト間の待機時間
	float fireCooldown_ = 0.0f; // バースト間のクールタイム

	const float kReloadTime = 2.0f; // リロードにかかる時間
	float reloadTimer_ = 0.0f; // リロード中タイマー
	bool isReloading_ = false; // リロードしているかフラグ

	// ---------------------------------------------------------
	// BehaviorTree
	// ---------------------------------------------------------

	std::unique_ptr<BehaviorTree<NormalEnemy>> behaviorTree_;
	std::unique_ptr<BehaviorTreeEditor<NormalEnemy>> btEditor_;

	/// <summary>
	/// ビヘイビアツリーの構築
	/// </summary>
	void BuildBehaviorTree();
};