#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorTree.h>
#include <Engine/BehaviourTree/BehaviorTreeEditor.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/Base/Enemy.h>

// =========================================================
// ボスエネミークラス
// =========================================================
class BossEnemy : public Enemy, public Cygnus::ICollisionCallback {
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
	void Initialize(const Cygnus::Float3& position, Cygnus::ModelManager::ModelData* model, Player* player) override;

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
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Cygnus::Collider* other) override;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 死亡中フラグを取得します。
	/// </summary>
	/// <returns>死亡中フラグ</returns>
	bool IsDying() const { return isDying_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// プレイヤー方向へボスを回転させます。
	/// </summary>
	void FacePlayer();

	/// <summary>
	/// プレイヤーに向かって移動を行います。
	/// </summary>
	void MoveTowardPlayer();

	/// <summary>
	/// 追尾ミサイルの発射を行います。
	/// </summary>
	void FireHomingMissile();

	/// <summary>
	/// 地面警告攻撃を行います。
	/// </summary>
	void GroundWarningAttack();

	/// <summary>
	/// BehaviorTreeの構築を行います。
	/// </summary>
	void BuildBehaviorTree();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float4 kBossColor = {0.2f, 0.2f, 0.2f, 1.0f}; /* ボスの色 */
	static constexpr Cygnus::Float3 kColliderSize = {5.2f, 3.0f, 5.2f};    /* コライダーサイズ */

	static constexpr Cygnus::Float2 kHPBarPosition = {640.0f, 25.0f};                 /* HPバーの位置 */
	static constexpr Cygnus::Float2 kHPBarSizeBoss = {640.0f, 50.0f};                 /* HPバーのサイズ */
	static constexpr Cygnus::Float4 kHPBarBackgroundColor = {0.0f, 0.0f, 0.0f, 1.0f}; /* HPバー背景色 */
	static constexpr Cygnus::Float4 kHPBarForegroundColor = {1.0f, 0.0f, 0.0f, 1.0f}; /* HPバー前景色 */

	static constexpr int32_t kInitialHP = 300;        /* 初期HP */
	static constexpr float kMoveSpeed = 3.0f;         /* 移動速度 */
	static constexpr float kDyingDuration = 7.0f;     /* 死亡演出時間 */
	static constexpr float kWaitAttackTimeMin = 2.0f; /* 攻撃間隔の最小値 */
	static constexpr float kWaitAttackTimeMax = 4.0f; /* 攻撃間隔の最大値 */

	static constexpr size_t kDyingEmitCount = 20;									/* 死亡時パーティクルの発生回数 */
	static constexpr Cygnus::Float3 kExplodeSmokeOffsetMin = {-6.0f, -5.0f, -6.0f}; /* 死亡時煙パーティクルの最小オフセット */
	static constexpr Cygnus::Float3 kExplodeSmokeOffsetMax = {6.0f, 5.0f, 6.0f};    /* 死亡時煙パーティクルの最大オフセット */
	static constexpr int32_t kExplodeSmokeCount = 30;								/* 爆発煙パーティクルの発生数 */
	static constexpr int32_t kBossFragmentsCount = 150;								/* 破片パーティクルの発生数 */
	static constexpr Cygnus::Float3 kRedCircleOffset = {0.0f, 0.1f, 0.0f};          /* 赤い円エフェクトのオフセット */
	static constexpr int32_t kRedCircleCount = 1;									/* 赤い円エフェクトの発生数 */

	// =========================================================
	// Member Variables
	// =========================================================
	bool isDying_ = false;    /* 死亡演出中フラグ */
	float dyingTimer_ = 0.0f; /* 死亡演出中経過時間 */

	std::unique_ptr<Cygnus::BehaviorTree<BossEnemy>> behaviorTree_;   /* BehaviorTree */
	std::unique_ptr<Cygnus::BehaviorTreeEditor<BossEnemy>> btEditor_; /* BehaviorTreeEditor */
};
