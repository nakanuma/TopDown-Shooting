#include "NormalEnemy.h"

// ---------------------------------------------------------
// ����������
// ---------------------------------------------------------
void NormalEnemy::Initialize(const Float3& position, ModelManager::ModelData* model)
{
	///
	/// �I�u�W�F�N�g����
	///
	
	objectEnemy_ = std::make_unique<Object3D>();
	objectEnemy_->model_ = model;
	objectEnemy_->transform_.translate = position;
}

// ---------------------------------------------------------
// �X�V����
// ---------------------------------------------------------
void NormalEnemy::Update()
{
	// �I�u�W�F�N�g�X�V
	objectEnemy_->UpdateMatrix();
}

// ---------------------------------------------------------
// �`�揈��
// ---------------------------------------------------------
void NormalEnemy::Draw()
{
	// �I�u�W�F�N�g�`��
	objectEnemy_->Draw();
}
