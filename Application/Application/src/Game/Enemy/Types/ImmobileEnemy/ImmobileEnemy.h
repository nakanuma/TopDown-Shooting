#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorTree.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/Base/Enemy.h>

// =========================================================
// 固定砲台の敵クラス
// =========================================================
class ImmobileEnemy : public Enemy {
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

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// プレイヤーとの距離・遮蔽チェックを行います。
	/// </summary>
	/// <returns>プレイヤー発見フラグ</returns>
	bool IsPlayerInSight();

	/// <summary>
	/// 索敵モーションを行います。
	/// : 回転->待機->回転->待機・・・のループを行う
	/// </summary>
	void SearchMotion();

	/// <summary>
	/// プレイヤー方向へ敵を回転させます。
	/// </summary>
	void FaceToPlayer();

	/// <summary>
	/// 弾の発射処理を行います。
	/// </summary>
	void Shoot();

	/// <summary>
	/// BehaviorTreeの構築を行います。
	/// </summary>
	void BuildBehaviorTree();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f};       /* コライダーサイズ */

	static constexpr int32_t kInitialHP = 40;    /* 初期HP */
	static constexpr float kSearchRange = 20.0f; /* プレイヤー発見距離 */

	static constexpr float kMinWaitTime = 1.5f;    /* 索敵中に次の回転を行うまでの最小待機時間（秒） */
	static constexpr float kMaxWaitTime = 2.0f;    /* 索敵中に次の回転を行うまでの最大待機時間（秒） */
	static constexpr float kMinRotateTime = 0.8f;  /* 索敵時の最小回転時間（秒） */
	static constexpr float kMaxRotateTime = 2.0f;  /* 索敵時の最大回転時間（秒） */
	static constexpr float kRotationSpeed = 0.01f; /* 索敵時の回転速度 */

	static constexpr float kBulletSpreadAngle = 0.1f; /* 弾の拡散角 */
	static constexpr float kShotInterval = 0.3f;      /* 射撃間隔 */
	static constexpr int32_t kMaxBullet = 5;          /* リロードするまでに撃てる弾数（1マガジン） */
	static constexpr float kReloadTime = 1.5f;        /* リロード所要時間 */

	static constexpr int32_t kDeathCrossCount = 3;     /* 死亡時クロスパーティクルの発生数 */
	static constexpr float kDeathCrossAngle1 = 45.0f;  /* 死亡時クロスパーティクル1の角度（度） */
	static constexpr float kDeathCrossAngle2 = 135.0f; /* 死亡時クロスパーティクル2の角度（度） */

	// =========================================================
	// Member Variables
	// =========================================================

	enum class SearchState {			/* 索敵時の状態を表す列挙体 */
		Rotate,							/* 回転中 */
		Wait,							/* 待機中 */
	};
	SearchState searchState_ = SearchState::Wait; /* 現在の索敵状態 */

	bool isPlayerVisible_ = false;      /* プレイヤー発見フラグ */
	float searchStateTimer_ = 0.0f;     /* 索敵時タイマー */
	float waitDuration_ = 0.0f;         /* 索敵中待機時の経過時間 */
	float searchRotateDuration_ = 0.0f; /* 回転時の経過時間 */
	float isRotatingRight_ = true;      /* 回転方向 : true（右回り）false（左回り） */

	float nextShotInterval_ = 0.0f; /* 攻撃間隔タイマー */
	int32_t bulletRemaining_ = 5;   /* 残弾数 */
	bool isReloading_ = false;      /* リロード中フラグ */
	float reloadTimer_ = 0.0f;      /* リロード中タイマー */

	std::unique_ptr<Cygnus::BehaviorTree<ImmobileEnemy>> behaviorTree_; /* behaviorTree */
};