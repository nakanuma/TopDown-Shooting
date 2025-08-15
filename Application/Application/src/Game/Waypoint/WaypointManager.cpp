#include "WaypointManager.h"

// Engine
#include <Engine/Wrapper/ImguiWrapper.h>
#include <Engine/Collider/CollisionManager.h>

// ---------------------------------------------------------
// インスタンス取得
// ---------------------------------------------------------
WaypointManager* WaypointManager::GetInstance() {
	static WaypointManager instance;
	return &instance;
}

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void WaypointManager::Initialize(const std::vector<Loader::TransformData> datas)
{
	waypoints_.clear();

	uint32_t waypointID = 0; // カウント

	modelSphere_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", DirectXBase::GetInstance()->GetDevice());
	modelSphere_.material.textureHandle = TextureManager::Load("resources/Images/white.png", DirectXBase::GetInstance()->GetDevice());

	// ローダーから受け取ったデータからウェイポイントの生成
	for (const auto& data : datas) {
		if (data.tag == "WAYPOINT") {
			waypoints_.emplace_back(std::make_unique<Waypoint>(data.tag + "_" + std::to_string(waypointID), data.translate, &modelSphere_));
			waypointID++;
		}
	}
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void WaypointManager::Update()
{
	///
	///	隣接ノードを計算
	/// 

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
			std::unordered_set<std::string> ignoreTags = { "Player" };
			// レイキャストで障害物チェック
			RayCastHit hit{};
			bool rayHit = CollisionManager::GetInstance()->RayCast(
				wpA->GetPosition(),
				Float3::Normalize(dir),
				dist,
				&hit,
				ignoreTags
			);

			// ウェイポイント同士のレイが障害物に遮られているかチェック
			if (rayHit && hit.hitCollider->GetTag() == "NormalObstacle") {
				continue;
			}

			// 障害物に遮られていないウェイポイントのみが隣接ノードとして登録される
			wpA->AddNeighbor(wpB.get());
		}
	}

	// オブジェクト更新処理
	for (auto& wp : waypoints_) {
		wp->Update();
	}
}

/// <summary>
/// 描画処理
/// </summary>
void WaypointManager::Draw()
{
	for (auto& wp : waypoints_) {
		wp->Draw();
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void WaypointManager::Debug() 
{
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