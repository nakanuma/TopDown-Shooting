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
	void Initialize(const std::vector<Loader::TransformData> datas);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// UI描画処理
	/// </summary>
	void DrawUI();

	/// <summary>
	/// プレイヤーのポインタをセット
	/// </summary>
	void SetPlayer(Player* player) { player_ = player; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	// 通常敵モデル
	ModelManager::ModelData modelNormalEnemy_;
	// ボスモデル
	ModelManager::ModelData modelBossEnemy_;

	// （追加）敵モデル

	// 弾モデル
	ModelManager::ModelData modelEnemyBullet_;

	// ---------------------------------------------------------
	// 敵
	// ---------------------------------------------------------

	// 敵のコンテナ
	std::vector<std::unique_ptr<Enemy>> enemies_;

	// ボス（単体で管理）
	std::unique_ptr<BossEnemy> bossEnemy_;

	// ---------------------------------------------------------
	// その他
	// ---------------------------------------------------------

	// プレイヤーのポインタ
	Player* player_ = nullptr;
};
