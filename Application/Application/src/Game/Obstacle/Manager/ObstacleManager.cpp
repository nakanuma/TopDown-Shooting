#include "ObstacleManager.h"

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void ObstacleManager::Initialize(const std::vector<Loader::TransformData> datas) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	各モデル読み込み
	///

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

	// タグに対応したモデルをマップに保存
	tagModelMap_ = {
		{"CONTAINER", &modelContainer_},
		{"FENCE", &modelFence_},
		{"WALL", &modelWall_},
		{"BRICKPALLET", &modelBrickPallet_},
		{"CONCRETEBARRIER", &modelConcreteBarrier_},
		{"DRUMCAN", &modelDrumCan_},
		{"WATERTANK", &modelWaterTank_},
		{"STEELBUNDLE", &modelSteelBundle_},
		{"VERTICALTANK", &modelVerticalTank_},
		{"ISOTANK", &modelISOTank_},
		{"IBCCONTAINER", &modelIBCContainer_}
	};

	///
	///	各障害物の生成
	///

	Reload(datas);
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// シャドウマップ用描画処理
// ---------------------------------------------------------
void ObstacleManager::DrawShadow()
{
	for (auto& obstacle : obstacles_) {
		obstacle->DrawShadow();
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
	///	各障害物の生成
	///

	for (const auto& data : datas) {
		auto it = tagModelMap_.find(data.tag);
		if (it != tagModelMap_.end()) {
			auto obstacle = std::make_unique<Obstacle>();
			obstacle->Initialize(data.translate, data.scale, data.rotate, data.colliderSize, it->second);
			obstacles_.emplace_back(std::move(obstacle));
		}
	}
}
