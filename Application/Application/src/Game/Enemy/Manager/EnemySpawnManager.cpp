#include "EnemySpawnManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void EnemySpawnManager::Initialize()
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
void EnemySpawnManager::Update()
{
	// 全ての敵を更新
	for (auto& enemy : enemies_) {
		enemy->Update();
	}
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void EnemySpawnManager::Draw()
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
// デバッグ表示
// ---------------------------------------------------------
void EnemySpawnManager::Debug()
{
#ifdef _DEBUG

	ImGui::Begin("enemySpawnManager");

	/* spawnTest */

	if (ImGui::Button("spawn")) {
		auto enemy = std::make_unique<NormalEnemy>();
		enemy->Initialize({ -5.0f, 1.0f, 0.0f }, &modelNormalEnemy_);

		enemies_.push_back(std::move(enemy));
	}

	/*  */

	ImGui::End();

#endif // _DEBUG
}
