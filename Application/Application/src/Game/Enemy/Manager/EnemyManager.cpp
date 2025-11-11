#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/ImmobileEnemy/ImmobileEnemy.h>
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>
#include <src/Game/Player/Player.h>

void EnemyManager::Initialize(const std::vector<Loader::TransformData>& datas, Player* player) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// プレイヤーのポインタを受け取る
	player_ = player;

	///
	///	各敵の生成
	///

	// 破棄を行ってからリストをクリア
	for (auto& enemy : enemies_) {
		enemy->OnDestroy();
	}
	enemies_.clear();

	for (const auto& data : datas) {
		// 通常敵の生成・初期化
		if (data.tag == "NORMAL_ENEMY") {
			auto enemy = std::make_unique<NormalEnemy>();
			enemy->Initialize(data.translate, &ModelManager::GetInstance()->GetModel("NormalEnemy"), player_);
			enemies_.emplace_back(std::move(enemy));
		}

		// 固定敵の生成・初期化
		if (data.tag == "IMMOBILE_ENEMY") {
			auto enemy = std::make_unique<ImmobileEnemy>();
			enemy->Initialize(data.translate, &ModelManager::GetInstance()->GetModel("ImmobileEnemy"), player_);
			enemies_.emplace_back(std::move(enemy));
		}

		// ボスの生成・初期化
		if (data.tag == "BOSS_ENEMY") {
			auto enemy = std::make_unique<BossEnemy>();
			enemy->Initialize(data.translate, &ModelManager::GetInstance()->GetModel("BossEnemy"), player_);
			enemies_.emplace_back(std::move(enemy));
		}
	}
}

void EnemyManager::Update() {
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
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), [](const std::unique_ptr<Enemy>& enemy) { return enemy->IsDead(); }), enemies_.end());
}

void EnemyManager::Draw() {
	// 全ての敵を描画
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}
}

void EnemyManager::DrawShadow() {
	// 全ての敵の影を描画
	for (auto& enemy : enemies_) {
		enemy->DrawShadow();
	}
}

void EnemyManager::DrawUI() {
	// 全ての敵のUIを描画
	for (auto& enemy : enemies_) {
		enemy->DrawUI();
	}
}

void EnemyManager::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("enemyManager");

	ImGui::Separator();
	ImGui::Text("Total Enemies: %zu", enemies_.size());

	// 敵ごとの情報表示
	for (size_t i = 0; i < enemies_.size(); ++i) {
		Enemy* enemy = enemies_[i].get();
		if (!enemy)
			continue;

		/* BossEnemy（一旦ここでデバッグ表示） */
		if (BossEnemy* bossEnemy = dynamic_cast<BossEnemy*>(enemy)) {
			bossEnemy->Debug();
		} else if (NormalEnemy* normalEnemy = dynamic_cast<NormalEnemy*>(enemy)) {
			normalEnemy->Debug();
		}

		std::string label = "Enemy[" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {

			///
			///	共通の情報を表示
			///

			// タイプの表示
			ImGui::Text("Tag : %s", enemy->GetTag().c_str());

			// 座標の表示
			const Float3& translate = enemy->GetTranslate();
			ImGui::Text("Translate : (%.2f, %.2f, %.2f)", translate.x, translate.y, translate.z);

			// HPの表示
			ImGui::Text("HP : %d", enemy->GetHP());

			// 他の項目追加

			///
			///	種類毎の情報を表示
			///

			/* NormalEnemy */
			if (NormalEnemy* normalEnemy = dynamic_cast<NormalEnemy*>(enemy)) {
			}

			/* ImmobileEnemy */
			if (ImmobileEnemy* immobileEnemy = dynamic_cast<ImmobileEnemy*>(enemy)) {
			}

			ImGui::TreePop();
		}
	}

	ImGui::End();
#endif
}
