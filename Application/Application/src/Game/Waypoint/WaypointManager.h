#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <ModelManager.h>
#include <memory>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Loader/Loader.h>
#include <src/Game/Waypoint/Waypoint.h>

/// <summary>
/// ノード1つ分の情報を保持する構造体
/// </summary>
struct NodeRecord {
	Waypoint* wp;         /* このノードのウェイポイント */
	Waypoint* cameFrom;   /* このノードに到達する直前のノード（親） */
	float costSoFar;      /* スタートからこのノードまでにかかったコスト（g値） */ 
	float estimatedTotal; /* このノードを通った場合の推定総コスト（f値 = g値 + h値） */

	/// <summary>
	/// 推定総コストが小さいノードを優先的に取り出すための比較演算子
	/// </summary>
	/// <param name="other">比較対象ノード</param>
	/// <returns>推定総コストが小さい場合はtrue</returns>
	bool operator>(const NodeRecord& other) const { return estimatedTotal > other.estimatedTotal; }
};

// =========================================================
// ウェイポイントの管理を行うクラス
// =========================================================
class WaypointManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスの取得を行います。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static WaypointManager* GetInstance();

	/// <summary>
	///	ウェイポイントの初期化処理を行います。 
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// 隣接ノードの計算を行います。
	/// </summary>
	void ComputeNeighbors();

	/// <summary>
	/// 最も近いウェイポイントを返します。
	/// </summary>
	/// <param name="pos">現在位置</param>
	/// <returns>最も近いウェイポイント</returns>
	Waypoint* FindClosestWaypoint(const Float3& pos);

	/// <summary>
	/// A*アルゴリズムを用いた経路探索を行います。
	/// </summary>
	/// <param name="start">初期位置</param>
	/// <param name="goal">目標位置</param>
	/// <returns>経路のリスト</returns>
	std::vector<Waypoint*> FindPath(Waypoint* start, Waypoint* goal);

	/// <summary>
	/// ウェイポイントのリストを取得します。
	/// </summary>
	/// <returns>ウェイポイントのリスト</returns>
	const std::vector<std::unique_ptr<Waypoint>>& GetWaypoints() const { return waypoints_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	// ----- Continer -----
	std::vector<std::unique_ptr<Waypoint>> waypoints_;		/* 全てのウェイポイントを格納するコンテナ */

	// ----- Parameters -----
	float maxDistance_ = 4.0f;								/* 隣接ノードの最大距離 */
	const float waypointRadius_ = 0.25f;					/* ウェイポイントの生成時衝突判定に使用する半径 */
};
