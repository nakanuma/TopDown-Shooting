#include "ObstacleManager.h"

// Externals
#include <ImguiWrapper.h>

void ObstacleManager::Initialize(const std::vector<Loader::TransformData> datas) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	各モデル読み込み
	///

	// コンテナ
	modelContainer_ = ModelManager::LoadModelFile("Obstacle/Container/container.obj");
	modelContainer_.material.textureHandle = TextureManager::Load("Obstacle/container.png");
	// フェンス
	modelFence_ = ModelManager::LoadModelFile("Obstacle/Fence/fence.obj");
	modelFence_.material.textureHandle = TextureManager::Load("Obstacle/fence.png");
	// 壁
	modelWall_ = ModelManager::LoadModelFile("Obstacle/Wall/wall.obj");
	modelWall_.material.textureHandle = TextureManager::Load("Obstacle/wall.png");
	// レンガのパレット積み
	modelBrickPallet_ = ModelManager::LoadModelFile("Obstacle/BrickPallet/brickPallet.obj");
	modelBrickPallet_.material.textureHandle = TextureManager::Load("Obstacle/brickPallet.png");
	// コンクリートバリア
	modelConcreteBarrier_ = ModelManager::LoadModelFile("Obstacle/ConcreteBarrier/concreteBarrier.obj");
	modelConcreteBarrier_.material.textureHandle = TextureManager::Load("Obstacle/concreteBarrier.png");
	// ドラム缶
	modelDrumCan_ = ModelManager::LoadModelFile("Obstacle/DrumCan/drumCan.obj");
	modelDrumCan_.material.textureHandle = TextureManager::Load("Obstacle/drumCan.png");
	// 貯水タンク
	modelWaterTank_ = ModelManager::LoadModelFile("Obstacle/WaterTank/waterTank.obj");
	modelWaterTank_.material.textureHandle = TextureManager::Load("Obstacle/waterTank.png");
	// 鉄骨の束
	modelSteelBundle_ = ModelManager::LoadModelFile("Obstacle/SteelBundle/steelBundle.obj");
	modelSteelBundle_.material.textureHandle = TextureManager::Load("Obstacle/steelBundle.png");
	// 縦型タンク
	modelVerticalTank_ = ModelManager::LoadModelFile("Obstacle/VerticalTank/verticalTank.obj");
	modelVerticalTank_.material.textureHandle = TextureManager::Load("Obstacle/verticalTank.png");
	// ISOタンク
	modelISOTank_ = ModelManager::LoadModelFile("Obstacle/ISOTank/ISOTank.obj");
	modelISOTank_.material.textureHandle = TextureManager::Load("Obstacle/ISOTank.png");
	// IBCコンテナ
	modelIBCContainer_ = ModelManager::LoadModelFile("Obstacle/IBCContainer/IBCContainer.obj");
	modelIBCContainer_.material.textureHandle = TextureManager::Load("Obstacle/IBCContainer.png");

	// タグに対応したモデルをマップに保存
	tagModelMap_ = {
	    {"CONTAINER",       &modelContainer_      },
	    {"FENCE",           &modelFence_          },
	    {"WALL",            &modelWall_           },
	    {"BRICKPALLET",     &modelBrickPallet_    },
	    {"CONCRETEBARRIER", &modelConcreteBarrier_},
	    {"DRUMCAN",         &modelDrumCan_        },
	    {"WATERTANK",       &modelWaterTank_      },
	    {"STEELBUNDLE",     &modelSteelBundle_    },
	    {"VERTICALTANK",    &modelVerticalTank_   },
	    {"ISOTANK",         &modelISOTank_        },
	    {"IBCCONTAINER",    &modelIBCContainer_   }
    };

	///
	///	各障害物の生成
	///

	Reload(datas);
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

void ObstacleManager::Reload(const std::vector<Loader::TransformData> datas) {
	// 破棄を行ってからリストをクリア
	for (auto& obstacle : obstacles_) {
		obstacle->OnDestroy();
	}
	obstacles_.clear();

	///
	///	各障害物の生成
	///

	for (const auto& data : datas) {
		auto it = tagModelMap_.find(data.tag);
		// タグと一致した障害物のモデルを適用して生成
		if (it != tagModelMap_.end()) {
			auto obstacle = std::make_unique<Obstacle>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, data.colliderSize, it->second);
			obstacles_.emplace_back(std::move(obstacle));
		}
	}
}
