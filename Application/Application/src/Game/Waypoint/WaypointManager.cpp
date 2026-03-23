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

void WaypointManager::Initialize(const std::vector<Loader::TransformData>& datas) {
	waypoints_.clear();

	// データからウェイポイント配置範囲を探索して保存
	for (const auto& data : datas) {
		if (data.tag == "WAYPOINT_BOTTOMLEFT") {
			bottomLeft_ = data.translate;
		} else if (data.tag == "WAYPOINT_TOPRIGHT") {
			topRight_ = data.translate;
		}
	}

	uint32_t waypointID = 0; // カウント

	// 分割数（隣接ノードの距離に合わせる）
	float spacing = kMaxDistance;

	// 縦横のサイズを計算
	uint32_t numX = static_cast<uint32_t>(std::floor((topRight_.x - bottomLeft_.x) / spacing)) + 1;
	uint32_t numZ = static_cast<uint32_t>(std::floor((topRight_.z - bottomLeft_.z) / spacing)) + 1;

	// 生成時の衝突判定を行うコライダータグ
	std::unordered_set<std::string> checkTags = {"Obstacle"};

	for (uint32_t x = 0; x < numX; ++x) {
		for (uint32_t z = 0; z < numZ; ++z) {
			Cygnus::Float3 pos;
			pos.x = bottomLeft_.x + x * spacing;
			pos.y = 1.0;
			pos.z = bottomLeft_.z + z * spacing;

			// 衝突していたら生成スキップ
			if (Cygnus::CollisionManager::GetInstance()->CheckSphereCollisionWithTag(pos, kWaypointRadius, checkTags)) {
				continue;
			}

			std::string name = "WP_" + std::to_string(x) + "_" + std::to_string(z);
			waypoints_.push_back(std::make_unique<Waypoint>(name, pos, &Cygnus::ModelManager::GetInstance()->GetModel("Sphere")));
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
#ifdef USE_IMGUI
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
			// 位置
			const Cygnus::Float3& pos = wp->GetPosition();
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
#endif
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
			Cygnus::Float3 dir = wpB->GetPosition() - wpA->GetPosition();
			float dist = Cygnus::Float3::Length(dir);
			// 離れすぎていたらスキップ
			if (dist > kMaxDistance) {
				continue;
			}

			// レイキャストで無視するコライダーリスト
			std::unordered_set<std::string> ignoreTags = {"Player"};
			// レイキャストで障害物チェック
			Cygnus::RayCastHit hit{};
			bool rayHit = Cygnus::CollisionManager::GetInstance()->RayCast(wpA->GetPosition(), Cygnus::Float3::Normalize(dir), dist, &hit, ignoreTags);

			// ウェイポイント同士のレイが障害物に遮られているかチェック
			if (rayHit && hit.hitCollider->GetTag() == "NormalObstacle") {
				continue;
			}

			// 障害物に遮られていないウェイポイントのみが隣接ノードとして登録される
			wpA->AddNeighbor(wpB.get());
		}
	}
}

Waypoint* WaypointManager::FindClosestWaypoint(const Cygnus::Float3& pos) {
	Waypoint* closest = nullptr;
	float minDistSq = FLT_MAX; // 最初に必ず比較が行われるように安全のためfloatの最大値を入れておく

	// 全ウェイポイントをループ
	for (auto& wp : waypoints_) {
		// 距離を計算
		float distSq = Cygnus::Float3::LengthSq(wp->GetPosition() - pos);
		// これまでの最短距離よりも近ければ更新
		if (distSq < minDistSq) {
			minDistSq = distSq;
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
	openList.push({start, nullptr, 0.0f, Cygnus::Float3::LengthSq(goal->GetPosition() - start->GetPosition())});
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
			float newCost = costSoFar[current.wp] + Cygnus::Float3::LengthSq(neighbor->GetPosition() - current.wp->GetPosition());
			// まだ訪れていないノード、またはより安いコストで到達できる場合のみ更新
			if (costSoFar.find(neighbor) == costSoFar.end() || newCost < costSoFar[neighbor]) {
				costSoFar[neighbor] = newCost;
				// priorityを計算してオープンリストに追加
				float priority = newCost + Cygnus::Float3::LengthSq(neighbor->GetPosition() - goal->GetPosition());
				openList.push({neighbor, current.wp, newCost, priority});
				cameFrom[neighbor] = current.wp;
			}
		}
	}

	return {};
}
