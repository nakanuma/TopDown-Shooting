#include "ObstacleManager.h"

// Externals
#include <ImguiWrapper.h>

void ObstacleManager::Initialize(const std::vector<Loader::TransformData>& datas) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// タグに対応したモデルデータキーをマップに保存
	tagModelMap_ = {
	    {"CONTAINER",       "Container"      },
	    {"FENCE",           "Fence"          },
	    {"WALL",            "Wall"           },
	    {"BRICKPALLET",     "BrickPallet"    },
	    {"CONCRETEBARRIER", "ConcreteBarrier"},
	    {"DRUMCAN",         "DrumCan"        },
	    {"WATERTANK",       "WaterTank"      },
	    {"STEELBUNDLE",     "SteelBundle"    },
	    {"VERTICALTANK",    "VerticalTank"   },
	    {"ISOTANK",         "ISOTank"        },
	    {"IBCCONTAINER",    "IBCContainer"   }
    };

	///
	///	各障害物の生成
	///

	// 破棄を行ってからリストをクリア
	for (auto& obstacle : obstacles_) {
		obstacle->OnDestroy();
	}
	obstacles_.clear();

	for (const auto& data : datas) {
		auto it = tagModelMap_.find(data.tag);
		// タグと一致した障害物のモデルを適用して生成
		if (it != tagModelMap_.end()) {
			auto obstacle = std::make_unique<Obstacle>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, data.colliderSize, &ModelManager::GetInstance()->GetModel(it->second));
			obstacles_.emplace_back(std::move(obstacle));
		}
	}
}

void ObstacleManager::Update(const Float3& playerPos) {
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		Float3 diff = obstacle->GetTranslate() - playerPos;
		float distSq = Float3::LengthSq(diff);

		// プレイヤーから一定距離内のみ更新
		if (distSq <= kActiveDistance * kActiveDistance) {
			obstacle->Update();
			obstacle->SetActiveCollider(true);
		} else {
			obstacle->SetActiveCollider(false);
		}
	}
}

void ObstacleManager::Draw(const Float3& playerPos) {
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		Float3 diff = obstacle->GetTranslate() - playerPos;
		float distSq = Float3::LengthSq(diff);

		// プレイヤーから一定距離内のみ描画
		if (distSq <= kActiveDistance * kActiveDistance) {
			obstacle->Draw();
		}
	}
}

void ObstacleManager::DrawShadow(const Float3& playerPos) {
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		Float3 diff = obstacle->GetTranslate() - playerPos;
		float distSq = Float3::LengthSq(diff);

		// プレイヤーから一定距離内のみ描画
		if (distSq <= kActiveDistance * kActiveDistance) {
			obstacle->DrawShadow();
		}
	}
}

void ObstacleManager::Debug() {
#ifdef _DEBUG
	ImGui::Begin("obstacleManager");

	ImGui::Separator();
	ImGui::Text("Total Obstacles: %zu", obstacles_.size());

	// 障害物ごとの情報表示
	for (size_t i = 0; i < obstacles_.size(); ++i) {
		Obstacle* obstacle = obstacles_[i].get();
		if (!obstacle)
			continue;

		std::string label = "Obstacle[" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {

			// タイプの表示
			ImGui::Text("Tag : %s", obstacle->GetTag().c_str());

			// 座標の表示
			const Float3& translate = obstacle->GetTranslate();
			ImGui::Text("Translate : (%.2f, %.2f, %.2f)", translate.x, translate.y, translate.z);

			// スケールの表示
			const Float3& scale = obstacle->GetScale();
			ImGui::Text("Scale : (%.2f, %.2f, %.2f)", scale.x, scale.y, scale.z);

			// ここから他の項目追加

			ImGui::TreePop();
		}
	}

	ImGui::End();
#endif
}
