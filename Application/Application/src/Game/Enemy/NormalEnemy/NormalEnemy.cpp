#include "NormalEnemy.h"

// Engine
#include <Collider/CollisionManager.h>

// Externals
#include <ImguiWrapper.h>

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

	///
	///	�R���C�_�[����
	/// 
	
	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);

	// �R���C�_�[��o�^
	CollisionManager::GetInstance()->Register(collider_.get());
}

// ---------------------------------------------------------
// �X�V����
// ---------------------------------------------------------
void NormalEnemy::Update()
{
	// �R���C�_�[�X�V
	UpdateCollider();
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


#ifdef _DEBUG
	ImGui::Begin("enemy collider");

	ImGui::DragFloat3("translate", &objectEnemy_->transform_.translate.x, 0.01f);

	ImGui::DragFloat3("center", &collider_->center_.x);
	ImGui::DragFloat("radius", &radius_);

	ImGui::End();
#endif
}

// ---------------------------------------------------------
// �Փˎ��R�[���o�b�N
// ---------------------------------------------------------
void NormalEnemy::OnCollision(Collider* other)
{
	// �Փ˂����R���C�_�[��PlayerBullet�������ꍇ�̏���
	if (other->GetTag() == "PlayerBullet") {
		// �e�X�g�p
		objectEnemy_->transform_.scale.x = 10.0f;
	}
}

// ---------------------------------------------------------
// �R���C�_�[�X�V����
// ---------------------------------------------------------
void NormalEnemy::UpdateCollider()
{
	// ���S�_
	collider_->center_ = objectEnemy_->transform_.translate;
	// ���a
	collider_->radius_ = radius_;
}
