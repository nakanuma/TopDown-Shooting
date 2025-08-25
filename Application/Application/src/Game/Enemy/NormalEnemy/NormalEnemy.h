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

	/* 索敵時に使用 */

	float searchRadius_ = 10.0f; // 索敵半径
	float searchFovDeg_ = 60.0f; // 索敵視野角

	Float3 spawnPosition_; // 初期スポーン地点
	float patrolRange_ = 12.0f; // スポーン地点からの最大移動範囲
	float minPatrolRange_ = 5.0f; // 現在地から最低限移動する範囲
	Waypoint* currentTargetWP_ = nullptr; // 現在の移動目標
	float patrolMoveSpeed_ = 5.0f; // 索敵時スピード

	float rotateTimer_ = 0.0f; // 回転用タイマー
	float rotateDirection_ = 0.0f; // 回転方向
	float targetAngle_ = 0.0f; // 目標角度

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