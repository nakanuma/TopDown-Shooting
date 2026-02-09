#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Player/Player.h>

void EnemyManager::Initialize(const std::vector<Loader::TransformData>& datas, Player* player) {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();
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
			enemy->Initialize(data.translate, player_);
			enemies_.emplace_back(std::move(enemy));
		}

		// 固定敵の生成・初期化
		if (data.tag == "IMMOBILE_ENEMY") {
			auto enemy = std::make_unique<ImmobileEnemy>();
			enemy->Initialize(data.translate, player_);
			enemies_.emplace_back(std::move(enemy));
		}

		// ボスの生成・初期化
		if (data.tag == "BOSS_ENEMY") {
			auto enemy = std::make_unique<BossEnemy>();
			enemy->Initialize(data.translate, player_);
			enemies_.emplace_back(std::move(enemy));
		}
	}
}

void EnemyManager::Finalize() { }

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
	// 情報・監視ウィンドウ
	ImGui::Begin("EnemyManager");
	ImGui::Text("Total Enemies: %zu", enemies_.size());
	ImGui::Separator();

	for(size_t i = 0; i < enemies_.size(); ++i) {
		Enemy* enemy = enemies_[i].get();
		if (!enemy) continue;

		std::string label = "[" + std::to_string(i) + "]" + enemy->GetTag();
	}
	ImGui::End();
#endif
}

BossEnemy* EnemyManager::GetBoss() const
{
	// 全ての敵の中からボスを探す
	for (const auto& enemy : enemies_) {
		if (BossEnemy* boss = dynamic_cast<BossEnemy*>(enemy.get())) {
			// 死亡中かどうかを返す
			return boss;
		}
	}
	return nullptr;
}
