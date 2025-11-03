#include "WaypointManager.h"

// C++
#include <algorithm>
#include <queue>
#include <unordered_map>

// Engine
#include <Engine/Collider/CollisionManager.h>
#include <Engine/Wrapper/ImguiWrapper.h>

WaypointManager* WaypointManager::GetInstance() {
	static WaypointManager instance;
	return &instance;
}

void WaypointManager::Initialize() {
	waypoints_.clear();

	uint32_t waypointID = 0; // カウント

	// 左下と右上を指定
	Float3 bottomLeft = {0.0f, 2.0f, 0.0f};
	Float3 topRight = {70.0f, 2.0f, 120.0f};
	// 分割数（隣接ノードの距離に合わせる）
	float spacing = maxDistance_;

	// 縦横のサイズを計算
	uint32_t numX = static_cast<uint32_t>(std::floor((topRight.x - bottomLeft.x) / spacing)) + 1;
	uint32_t numZ = static_cast<uint32_t>(std::floor((topRight.z - bottomLeft.z) / spacing)) + 1;

	// 生成時の衝突判定を行うコライダータグ
	std::unordered_set<std::string> checkTags = {"Obstacle"};

	for (uint32_t x = 0; x < numX; ++x) {
		for (uint32_t z = 0; z < numZ; ++z) {
			Float3 pos;
			pos.x = bottomLeft.x + x * spacing;
			pos.y = 1.0;
			pos.z = bottomLeft.z + z * spacing;

			// 衝突していたら生成スキップ
			if (CollisionManager::GetInstance()->CheckSphereCollisionWithTag(pos, waypointRadius_, checkTags)) {
				continue;
			}

			std::string name = "WP_" + std::to_string(x) + "_" + std::to_string(z);
			waypoints_.push_back(std::make_unique<Waypoint>(name, pos, &ModelManager::GetInstance()->GetModel("Sphere")));
		}
	}

	// 生成したウェイポイントの隣接ノードを計算（障害物があれば隣接ノードとして登録しない）
	ComputeNeighbors();
}

void WaypointManager::Update() {
	for (auto& wp : waypoints_) {
		wp->Update();
	}
}

/// <summary>
/// 描画処理
/// </summary>
void WaypointManager::Draw() {
#ifdef _DEBUG
	for (auto& wp : waypoints_) {
		wp->Draw();
	}
#endif
}

void WaypointManager::Debug() {
	ImGui::Begin("WaypointManager");

	ImGui::Text("Total Waypoints : %zu", waypoints_.size());
	ImGui::Separator();

	for (size_t i = 0; i < waypoints_.size(); ++i) {
		Waypoint* wp = waypoints_[i].get();
		if (!wp) {
			continue;
		}

		std::string label = wp->GetName();
		if (ImGui::TreeNode(label.c_str())) {
			// 選択切り替えボタン
			if (ImGui::Button("Select")) {
				wp->isSelected_ = !wp->isSelected_;
			}

			// 位置
			const Float3& pos = wp->GetPosition();
			ImGui::Text("Position : (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);

			// 隣接ノード
			const auto& neighbors = wp->GetNeighbors();
			if (!neighbors.empty()) {
				ImGui::Text("Neighbors : %zu", neighbors.size());
				ImGui::Indent();
				for (const auto& neighbor : neighbors) {
					ImGui::Text("- %s", neighbor->GetName().c_str());
				}
				ImGui::Unindent();
			}

			ImGui::TreePop();
		}

		ImGui::Separator();
	}

	ImGui::End();
}

void WaypointManager::ComputeNeighbors() {
	for (auto& wpA : waypoints_) {
		wpA->ClearNeighbors();

		for (auto& wpB : waypoints_) {
			// 同一ウェイポイントはスキップ
			if (wpA.get() == wpB.get()) {
				continue;
			}

			// 一定距離以内だけ接続対象とする
			Float3 dir = wpB->GetPosition() - wpA->GetPosition();
			float dist = Float3::Length(dir);
			// 離れすぎていたらスキップ
			if (dist > maxDistance_) {
				continue;
			}

			// レイキャストで無視するコライダーリスト
			std::unordered_set<std::string> ignoreTags = {"Player"};
			// レイキャストで障害物チェック
			RayCastHit hit{};
			bool rayHit = CollisionManager::GetInstance()->RayCast(wpA->GetPosition(), Float3::Normalize(dir), dist, &hit, ignoreTags);

			// ウェイポイント同士のレイが障害物に遮られているかチェック
			if (rayHit && hit.hitCollider->GetTag() == "NormalObstacle") {
				continue;
			}

			// 障害物に遮られていないウェイポイントのみが隣接ノードとして登録される
			wpA->AddNeighbor(wpB.get());
		}
	}
}

Waypoint* WaypointManager::FindClosestWaypoint(const Float3& pos) {
	Waypoint* closest = nullptr;
	float minDist = 3.402823466e+38f; // 最初に必ず比較が行われるように安全のためfloatの最大値を入れておく

	// 全ウェイポイントをループ
	for (auto& wp : waypoints_) {
		// 距離を計算
		float dist = Float3::Length(wp->GetPosition() - pos);
		// これまでの最短距離よりも近ければ更新
		if (dist < minDist) {
			minDist = dist;
			closest = wp.get();
		}
	}
	// 最も近いウェイポイントを返す
	return closest;
}

std::vector<Waypoint*> WaypointManager::FindPath(Waypoint* start, Waypoint* goal) {
	if (!start || !goal)
		return {};

	// 未探索ノードの管理を行う優先度付きキュー
	std::priority_queue<NodeRecord, std::vector<NodeRecord>, std::greater<>> openList;
	// どのウェイポイントから現在のウェイポイントに到達したかを記録
	std::unordered_map<Waypoint*, Waypoint*> cameFrom;
	// 開始点から各ウェイポイントまでの累積コストを記録
	std::unordered_map<Waypoint*, float> costSoFar;

	// 初期ノードの追加
	openList.push({start, nullptr, 0.0f, Float3::Length(goal->GetPosition() - start->GetPosition())});
	cameFrom[start] = nullptr;
	costSoFar[start] = 0.0f;

	while (!openList.empty()) {
		// 未探索ノードを取り出す
		NodeRecord current = openList.top();
		openList.pop(); // 取り出したノードはキューから削除

		// ゴールに到達したら経路復元
		if (current.wp == goal) {
			std::vector<Waypoint*> path;
			Waypoint* p = goal;
			// cameFromをたどってゴールから開始点まで逆順にノードを追加
			while (p) {
				path.push_back(p);
				p = cameFrom[p];
			}
			// 開始点->ゴールの順序に修正
			std::reverse(path.begin(), path.end());
			return path;
		}

		// 隣接ノードの探索
		for (auto neighbor : current.wp->GetNeighbors()) {
			// 開始点から隣接ノードまでの累積コストを計算
			float newCost = costSoFar[current.wp] + Float3::Length(neighbor->GetPosition() - current.wp->GetPosition());
			// まだ訪れていないノード、またはより安いコストで到達できる場合のみ更新
			if (costSoFar.find(neighbor) == costSoFar.end() || newCost < costSoFar[neighbor]) {
				costSoFar[neighbor] = newCost;
				// priorityを計算してオープンリストに追加
				float priority = newCost + Float3::Length(neighbor->GetPosition() - goal->GetPosition());
				openList.push({neighbor, current.wp, newCost, priority});
				cameFrom[neighbor] = current.wp;
			}
		}
	}

	return {};
}
