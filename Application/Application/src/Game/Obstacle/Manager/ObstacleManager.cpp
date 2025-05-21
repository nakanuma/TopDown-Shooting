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
	modelNormalObstacle_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelNormalObstacle_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// 新しく追加

	///
	///	各障害物の生成
	/// 
	
	for (const auto& data : datas) {
		// 通常障害物
		if (data.tag == "NORMAL_OBSTACLE") {
			auto obstacle = std::make_unique<NormalObstacle>();
			obstacle->Initialize(data.translate, data.scale, &modelNormalObstacle_);

			obstacles_.emplace_back(std::move(obstacle));
		}
	}
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void ObstacleManager::Update() 
{
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		obstacle->Update();
	}
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void ObstacleManager::Draw() 
{
	// 全ての障害物を更新
	for (auto& obstacle : obstacles_) {
		obstacle->Draw();
	}

#ifdef _DEBUG
	// デバッグ表示
	Debug();
#endif //  _DEBUG
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void ObstacleManager::Debug() 
{
#ifdef _DEBUG
	ImGui::Begin("obstacleManager");

	// スポーンボタン（デバッグ用）
	if (ImGui::Button("spawn")) {
		auto obstacle = std::make_unique<NormalObstacle>();
		obstacle->Initialize({10.0f, 1.0f, 0.0f}, {3.0f, 1.0f, 3.0f}, &modelNormalObstacle_);

		obstacles_.emplace_back(std::move(obstacle));
	}

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
