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
class BossEnemy : public Enemy, public ICollisionCallback {
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
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Collider* other) override;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// コライダーの更新処理を行います。
	/// </summary>
	void UpdateCollider();

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
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	const Float2 kHPBarPosition = {640.0f, 25.0f};				/* HPバーの位置 */
	const Float2 kHPBarSizeBoss = {640.0f, 50.0f};				/* HPバーのサイズ */

	float moveSpeed_ = 3.0f;/* 移動速度 */

	// ----- BehaviorTree -----
	std::unique_ptr<BehaviorTree<BossEnemy>> behaviorTree_;		/* BehaviorTree */
	std::unique_ptr<BehaviorTreeEditor<BossEnemy>> btEditor_;	/* BehaviorTreeEditor */
};
