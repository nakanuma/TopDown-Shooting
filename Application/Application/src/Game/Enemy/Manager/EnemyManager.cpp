#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>

// ---------------------------------------------------------
// ‰Šú‰»ˆ—
// ---------------------------------------------------------
void EnemyManager::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	Šeƒ‚ƒfƒ‹“Ç‚Ýž‚Ý
	/// 
	
	// ’Êí“G
	modelNormalEnemy_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelNormalEnemy_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// ’Ç‰Á

}

// ---------------------------------------------------------
// XVˆ—
// ---------------------------------------------------------
void EnemyManager::Update()
{
	// ‘S‚Ä‚Ì“G‚ðXV
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	// “G‚Ìíœˆ—
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
// •`‰æˆ—
// ---------------------------------------------------------
void EnemyManager::Draw()
{
	// ‘S‚Ä‚Ì“G‚ð•`‰æ
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}


#ifdef _DEBUG
	// ƒfƒoƒbƒO•\Ž¦
	Debug();
#endif // _DEBUG
}

// ---------------------------------------------------------
// UI•`‰æˆ—
// ---------------------------------------------------------
void EnemyManager::DrawUI()
{
	// ‘S‚Ä‚Ì“G‚ÌUI‚ð•`‰æ
	for (auto& enemy : enemies_) {
		enemy->DrawUI();
	}
}

// ---------------------------------------------------------
// ƒfƒoƒbƒO•\Ž¦
// ---------------------------------------------------------
void EnemyManager::Debug()
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
