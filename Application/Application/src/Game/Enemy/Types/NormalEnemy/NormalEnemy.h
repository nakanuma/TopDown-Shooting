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
class NormalEnemy : public Enemy, public Cygnus::ICollisionCallback {
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

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f};	/* コライダーサイズ */
	static constexpr int32_t kInitialHP = 40;	/* 初期HP */

	static constexpr int32_t kDeathCrossCount = 3;     /* 死亡時クロスパーティクルの発生数 */
	static constexpr float kDeathCrossAngle1 = 45.0f;  /* 死亡時クロスパーティクル1の角度（度） */
	static constexpr float kDeathCrossAngle2 = 135.0f; /* 死亡時クロスパーティクル2の角度（度） */

	// =========================================================
	// Member Variables
	// =========================================================
	Cygnus::Float3 spawnPosition_ = {0.0f, 0.0f, 0.0f}; /* 初期スポーン地点 */
};