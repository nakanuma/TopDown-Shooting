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
class ImmobileEnemy : public Enemy, public ICollisionCallback {
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
	/// プレイヤーとの距離・遮蔽チェックを行います。
	/// </summary>
	/// <returns>プレイヤー発見フラグ</returns>
	bool IsPlayerInSight();

	/// <summary>
	/// 索敵モーションを行います。
	/// : 回転->待機->回転->待機・・・を繰り返す
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
	// Member Variables
	// =========================================================

	// ----- Models -----
	ModelManager::ModelData* modelEnemyBullet_;					/* 弾モデル */

	// ----- Search -----
	float searchRange_ = 20.0f;									/* プレイヤーを発見できる距離 */
	bool isPlayerVisible_ = false;								/* プレイヤー発見フラグ */

	enum class SearchState {									/* 索敵時の状態を表す列挙体 */
		Rotate,
		Wait,
	};
	SearchState searchState_ = SearchState::Wait;				/* 索敵時の状態 */

	float searchStateTimer_ = 0.0f;								/* 索敵時タイマー */
	
	float waitDuration_ = 0.0f;									/* 待機時の経過時間 */
	const float kMinWaitTime = 1.5f;							/* 最小経過時間 */
	const float kMaxWaitTime = 2.0f;							/* 最大経過時間 */

	float searchRotateDuration_ = 0.0f;							/* 回転時の経過時間 */
	const float kMinRotateTime = 0.8f;							/* 最小回転時間 */
	const float kMaxRotateTime = 2.0f;							/* 最大回転時間 */
	
	float rotationSpeed_ = 0.01f;								/* 回転速度 */
	float isRotatingRight_ = true;								/* 回転方向 : true（右回り）false（左回り） */

	// ----- Attack -----
	float bulletSpreadAngle_ = 0.1f;							/* 弾の拡散角 */

	float nextShotInterval_ = 0.0f;								/* 攻撃間隔タイマー */
	const float kShotInterval = 0.3f;							/* 攻撃間隔 */

	int32_t bulletRemaining_ = 5;								/* 残弾数 */
	const int32_t kMaxBullet = 5;								/* 一度に撃てる弾の上限数 */

	bool isReloading_ = false;									/* リロード中フラグ */
	const float kReloadTime = 1.5f;								/* リロード所要時間 */
	float reloadTimer_ = 0.0f;									/* リロード中タイマー */

	// ----- BehaviorTree -----
	std::unique_ptr<BehaviorTree<ImmobileEnemy>> behaviorTree_;	/* behaviorTree */
};