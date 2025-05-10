#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void EnemyManager::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	各モデル読み込み
	/// 
	
	// 通常敵
	modelNormalEnemy_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelNormalEnemy_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// 追加

}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void EnemyManager::Update()
{
	// 全ての敵を更新
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	// 敵の削除処理
	for (auto& enemy : enemies_) {
		if (enemy->IsDead()) {
			enemy->OnDestroy();
		}
	}
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<Enemy>& enemy) {
				return enemy->IsDead();
			}),
		enemies_.end()
	);
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void EnemyManager::Draw()
{
	// 全ての敵を描画
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}


#ifdef _DEBUG
	// デバッグ表示
	Debug();
#endif // _DEBUG
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void EnemyManager::DrawUI()
{
	// 全ての敵のUIを描画
	for (auto& enemy : enemies_) {
		enemy->DrawUI();
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void EnemyManager::Debug()
{
#ifdef _DEBUG

	ImGui::Begin("enemyManager");

	// スポーンボタン（デバッグ用）
	if (ImGui::Button("spawn")) {
		auto enemy = std::make_unique<NormalEnemy>();
		enemy->Initialize({ -5.0f, 1.0f, 0.0f }, &modelNormalEnemy_);

		enemies_.push_back(std::move(enemy));
	}

	ImGui::Separator();
	ImGui::Text("Total Enemies: %zu", enemies_.size());

	// 敵ごとの情報表示
	for (size_t i = 0; i < enemies_.size(); ++i) {
		Enemy* enemy = enemies_[i].get();
		if (!enemy) continue;

		std::string label = "Enemy[" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {

			// タイプの表示
			ImGui::Text("Tag : %s", enemy->GetTag().c_str());

			// 座標の表示
			const Float3& translate = enemy->GetTranslate();
			ImGui::Text("Translate : (%.2f, %.2f, %.2f)", translate.x, translate.y, translate.z);

			// HPの表示
			ImGui::Text("HP : %d", enemy->GetHP());

			// ここから他の項目追加

			ImGui::TreePop();
		}
	}

	ImGui::End();

#endif // _DEBUG
}
