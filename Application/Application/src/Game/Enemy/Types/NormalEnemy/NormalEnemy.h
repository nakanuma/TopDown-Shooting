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
// 通常の敵クラス
// =========================================================
class NormalEnemy : public Enemy {
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

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// ビヘイビアツリーの構築を行います。
	/// </summary>
	void BuildBehaviorTree();

	/// <summary>
	/// プレイヤー検出判定を行います。
	/// </summary>
	bool CheckDetect();

	/// <summary>
	/// プレイヤー発見時の処理を行います
	/// </summary>
	void OnDetected() override;

	/// <summary>
	/// プレイヤー方向を見続けます。
	/// </summary>
	/// <returns></returns>
	Cygnus::BehaviorStatus FaceToPlayer();

	/// <summary>
	/// 射撃処理を行います。
	/// </summary>
	/// <returns></returns>
	Cygnus::BehaviorStatus ActionShoot();

	/// <summary>
	/// リロード処理を行います。
	/// </summary>
	/// <returns></returns>
	Cygnus::BehaviorStatus ActionReload();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f};	/* コライダーサイズ */
	static constexpr int32_t kInitialHP = 40;	/* 初期HP */

	static constexpr float kShootDetectionRadius = 17.5f;	/* プレイヤーの射撃音で気づく距離 */
	static constexpr float kProximityRadius = 10.0f;		/* 接近（円形）で気づく距離 */
	static constexpr float kVisionRange = 20.0f;			/* 視界（扇形）の長さ */
	static constexpr float kSearchFovDeg = 75.0f;			/* 索敵視野角（度） */
	static constexpr float kFirstShootDelay = 1.5f;         /* プレイヤー発見時に射撃を始めるまでの遅延時間 */

	static constexpr int32_t kMaxMagazine = 8;			/* マガジン内最大弾数 */
	static constexpr float kBulletSpreadAngle = 0.1f;	/* 弾の拡散角 */
	static constexpr float kShootMinInterval = 1.0f;	/* 発射最小間隔 */
	static constexpr float kShootMaxInterval = 2.0f;    /* 発射最大間隔 */
	static constexpr float kReloadTime = 2.0f;			/* リロード所要時間 */

	static constexpr float kRotationSpeed = 4.0f;	/* 回転速度 */

	// =========================================================
	// Member Variables
	// =========================================================
	
	int32_t magazine_ = 8;		/* 現在のマガジン内弾数 */
	float shootTimer_ = 0.0f;	/* 射撃間隔タイマー */
	float reloadTimer_ = 0.0f;	/* リロードタイマー */
	bool isReloading_ = false;	/* リロード中フラグ */

	std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> behaviorTree_;	/* ビヘイビアツリー */
};