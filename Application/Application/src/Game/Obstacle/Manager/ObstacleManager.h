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
	void Update(const Float3& playerPos);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Float3& playerPos);

	/// <summary>
	/// シャドウマップ用描画処理
	/// </summary>
	void DrawShadow(const Float3& playerPos);

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
	// 鉄骨の束
	ModelManager::ModelData modelSteelBundle_;
	// 縦型タンク
	ModelManager::ModelData modelVerticalTank_;
	// ISOタンク
	ModelManager::ModelData modelISOTank_;
	// IBCコンテナ
	ModelManager::ModelData modelIBCContainer_;

	// ---------------------------------------------------------
	// リスト
	// ---------------------------------------------------------

	// 障害物のリスト
	std::vector<std::unique_ptr<Obstacle>> obstacles_;

	// タグごとのモデルをマップで保持
	std::unordered_map<std::string, ModelManager::ModelData*> tagModelMap_{};

	// ---------------------------------------------------------
	// その他
	// ---------------------------------------------------------

	const float kActiveDistance = 50.0f;
};
