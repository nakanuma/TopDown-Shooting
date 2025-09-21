#include "ObstacleManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Obstacle/NormalObstacle/NormalObstacle.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void ObstacleManager::Initialize(const std::vector<Loader::TransformData> datas) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	各モデル読み込み
	///

	// 通常障害物
	modelNormalObstacle_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/TestObstacle/testObstacle.obj", dxBase->GetDevice());
	modelNormalObstacle_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	// コンテナ
	modelContainer_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/Container/container.obj", dxBase->GetDevice());
	modelContainer_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/container.png", dxBase->GetDevice());
	// フェンス
	modelFence_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/Fence/fence.obj", dxBase->GetDevice());
	modelFence_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/fence.png", dxBase->GetDevice());
	// 壁
	modelWall_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/Wall/wall.obj", dxBase->GetDevice());
	modelWall_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/wall.png", dxBase->GetDevice());
	// レンガのパレット積み
	modelBrickPallet_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/BrickPallet/brickPallet.obj", dxBase->GetDevice());
	modelBrickPallet_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/brickPallet.png", dxBase->GetDevice());
	// コンクリートバリア
	modelConcreteBarrier_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/ConcreteBarrier/concreteBarrier.obj", dxBase->GetDevice());
	modelConcreteBarrier_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/concreteBarrier.png", dxBase->GetDevice());
	// ドラム缶
	modelDrumCan_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/DrumCan/drumCan.obj", dxBase->GetDevice());
	modelDrumCan_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/drumCan.png", dxBase->GetDevice());
	// 貯水タンク
	modelWaterTank_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/WaterTank/waterTank.obj", dxBase->GetDevice());
	modelWaterTank_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/waterTank.png", dxBase->GetDevice());
	// 鉄骨の束
	modelSteelBundle_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/SteelBundle/steelBundle.obj", dxBase->GetDevice());
	modelSteelBundle_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/steelBundle.png", dxBase->GetDevice());
	// 縦型タンク
	modelVerticalTank_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/VerticalTank/verticalTank.obj", dxBase->GetDevice());
	modelVerticalTank_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/verticalTank.png", dxBase->GetDevice());
	// ISOタンク
	modelISOTank_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/ISOTank/ISOTank.obj", dxBase->GetDevice());
	modelISOTank_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/ISOTank.png", dxBase->GetDevice());
	// IBCコンテナ
	modelIBCContainer_ = ModelManager::LoadModelFile("resources/Models", "Obstacle/IBCContainer/IBCContainer.obj", dxBase->GetDevice());
	modelIBCContainer_.material.textureHandle = TextureManager::Load("resources/Images/Obstacle/IBCContainer.png", dxBase->GetDevice());

	///
	///	各障害物の生成
	///

	Reload(datas);
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void ObstacleManager::Update() {
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		obstacle->Update();
	}
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void ObstacleManager::Draw() {
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		obstacle->Draw();
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 再生成処理
// ---------------------------------------------------------
void ObstacleManager::Reload(const std::vector<Loader::TransformData> datas)
{
	// 破棄を行ってからリストをクリア
	for (auto& obstacle : obstacles_) {
		obstacle->OnDestroy();
	}
	obstacles_.clear();

	///
	///	各障害物の再生成
	///

	for (const auto& data : datas) {
		// 通常障害物
		if (data.tag == "NORMAL_OBSTACLE") {
			auto obstacle = std::make_unique<NormalObstacle>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelNormalObstacle_);

			obstacles_.emplace_back(std::move(obstacle));
		}
		// コンテナ
		if (data.tag == "CONTAINER") {
			auto obstacle = std::make_unique<Container>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelContainer_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// フェンス
		if (data.tag == "FENCE") {
			auto obstacle = std::make_unique<Fence>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelFence_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// 壁
		if (data.tag == "WALL") {
			auto obstacle = std::make_unique<Wall>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelWall_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// レンガのパレット積み
		if (data.tag == "BRICKPALLET") {
			auto obstacle = std::make_unique<BrickPallet>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelBrickPallet_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// コンクリートバリア
		if (data.tag == "CONCRETEBARRIER") {
			auto obstacle = std::make_unique<ConcreteBarrier>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelConcreteBarrier_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// ドラム缶
		if (data.tag == "DRUMCAN") {
			auto obstacle = std::make_unique<DrumCan>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelDrumCan_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// 貯水タンク
		if (data.tag == "WATERTANK") {
			auto obstacle = std::make_unique<WaterTank>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelWaterTank_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// 鉄骨の束
		if (data.tag == "STEELBUNDLE") {
			auto obstacle = std::make_unique<SteelBundle>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelSteelBundle_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// 縦型タンク
		if (data.tag == "VERTICALTANK") {
			auto obstacle = std::make_unique<VerticalTank>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelVerticalTank_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// ISOタンク
		if (data.tag == "ISOTANK") {
			auto obstacle = std::make_unique<VerticalTank>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelISOTank_);
			obstacles_.emplace_back(std::move(obstacle));
		}
		// IBCコンテナ
		if (data.tag == "IBCCONTAINER") {
			auto obstacle = std::make_unique<IBCContainer>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, &modelIBCContainer_);
			obstacles_.emplace_back(std::move(obstacle));
		}
	}
}
