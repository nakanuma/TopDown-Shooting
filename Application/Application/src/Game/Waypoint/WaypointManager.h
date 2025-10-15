#pragma once

// Engine
#include <ModelManager.h>
#include <memory>

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Waypoint/Waypoint.h>

/// <summary>
/// ノード1つ分の情報を保持する構造体
/// </summary>
struct NodeRecord {
	Waypoint* wp;         // このノードのウェイポイント
	Waypoint* cameFrom;   // このノードに到達する直前のノード（親）
	float costSoFar;      // スタートからこのノードまでにかかったコスト（g値）
	float estimatedTotal; // このノードを通った場合の推定総コスト（f値 = g値 + h値）

	// 推定総コストが小さいノードを優先的に取り出すための比較演算子
	bool operator>(const NodeRecord& other) const { return estimatedTotal > other.estimatedTotal; }
};

/// <summary>
/// ウェイポイントを管理するクラス
/// </summary>
class WaypointManager {
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
	/// 隣接ノードの計算
	/// </summary>
	void ComputeNeighbors();

	/// <summary>
	/// 最も近いウェイポイントを返す
	/// </summary>
	Waypoint* FindClosestWaypoint(const Float3& pos);

	/// <summary>
	/// A*アルゴリズムを用いた経路探索
	/// </summary>
	std::vector<Waypoint*> FindPath(Waypoint* start, Waypoint* goal);

	/// <summary>
	/// ウェイポイントのリストを取得
	/// </summary>
	const std::vector<std::unique_ptr<Waypoint>>& GetWaypoints() const { return waypoints_; }

private:
	std::vector<std::unique_ptr<Waypoint>> waypoints_;
	float maxDistance_ = 4.0f; // 隣接ノードの最大距離

	const float waypointRadius_ = 0.25f; // ウェイポイントの生成時衝突判定に使用する半径

	// デバッグ用球モデル
	ModelManager::ModelData modelSphere_;
};
