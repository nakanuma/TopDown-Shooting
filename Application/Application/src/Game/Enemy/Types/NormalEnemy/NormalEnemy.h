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
	/// プレイヤー方向を見続けます。
	/// </summary>
	/// <returns></returns>
	Cygnus::BehaviorStatus FaceToPlayer();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f};	/* コライダーサイズ */
	static constexpr int32_t kInitialHP = 40;	/* 初期HP */

	// =========================================================
	// Member Variables
	// =========================================================
	
	std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> behaviorTree_;	/* ビヘイビアツリー */
};