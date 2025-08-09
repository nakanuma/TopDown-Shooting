#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>
#include <src/Game/Enemy/ImmobileEnemy/ImmobileEnemy.h>
#include <src/Game/Player/Player.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void EnemyManager::Initialize(const std::vector<Loader::TransformData> datas, Player* player) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// プレイヤーのポインタを受け取る
	player_ = player;

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
	modelBossEnemy_ = ModelManager::LoadModelFile("resources/Models", "Character/Enemy/BossEnemy/bossEnemy.obj", dxBase->GetDevice());
	modelBossEnemy_.material.textureHandle = TextureManager::Load("resources/Images/Character/Enemy/BossEnemy/bossEnemy.png", dxBase->GetDevice());

	// （追加）敵モデル


	// 弾モデル
	modelEnemyBullet_ = ModelManager::LoadModelFile("resources/Models", "Bullet/TestBullet/testBullet.obj", dxBase->GetDevice());
	modelEnemyBullet_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// ミサイルモデル
	modelMissile_ = ModelManager::LoadModelFile("resources/Models", "Bullet/Missile/missile.obj", dxBase->GetDevice());
	modelMissile_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	///
	///	各敵の生成
	///

	Reload(datas);
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
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
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void EnemyManager::DrawUI() {
	// 全ての敵のUIを描画
	for (auto& enemy : enemies_) {
		enemy->DrawUI();
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void EnemyManager::Debug() {
#ifdef _DEBUG

	ImGui::Begin("enemyManager");

	// スポーンボタン（デバッグ用）
	if (ImGui::Button("spawn")) {
		auto enemy = std::make_unique<BossEnemy>();
		enemy->Initialize({0.0f, 3.0f, 0.0f}, &modelBossEnemy_, player_);

		enemies_.emplace_back(std::move(enemy));
	}

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

#endif // _DEBUG
}

// ---------------------------------------------------------
// 再生成処理
// ---------------------------------------------------------
void EnemyManager::Reload(const std::vector<Loader::TransformData> datas) {
	// 破棄を行ってからリストをクリア
	for (auto& enemy : enemies_) {
		enemy->OnDestroy();
	}
	enemies_.clear();

	for (const auto& data : datas) {
		// 通常敵
		if (data.tag == "NORMAL_ENEMY") {
			auto enemy = std::make_unique<NormalEnemy>();
			enemy->Initialize(data.translate, &modelNormalEnemy_, player_);
			enemy->SetBulletModel(&modelEnemyBullet_);
			enemies_.emplace_back(std::move(enemy));
		}

		// 固定敵
		if (data.tag == "IMMOBILE_ENEMY") {
			auto enemy = std::make_unique<ImmobileEnemy>();
			enemy->Initialize(data.translate, &modelImmobileEnemy_, player_);
			enemy->SetBulletModel(&modelEnemyBullet_);
			enemies_.emplace_back(std::move(enemy));
		}

		// ボス生成
		/*if (data.tag == "BOSS_ENEMY") {
			bossEnemy_ = std::make_unique<BossEnemy>();

		}*/
	}

	// 一旦ここでボス生成
	auto enemy = std::make_unique<BossEnemy>();
	enemy->Initialize({ 0.0f, 3.0f, 0.0f }, &modelBossEnemy_, player_);
	enemy->SetMissileModel(&modelMissile_);
	enemies_.emplace_back(std::move(enemy));
}
