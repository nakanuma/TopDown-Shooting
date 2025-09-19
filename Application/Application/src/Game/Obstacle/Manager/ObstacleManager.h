#pragma once

// Engine
#include <Engine/Model/ModelManager.h>

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Base/Obstacle.h>

/// <summary>
/// 障害物の管理
/// </summary>
class ObstacleManager {
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

	// 通常
	ModelManager::ModelData modelNormalObstacle_;

	// コンテナ
	ModelManager::ModelData modelContainer_;
	// フェンス
	ModelManager::ModelData modelFence_;
	// 壁
	ModelManager::ModelData modelWall_;
	// レンガのパレット積み
	ModelManager::ModelData modelBrickPallet_;
	// コンクリートバリア
	ModelManager::ModelData modelConcreteBarrier_;
	// ドラム缶
	ModelManager::ModelData modelDrumCan_;
	// 貯水タンク
	ModelManager::ModelData modelWaterTank_;

	// ---------------------------------------------------------
	// 敵
	// ---------------------------------------------------------

	// 敵のコンテナ
	std::vector<std::unique_ptr<Obstacle>> obstacles_;
};
