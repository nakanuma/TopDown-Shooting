#include "EnemyManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>

// ---------------------------------------------------------
// ����������
// ---------------------------------------------------------
void EnemyManager::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	�e���f���ǂݍ���
	/// 
	
	// �ʏ�G
	modelNormalEnemy_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelNormalEnemy_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// �ǉ�

}

// ---------------------------------------------------------
// �X�V����
// ---------------------------------------------------------
void EnemyManager::Update()
{
	// �S�Ă̓G���X�V
	for (auto& enemy : enemies_) {
		enemy->Update();
	}

	// �G�̍폜����
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
// �`�揈��
// ---------------------------------------------------------
void EnemyManager::Draw()
{
	// �S�Ă̓G��`��
	for (auto& enemy : enemies_) {
		enemy->Draw();
	}


#ifdef _DEBUG
	// �f�o�b�O�\��
	Debug();
#endif // _DEBUG
}

// ---------------------------------------------------------
// UI�`�揈��
// ---------------------------------------------------------
void EnemyManager::DrawUI()
{
	// �S�Ă̓G��UI��`��
	for (auto& enemy : enemies_) {
		enemy->DrawUI();
	}
}

// ---------------------------------------------------------
// �f�o�b�O�\��
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
