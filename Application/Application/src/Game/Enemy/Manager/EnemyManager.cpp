#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>
#include <src/Game/Enemy/ImmobileEnemy/ImmobileEnemy.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void EnemyManager::Initialize(const std::vector<Loader::TransformData> datas) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	各モデル読み込み
	///

	// 通常敵モデル
	modelNormalEnemy_ = ModelManager::LoadModelFile("resources/Models", "Character/Enemy/NormalEnemy/normalEnemy.obj", dxBase->GetDevice());
	modelNormalEnemy_.material.textureHandle = TextureManager::Load("resources/Images/Character/Enemy/NormalEnemy/normalEnemy.png", dxBase->GetDevice());
	
	// 固定敵モデル
	modelImmobileEnemy_ = ModelManager::LoadModelFile("resources/Models", "Character/Enemy/ImmobileEnemy/immobileEnemy.obj", dxBase->GetDevice());
	modelImmobileEnemy_.material.textureHandle = TextureManager::Load("resources/Images/Character/Enemy/ImmobileEnemy/immobileEnemy.png", dxBase->GetDevice());

	// ボスモデル
	/*modelBossEnemy_ = ModelManager::LoadModelFile("resources/Models", "Character/Enemy/BossEnemy/bossEnemy.obj", dxBase->GetDevice());
	modelBossEnemy_.material.textureHandle = TextureManager::Load("resources/Images/Character/Enemy/BossEnemy/bossEnemy.png", dxBase->GetDevice());*/

	// （追加）敵モデル


	// 弾モデル
	modelEnemyBullet_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	modelEnemyBullet_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	///
	///	各敵の生成
	///

	for (const auto& data : datas) {
		// 通常敵
		if (data.tag == "NORMAL_ENEMY") {
			auto enemy = std::make_unique<NormalEnemy>();
			enemy->Initialize(data.translate, &modelNormalEnemy_);
			enemy->SetBulletModel(&modelEnemyBullet_);
			enemies_.emplace_back(std::move(enemy));
		}

		// 固定敵
		if (data.tag == "IMMOBILE_ENEMY") {
			auto enemy = std::make_unique<ImmobileEnemy>();
			enemy->Initialize(data.translate, &modelImmobileEnemy_);
			enemy->SetBulletModel(&modelEnemyBullet_);
			enemies_.emplace_back(std::move(enemy));
		}

		// ボス生成
		/*if (data.tag == "BOSS_ENEMY") {
			bossEnemy_ = std::make_unique<BossEnemy>();

		}*/
	}

	// 一旦手動でボス追加
	/*bossEnemy_ = std::make_unique<BossEnemy>();
	bossEnemy_->Initialize({ 0.0f, 3.0f, 20.0f }, &modelBossEnemy_);*/

}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void EnemyManager::Update() {
	// 全ての敵を更新
	for (auto& enemy : enemies_) {
		enemy->Update(player_);
	}
	// ボス更新
	/*if (bossEnemy_) {
		bossEnemy_->Update(player_);
	}*/

	// 敵の削除処理
	for (auto& enemy : enemies_) {
		if (enemy->IsDead()) {
			enemy->OnDestroy();
		}
	}
	enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(), 
		[](const std::unique_ptr<Enemy>& enemy) 
		{ return enemy->IsDead(); }), enemies_.end());
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void EnemyManager::Draw() {
	// 全ての敵を描画
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}
	// ボス描画
	/*if (bossEnemy_) {
		bossEnemy_->Draw();
	}*/

#ifdef _DEBUG
	// デバッグ表示
	Debug();
#endif // _DEBUG
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void EnemyManager::DrawUI() {
	// 全ての敵のUIを描画
	for (auto& enemy : enemies_) {
		enemy->DrawUI();
	}
	// ボスのUIを描画
	/*if (bossEnemy_) {
		bossEnemy_->DrawUI();
	}*/
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void EnemyManager::Debug() {
#ifdef _DEBUG

	ImGui::Begin("enemyManager");

	// スポーンボタン（デバッグ用）
	if (ImGui::Button("spawn")) {
		auto enemy = std::make_unique<NormalEnemy>();
		enemy->Initialize({0.0f, 1.0f, 0.0f}, &modelNormalEnemy_);

		enemies_.emplace_back(std::move(enemy));
	}

	ImGui::Separator();
	ImGui::Text("Total Enemies: %zu", enemies_.size());

	// 敵ごとの情報表示
	for (size_t i = 0; i < enemies_.size(); ++i) {
		Enemy* enemy = enemies_[i].get();
		if (!enemy)
			continue;

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
				// 現在ステートの表示
				const char* stateName = "Empty";
				switch (normalEnemy->GetState()) {
				case EnemyState::Alert:
					stateName = "Alert";
					break;
				case EnemyState::Move:
					stateName = "Move";
					break;
				case EnemyState::Attack:
					stateName = "Attack";
					break;
				}
				ImGui::Text("State : %s", stateName);

				// リロード中？
				if (normalEnemy->isReloading_) {
					ImGui::Text("isReloading : true");
				} else {
					ImGui::Text("isReloading : false");
				}

				// リロード時間
				ImGui::Text("reloadTimer : %.2f", normalEnemy->reloadTimer_);

				// 残弾数
				ImGui::Text("remainingBullets : %d", normalEnemy->bulletRemaining_);

			}

			ImGui::TreePop();
		}
	}

	ImGui::End();

#endif // _DEBUG
}
