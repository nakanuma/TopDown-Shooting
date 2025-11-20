#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Model/ModelManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/Base/Enemy.h>
#include <src/Game/Enemy/BossEnemy/BossEnemy.h>
#include <src/Game/Loader/Loader.h>

// =========================================================
// 敵の管理を行うクラス
// =========================================================
class EnemyManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 敵管理クラスの初期化処理を行います。
	/// </summary>
	/// <param name="datas">初期位置や回転などのTransformデータ</param>
	/// <param name="player">プレイヤーのポインタ</param>
	void Initialize(const std::vector<Loader::TransformData>& datas, Player* player);

	/// <summary>
	/// 全ての敵の毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 全ての敵のモデル描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// 全ての敵のシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// 全ての敵のUI描画処理を行います。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 全ての敵のデバッグ用描画処理を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// ボスを取得します。
	/// </summary>
	/// <returns>ボスのポインタ</returns>
	BossEnemy* GetBoss() const;

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::vector<std::unique_ptr<Enemy>> enemies_;		/* 全ての敵を格納したコンテナ */
	Player* player_ = nullptr;							/* プレイヤーのポインタ */
};
