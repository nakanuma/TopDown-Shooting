#pragma once

// Engine
#include <Engine/Model/ModelManager.h>

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// 敵のスポーンを管理
/// </summary>
class EnemySpawnManager
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

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

	// 通常敵
	ModelManager::ModelData modelNormalEnemy_;
	
	// 追加


	// ---------------------------------------------------------
	// 敵
	// ---------------------------------------------------------

	// 敵のコンテナ
	std::vector<std::unique_ptr<Enemy>> enemies_;
};

