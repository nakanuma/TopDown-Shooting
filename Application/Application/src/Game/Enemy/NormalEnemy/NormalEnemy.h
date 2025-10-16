#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorTree.h>
#include <Engine/BehaviourTree/BehaviorTreeEditor.h>
#include <Engine/Util/ParameterSystem.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/Base/Enemy.h>
#include <src/Game/Waypoint/WaypointManager.h>

// =========================================================
// 通常の敵クラス
// =========================================================
class NormalEnemy : public Enemy, public ICollisionCallback, public IConfigurable {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 敵の初期化処理を行います。
	/// </summary>
	/// <param name="position">初期位置</param>
	/// <param name="model">モデルデータ</param>
	/// <param name="player">プレイヤーのポインタ</param>
	void Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) override;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow() override;

	/// <summary>
	/// UIの描画処理を行います。
	/// </summary>
	void DrawUI() override;

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 弾モデルをセットします。
	/// </summary>
	/// <param name="model">モデルデータ</param>
	void SetBulletModel(ModelManager::ModelData* model) { modelEnemyBullet_ = model; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// コライダーの更新処理を行います。
	/// </summary>
	void UpdateCollider();

	/// <summary>
	/// 経路探索で得たウェイポイント列に沿って移動します。
	/// </summary>
	/// <param name="path">移動経路</param>
	/// <param name="speed">移動速度</param>
	void MoveAlongPath(const std::vector<Waypoint*>& path, float speed);

	/// <summary>
	/// プレイヤーとの距離・遮蔽チェックを行います。
	/// </summary>
	/// <returns>プレイヤー発見フラグ</returns>
	bool IsPlayerInSight();

	/// <summary>
	/// 敵の扇形の視界を可視化します。（デバッグ用）
	/// </summary>
	void DrawDebugSight();

private:
	/// <summary>
	/// 敵が一定範囲内をランダムに移動します。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	BehaviorStatus RandomPatrol();

	/// <summary>
	/// ランダムに敵の回転を行います。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	BehaviorStatus RandomRotate();

	/// <summary>
	/// プレイヤー方向へ敵を回線させます。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	BehaviorStatus FacePlayer();

	/// <summary>
	/// 弾の発射処理を行います。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	BehaviorStatus Shoot();

	/// <summary>
	/// 経路探索をしてプレイヤーまで移動します。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	BehaviorStatus MoveToPlayer();

	/// <summary>
	/// BehaviorTreeの構築を行います。
	/// </summary>
	void BuildBehaviorTree();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Models -----
	ModelManager::ModelData* modelEnemyBullet_;					/* 弾モデル */

	// ----- Parameters -----
	float speed_ = 10.0f;										/* 移動速度 */ 
	bool isPlayerDetected_ = false;								/* プレイヤー発見フラグ */

	// ----- Search -----
	float searchRadius_ = 20.0f;								/* 索敵半径 */
	float searchFovDeg_ = 80.0f;								/* 索敵視野角 */

	Float3 spawnPosition_ = {0.0f, 0.0f, 0.0f};					/* 初期スポーン地点 */
	float patrolRange_ = 12.0f;									/* スポーン地点からの最大移動範囲 */
	float minPatrolRange_ = 5.0f;								/* 現在地から最低限移動する範囲 */
	Waypoint* currentTargetWP_ = nullptr;						/* 現在の移動目標 */
	float patrolMoveSpeed_ = 5.0f;								/* 索敵時移動速度 */

	float rotateTimer_ = 0.0f;									/* 回転時タイマー */
	float rotateDirection_ = 0.0f;								/* 回転方向 */

	// ----- Attack -----
	float bulletSpreadAngle_ = 0.1f;							/* 弾の拡散角 */
	const uint32_t kMagazineSize = 12;							/* マガジン最大サイズ */
	uint32_t currentAmmo_ = 0;									/* 現在の弾数 */
	uint32_t burstCount_ = 0;									/* 現在のバースト内で撃った弾数 */
	const uint32_t kBurstSize = 3;								/* 1回のバースト発射数 */
	const float kBurstInterval = 0.2f;							/* バースト内の発射間隔 */
	float burstCooldown_ = 0.0f;								/* バースト内のクールタイム */
	const float kFireInterval = 0.6f;							/* バースト間の待機時間 */
	float fireCooldown_ = 0.0f;									/* バースト間のクールタイム */

	const float kReloadTime = 2.0f;								/* リロード所要時間 */
	float reloadTimer_ = 0.0f;									/* リロード中タイマー */
	bool isReloading_ = false;									/* リロード中フラグ */

	// ----- BehaviorTree -----
	std::unique_ptr<BehaviorTree<NormalEnemy>> behaviorTree_;	/* behaviorTree */
	std::unique_ptr<BehaviorTreeEditor<NormalEnemy>> btEditor_;	/* BehaviorTreeEditor */
};