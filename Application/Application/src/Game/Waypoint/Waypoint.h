#pragma once

// C++
#include <string>
#include <vector>

// Engine
#include <MyMath.h>
#include <Object3D.h>

/// <summary>
/// ウェイポイント（経路探索用）
/// </summary>
class Waypoint
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Waypoint(const std::string& name, const Float3& pos, ModelManager::ModelData* model);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 名前を取得
	/// </summary>
	const std::string& GetName() const { return name_; }

	/// <summary>
	/// 位置を取得
	/// </summary>
	const Float3& GetPosition() const { return position_; }

	/// <summary>
	/// 隣接ノードを追加
	/// </summary>
	void AddNeighbor(Waypoint* neighbor) { neighbors_.push_back(neighbor); }

	/// <summary>
	/// 隣接ノードのリストをクリア
	/// </summary>
	void ClearNeighbors() { neighbors_.clear(); }

	/// <summary>
	/// 隣接ノードを取得
	/// </summary>
	const std::vector<Waypoint*>& GetNeighbors() const { return neighbors_; }

	bool isSelected_;

private:
	std::string name_; // ウェイポイント名
	Float3 position_; // 位置
	std::vector<Waypoint*> neighbors_; // 隣接ノード

	// オブジェクト
	std::unique_ptr<Object3D> objectSphere_;
};

