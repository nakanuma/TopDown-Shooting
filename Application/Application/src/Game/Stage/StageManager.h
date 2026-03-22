#pragma once

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class EnemyManager;
class PowerGeneratorManager;
class TeleporterManager;

// ---------------------------------------------------------
// C++ includes
// ---------------------------------------------------------
#include <cstdint>

/// <summary>
/// ステージの目標タイプ
/// </summary>
enum class StageType {
	killAllEnemies,			/* 全ての敵を撃破 */
	DestroyAllGeneratos,	/* 全ての発電機を破壊 */
	BossBattle,				/* ボスを撃破 */
};

// =========================================================
// ステージ毎の開始時や目標達成時の処理を管理するクラス
// =========================================================
class StageManager
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="enemyMng"></param>
	/// <param name="powerGeneratorMng"></param>
	/// <param name="teleporterMng"></param>
	void Initialize(EnemyManager* enemyMng, PowerGeneratorManager* powerGeneratorMng, TeleporterManager* teleporterMng);

	/// <summary>
	/// ステージ開始時の設定を行います。（ステージ移行時に呼ぶ）
	/// </summary>
	/// <param name="floor"></param>
	void PrepareNextState(int32_t floor);

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// デバッグ表示を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// 現在のステージ階層を取得します。
	/// </summary>
	/// <returns></returns>
	int32_t GetCurrentFloor() const { return currentFloor_; }
 
private:
	// =========================================================
	// Constants
	// =========================================================


	// =========================================================
	// Member Variables
	// =========================================================
	EnemyManager* enemyManager_ = nullptr;
	PowerGeneratorManager* powerGeneratorManager_ = nullptr;
	TeleporterManager* teleporterManager_ = nullptr;

	int32_t currentFloor_ = 1;	/* 現在の階層 */
	StageType currentStageType_ = StageType::killAllEnemies;	/* 現在の階層の目標 */

	bool isObjectiveCleared_ = false;	/* 目標達成フラグ */
};

