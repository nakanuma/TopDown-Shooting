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
class NormalEnemy : public Enemy, public Cygnus::ICollisionCallback, public Cygnus::Configurator {
public:
	/// <summary>
	/// 思考や状態に関する変数をまとめたBlackBoard
	/// </summary>
	struct NormalEnemyBlackBoard {
		// 索敵
		bool isPlayerDetected = false;

		// 移動・タイマー
		float rotateTimer = 0.0f;
		float rotateDirection = 0.0f;
		Waypoint* currentTargetWP = nullptr;

		// 戦闘
		uint32_t currentAmmo = 0;
		uint32_t burstCount = 0;
		float burstCooldown = 0.0f;
		float fireCooldown = 0.0f;
		float reloadTimer = 0.0f;
		bool isReloading = false;
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 敵の初期化処理を行います。
	/// </summary>
	/// <param name="position">初期位置</param>
	/// <param name="model">モデルデータ</param>
	/// <param name="player">プレイヤーのポインタ</param>
	void Initialize(const Cygnus::Float3& position, Player* player) override;

	void Initialize(
		const Cygnus::Float3& position, 
		Cygnus::ModelManager::ModelData* model, 
		Player* player,
		Cygnus::BehaviorTree<NormalEnemy>* masterTree 
	);

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
	void OnCollision(Cygnus::Collider* other) override;

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

public:
	/// <summary>
	/// ブラックボードを取得します。
	/// </summary>
	/// <returns></returns>
	NormalEnemyBlackBoard& GetBlackBoard() { return bb_; }

	// =========================================================
	// BehaviorTreeのノードから呼ばれる関数群
	// =========================================================

	/// <summary>
	/// プレイヤーとの距離・遮蔽チェックを行います。
	/// </summary>
	/// <returns>プレイヤー発見フラグ</returns>
	bool IsPlayerInSight();

	/// <summary>
	/// プレイヤーを発見したかを取得します。
	/// </summary>
	/// <returns></returns>
	bool IsDetected() const { return bb_.isPlayerDetected; }

	/// <summary>
	/// 敵が一定範囲内をランダムに移動します。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	Cygnus::BehaviorStatus RandomPatrol();

	/// <summary>
	/// ランダムに敵の回転を行います。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	Cygnus::BehaviorStatus RandomRotate();

	/// <summary>
	/// プレイヤー方向へ敵を回線させます。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	Cygnus::BehaviorStatus FacePlayer();

	/// <summary>
	/// 弾の発射処理を行います。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	Cygnus::BehaviorStatus Shoot();

	/// <summary>
	/// 経路探索をしてプレイヤーまで移動します。
	/// </summary>
	/// <returns>BehaviorStatus</returns>
	Cygnus::BehaviorStatus MoveToPlayer();

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 経路探索で得たウェイポイント列に沿って移動します。
	/// </summary>
	/// <param name="path">移動経路</param>
	/// <param name="speed">移動速度</param>
	void MoveAlongPath(const std::vector<Waypoint*>& path, float speed);

private:
	/// <summary>
	/// 敵の扇形の視界を可視化します。（デバッグ用）
	/// </summary>
	void DrawDebugSight();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f};               /* コライダーサイズ */

	static constexpr int32_t kInitialHP = 40;  /* 初期HP */
	static constexpr float kMoveSpeed = 10.0f; /* 移動速度 */

	static constexpr float kSearchRadius = 20.0f;         /* 索敵半径 */
	static constexpr float kSearchFovDeg = 80.0f;         /* 索敵視野角（度） */
	static constexpr float kPatrolRange = 12.0f;          /* スポーン地点からの最大移動範囲 */
	static constexpr float kMinPatrolRange = 5.0f;        /* 現在地から最低限移動する範囲 */
	static constexpr float kPatrolMoveSpeed = 5.0f;       /* 索敵時移動速度 */
	static constexpr float kTurnSpeed = 5.0f;             /* 回転補間速度 */
	static constexpr float kWaypointReachDistance = 3.0f; /* ウェイポイント到達判定距離 */
	static constexpr float kPathInterpolation = 0.5f;     /* 経路補間係数 */

	static constexpr float kBulletSpreadAngle = 0.1f; /* 弾の拡散角 */
	static constexpr int32_t kMagazineSize = 12;      /* マガジン内の最大弾数 */
	static constexpr int32_t kBurstSize = 3;          /* 1回のバーストでの射撃数 */
	static constexpr float kBurstInterval = 0.2f;     /* バースト内での発射間隔 */
	static constexpr float kFireInterval = 0.6f;      /* 次のバースト射撃までの待機時間 */
	static constexpr float kReloadTime = 2.0f;        /* リロード所要時間 */

	static constexpr float kWaitBeforePatrol = 1.0f; /* 索敵時の移動前待機時間（秒） */
	static constexpr float kWaitBeforeRotate = 1.0f; /* 索敵時の回転前待機時間（秒） */
	static constexpr float kRotateTimeMin = 1.0f;    /* 索敵時の回転最小時間（秒） */
	static constexpr float kRotateTimeMax = 2.0f;    /* 索敵時の回転最大時間（秒） */

	static constexpr int32_t kDebugSightSegments = 16;									/* デバッグ視界の分割数 */
	static constexpr Cygnus::Float4 kDebugSightColorNormal = {1.0f, 1.0f, 1.0f, 1.0f};	/* デバッグ視界色（通常時） */
	static constexpr Cygnus::Float4 kDebugSightColorDetect = {1.0f, 0.0f, 0.0f, 1.0f};	/* デバッグ視界色（発見時） */

	static constexpr int32_t kDeathCrossCount = 3;     /* 死亡時クロスパーティクルの発生数 */
	static constexpr float kDeathCrossAngle1 = 45.0f;  /* 死亡時クロスパーティクル1の角度（度） */
	static constexpr float kDeathCrossAngle2 = 135.0f; /* 死亡時クロスパーティクル2の角度（度） */

	// =========================================================
	// Member Variables
	// =========================================================
	Cygnus::Float3 spawnPosition_ = {0.0f, 0.0f, 0.0f}; /* 初期スポーン地点 */
	//Waypoint* currentTargetWP_ = nullptr;				/* 現在の移動目標 */

	//bool isPlayerDetected_ = false; /* プレイヤー発見フラグ */
	//float rotateTimer_ = 0.0f;      /* 回転時タイマー */
	//float rotateDirection_ = 0.0f;  /* 回転方向 */

	//uint32_t currentAmmo_ = 0;   /* 現在の弾数 */
	//uint32_t burstCount_ = 0;    /* 現在のバースト内で撃った弾数 */
	//float burstCooldown_ = 0.0f; /* バースト内のクールタイム */
	//float fireCooldown_ = 0.0f;  /* バースト間のクールタイム */

	//float reloadTimer_ = 0.0f; /* リロード中タイマー */
	//bool isReloading_ = false; /* リロード中フラグ */

	NormalEnemyBlackBoard bb_; /* 個別の変数群をBlackBoardに統合*/

	Cygnus::BehaviorTree<NormalEnemy>* behaviorTree_ = nullptr;   /* behaviorTree */
};