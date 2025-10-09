#pragma once

// Engine
#include <Engine/Model/ModelManager.h>

// Application
#include <src/Game/Enemy/Base/Enemy.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Enemy/BossEnemy/BossEnemy.h>

class Player;

/// <summary>
/// 敵の管理
/// </summary>
class EnemyManager {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const std::vector<Loader::TransformData> datas, Player* player);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// シャドウマップ描画処理
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// UI描画処理
	/// </summary>
	void DrawUI();

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 再生成処理
	/// </summary>
	void Reload(const std::vector<Loader::TransformData> datas);

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	/* 敵本体 */

	// 通常敵モデル
	ModelManager::ModelData modelNormalEnemy_;
	// 固定敵モデル
	ModelManager::ModelData modelImmobileEnemy_;

	// ボスモデル
	ModelManager::ModelData modelBossEnemy_;

	/* 弾 */

	// 弾モデル
	ModelManager::ModelData modelEnemyBullet_;
	// ミサイルモデル
	ModelManager::ModelData modelMissile_;
	// 地面警告モデル
	ModelManager::ModelData modelGroundWarning_;

	// ---------------------------------------------------------
	// 敵
	// ---------------------------------------------------------

	// 敵のコンテナ
	std::vector<std::unique_ptr<Enemy>> enemies_;

	// ---------------------------------------------------------
	// その他
	// ---------------------------------------------------------

	// プレイヤーのポインタ
	Player* player_ = nullptr;
};
