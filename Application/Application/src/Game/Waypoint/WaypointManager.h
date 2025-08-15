#pragma once

// Engine
#include <memory>
#include <ModelManager.h>

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Waypoint/Waypoint.h>

/// <summary>
/// ウェイポイントを管理
/// </summary>
class WaypointManager
{
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	static WaypointManager* GetInstance();

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
	/// ウェイポイントのリストを取得
	/// </summary>
	const std::vector<std::unique_ptr<Waypoint>>& GetWaypoints() const { return waypoints_; }

private:
	std::vector<std::unique_ptr<Waypoint>> waypoints_;
	float maxDistance_ = 50.0f; // 隣接ノードの最大距離

	// デバッグ用球モデル
	ModelManager::ModelData modelSphere_;
};

