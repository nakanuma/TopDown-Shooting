#include "EnemySpawnManager.h"

// Externals
#include <ImguiWrapper.h>

// Application
#include <src/Game/Enemy/NormalEnemy/NormalEnemy.h>

// ---------------------------------------------------------
// ����������
// ---------------------------------------------------------
void EnemySpawnManager::Initialize()
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
void EnemySpawnManager::Update()
{
	// �S�Ă̓G���X�V
	for (auto& enemy : enemies_) {
		enemy->Update();
	}
}

// ---------------------------------------------------------
// �`�揈��
// ---------------------------------------------------------
void EnemySpawnManager::Draw()
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
// �f�o�b�O�\��
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
