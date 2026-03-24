#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorTree.h>
#include <Engine/Model/Animation/AnimatedModelInstance.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/Base/Enemy.h>
#include <src/Game/Waypoint/Waypoint.h>

// =========================================================
// 通常の敵クラス
// =========================================================
class NormalEnemy : public Enemy {
	// NormalEnemyBehaviorクラスでprivateメンバ変数へのアクセスを許可
	friend class NormalEnemyBehavior;

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
	void Initialize(const Cygnus::Float3& position, Player* player) override;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 通常モデルのシャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow() override;

	/// <summary>
	/// スキニングモデルのシャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadowSkinning() override;

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

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// プレイヤー発見時の処理を行います
	/// </summary>
	void OnDetected() override;

	/// <summary>
	/// 各種範囲のデバッグ表示を行います。
	/// </summary>
	void DebugDrawLine();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f}; /* コライダーサイズ */
	static constexpr int32_t kInitialHP = 40;                           /* 初期HP */

	static constexpr float kShootDetectionRadius = 17.5f; /* プレイヤーの射撃音で気づく距離 */
	static constexpr float kProximityRadius = 10.0f;      /* 接近（円形）で気づく距離 */
	static constexpr float kSearchFovDeg = 75.0f;         /* 索敵視野角（度） */
	static constexpr float kVisionRange = 20.0f;          /* 視界（扇形）の長さ */

	static constexpr float kReloadTime = 2.0f;        /* リロード所要時間 */
	static constexpr float kFirstShootDelay = 0.5f;   /* プレイヤー発見時に射撃を始めるまでの遅延時間 */
	static constexpr int32_t kMaxMagazine = 6;        /* マガジン内最大弾数 */
	static constexpr float kBulletSpreadAngle = 0.1f; /* 弾の拡散角 */
	static constexpr float kShootMinInterval = 0.5f; /* 発射最小間隔 */
	static constexpr float kShootMaxInterval = 1.0f;  /* 発射最大間隔 */

	static constexpr float kKeepDistance = 10.0f;    /* プレイヤーとの適正距離（移動場所判定用） */
	static constexpr float kRotationSpeed = 4.0f;    /* 回転速度（プレイヤー発見時） */
	static constexpr float kMoveMinSpeed = 3.0f;     /* 移動最小速度（射撃時） */
	static constexpr float kMoveMaxSpeed = 6.0f;     /* 移動最大速度（射撃時） */
	static constexpr float kMoveProbability = 0.7f;  /* 移動する確率（射撃時） */
	static constexpr float kMoveMinDuration = 0.25f; /* 移動最小時間（射撃時） */
	static constexpr float kMoveMaxDuration = 2.0f;  /* 移動最大時間（射撃時） */
	
	static constexpr float kSearchWaitMinTime = 1.5f;	/* 次の索敵行動までの最小待機時間 */
	static constexpr float kSearchWaitMaxTime = 3.0f;	/* 次の索敵行動までの最大待機時間 */
	static constexpr float kSearchRotationAngle = 1.0f;	/* 索敵時の首振り角度（ラジアン） */

	static constexpr float kAttackRange = 20.0f;	/* この距離よりも近ければプレイヤーに攻撃する */
	static constexpr float kApproachSpeed = 8.0f;	/* 攻撃範囲外の際、経路探索で移動する際の速度 */

	static constexpr float kAnimationPlaybackSpeed = 0.5f; /* アニメーション再生速度 */

	static constexpr Cygnus::Float4 kGunColor = {0.25f, 0.25f, 0.25f, 1.0f}; /* 銃の色 */
	static constexpr float kGunForwardOffset = 1.8f;  /* 銃の前方位置オフセット */
	static constexpr float kGunUpOffset = 0.9f;       /* 銃の上方向位置オフセット */

	// =========================================================
	// Member Variables
	// =========================================================

	int32_t magazine_ = 6;     /* 現在のマガジン内弾数 */
	float shootTimer_ = 0.0f;  /* 射撃間隔タイマー */
	float reloadTimer_ = 0.0f; /* リロードタイマー */
	bool isReloading_ = false; /* リロード中フラグ */

	float moveSpeed_ = 0.0f;             /* 移動速度 */
	float moveTimer_ = 0.0f;             /* 移動残り時間 */
	Cygnus::Float3 moveDir_ = {0, 0, 0}; /* 移動方向 */

	float searchTimer_ = 0.0f;			/* 索敵行動用タイマー */
	float searchTargetAngle_ = 0.0f;	/* 索敵時の目標角度 */

	bool isWalking_ = false; /* 歩いているかどうか */

	Waypoint* combatTargetWP_ = nullptr; /* 戦闘時移動用のウェイポイント */

	std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> behaviorTree_; /* ビヘイビアツリー */

	std::unique_ptr<Cygnus::AnimatedModelInstance> objectEnemyAnim_; /* オブジェクト（アニメーション適用） */
	Cygnus::AnimatedModelInstance::AnimatedModelData walkData_;      /* 歩行アニメーションデータ */
};