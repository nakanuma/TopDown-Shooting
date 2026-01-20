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
	///	BehaviorTreeのセットアップ
	/// 

	// BehaviorTreeの構築
	normalEnemyBT_ = CreateNormalEnemyMasterTree();
	// BTEditorにセット
	normalEnemyBTEditor_ = std::make_unique<Cygnus::BehaviorTreeEditor<NormalEnemy>>();
	normalEnemyBTEditor_->SetBehaviorTree(normalEnemyBT_.get());
	normalEnemyBTEditor_->Load("normalEnemy.json");

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
			enemy->Initialize(data.translate, &Cygnus::ModelManager::GetInstance()->GetModel("NormalEnemy"), player_, normalEnemyBT_.get());
			enemies_.emplace_back(std::move(enemy));
		}

		// 固定敵の生成・初期化
		if (data.tag == "IMMOBILE_ENEMY") {
			auto enemy = std::make_unique<ImmobileEnemy>();
			enemy->Initialize(data.translate, &Cygnus::ModelManager::GetInstance()->GetModel("ImmobileEnemy"), player_);
			enemies_.emplace_back(std::move(enemy));
		}

		// ボスの生成・初期化
		if (data.tag == "BOSS_ENEMY") {
			auto enemy = std::make_unique<BossEnemy>();
			enemy->Initialize(data.translate, &Cygnus::ModelManager::GetInstance()->GetModel("BossEnemy"), player_);
			enemies_.emplace_back(std::move(enemy));
		}
	}
}

void EnemyManager::Finalize() { 
	if (normalEnemyBTEditor_) {
		normalEnemyBTEditor_->Finalize();
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
	// 共通エディタウィンドウ
	ImGui::Begin("Global BT Editor");
	/*if(ImGui::CollapsingHeader("NormalEnemy BehaviorTree")) {
		normalEnemyBTEditor_->Draw();
		if(ImGui::Button("SAVE")) {
			normalEnemyBTEditor_->Save("normalEnemy.json");
		}
	}*/

	normalEnemyBTEditor_->Draw();

	ImGui::End();

	// 情報ウィンドウ
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
			
		}

		std::string label = "Enemy[" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {

			///
			///	共通の情報を表示
			///

			// タイプの表示
			ImGui::Text("Tag : %s", enemy->GetTag().c_str());

			// 座標の表示
			const Cygnus::Float3& translate = enemy->GetTranslate();
			ImGui::Text("Translate : (%.2f, %.2f, %.2f)", translate.x, translate.y, translate.z);

			// HPの表示
			ImGui::Text("HP : %d", enemy->GetHP());

			// 他の項目追加

			///
			///	種類毎の情報を表示
			///

			/* NormalEnemy */
			if (NormalEnemy* normalEnemy = dynamic_cast<NormalEnemy*>(enemy)) {
				normalEnemy->Debug();
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

std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> EnemyManager::CreateNormalEnemyMasterTree()
{
	///
	///	索敵シーケンス
	///

	// 移動前待機
	auto waitBeforePatrol = std::make_unique<Cygnus::WaitNode<NormalEnemy>>(1.0f, 1.0f, "wait");

	// ランダム移動
	auto randomPatrol = std::make_unique<Cygnus::ActionNode<NormalEnemy>>(
		[](NormalEnemy* enemy, float dt) { return enemy->RandomPatrol(); }, "randomPatrol");

	// 回転前待機
	auto waitBeforeRotate = std::make_unique<Cygnus::WaitNode<NormalEnemy>>(1.0f, 1.0f, "wait");

	// ランダム回転
	auto randomRotate = std::make_unique<Cygnus::ActionNode<NormalEnemy>>(
		[](NormalEnemy* enemy, float dt) { return enemy->RandomRotate(); }, "randomRotate");

	// searchSequence構築
	auto searchSequence = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>("searchSequence");
	searchSequence->AddChild(std::move(waitBeforePatrol));
	searchSequence->AddChild(std::move(randomPatrol));
	searchSequence->AddChild(std::move(waitBeforeRotate));
	searchSequence->AddChild(std::move(randomRotate));

	///
	///	攻撃シーケンス
	///

	// 視界チェック
	auto isPlayerInSight = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>(
		[](NormalEnemy* enemy) { return enemy->IsPlayerInSight(); }, "isPlayerInSight");

	// プレイヤー方向を向く
	auto facePlayer = std::make_unique<Cygnus::ActionNode<NormalEnemy>>(
		[](NormalEnemy* enemy, float dt) { return enemy->FacePlayer(); }, "facePlayer");

	// 射撃を行う
	auto shoot = std::make_unique<Cygnus::ActionNode<NormalEnemy>>(
		[](NormalEnemy* enemy, float dt) { return enemy->Shoot(); }, "shoot");

	// attackParallel構築
	auto attackParallel = std::make_unique<Cygnus::ParallelNode<NormalEnemy>>("attackParallel");
	attackParallel->AddChild(std::move(facePlayer));
	attackParallel->AddChild(std::move(shoot));

	// attackSequence構築
	auto attackSequence = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>("attackSequence");
	attackSequence->AddChild(std::move(isPlayerInSight));
	attackSequence->AddChild(std::move(attackParallel));

	///
	/// 移動シーケンス
	///

	// 視界チェック
	auto isDetected = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>(
		[](NormalEnemy* enemy) { return enemy->IsDetected(); }, "isPlayerDetected");

	// 発見済みなら移動
	auto moveToPlayer = std::make_unique<Cygnus::ActionNode<NormalEnemy>>(
		[](NormalEnemy* enemy, float dt) { return enemy->MoveToPlayer(); }, "moveToPlayer");

	// moveSequence構築
	auto moveSequence = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>("moveSequence");
	moveSequence->AddChild(std::move(isDetected));
	moveSequence->AddChild(std::move(moveToPlayer));

	///
	///	攻撃セレクタ
	///

	// attackSelector構築
	auto attackSelector = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>("attackSelector");
	attackSelector->AddChild(std::move(attackSequence));
	attackSelector->AddChild(std::move(moveSequence));

	///
	///	ルートノード構築
	///

	auto root = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>("root");
	root->AddChild(std::move(attackSelector));
	root->AddChild(std::move(searchSequence));

	///
	///	BehaviorTree構築
	///

	return std::make_unique<Cygnus::BehaviorTree<NormalEnemy>>(std::move(root));
}
