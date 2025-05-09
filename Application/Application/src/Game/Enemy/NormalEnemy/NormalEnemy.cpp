#include "NormalEnemy.h"

// C++
#include <algorithm>

// Engine
#include <DirectXBase.h>
#include <Collider/CollisionManager.h>
#include <Camera.h>

// Application
#include <src/Game/Utility/Utility.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// ����������
// ---------------------------------------------------------
void NormalEnemy::Initialize(const Float3& position, ModelManager::ModelData* model)
{
	///
	///	��Ջ@�\����
	/// 

	DirectXBase* dxBase = DirectXBase::GetInstance();

	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

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

	///
	///	�X�v���C�g����
	/// 

	// HP�o�[�i��i�j
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize({ 100.0f, 25.0f });
	spriteHPBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// HP�o�[�i�O�i�j
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize({ 100.0f, 25.0f });
	spriteHPForeground_->SetColor({ 0.0f, 1.0f, 0.5f, 1.0f });

	///
	///	�p�����[�^�[�ݒ�
	/// 

	// HP�̐ݒ�
	currentHP_ = 5;
	maxHP_ = currentHP_; // �ő�HP�ɂ͐ݒ肵������HP��ݒ�i�SEnemy�N���X���ʁj
}

// ---------------------------------------------------------
// �X�V����
// ---------------------------------------------------------
void NormalEnemy::Update()
{
	///
	/// �R���C�_�[�X�V����
	///

	UpdateCollider();

	///
	/// �I�u�W�F�N�g�X�V����
	/// 

	objectEnemy_->UpdateMatrix();

	///
	///	�X�v���C�g�X�V����
	/// 

	// HP�o�[�i��i�j�X�V
	spriteHPBackground_->Update();
	// HP�o�[�i�O�i�j�X�V
	spriteHPForeground_->Update();
}

// ---------------------------------------------------------
// �`�揈��
// ---------------------------------------------------------
void NormalEnemy::Draw()
{
	// �I�u�W�F�N�g�`��
	objectEnemy_->Draw();
}

// ---------------------------------------------------------
// UI�`�揈��
// ---------------------------------------------------------
void NormalEnemy::DrawUI()
{
	// �I�u�W�F�N�g�̃��[���h���W->�X�N���[�����W�ɕϊ�
	Float3 screenPosition = Utility::WorldToScreen(objectEnemy_->transform_.translate);
	// ��ɂ��炷���̃I�t�Z�b�g
	float offset = 60.0f;

	// HP����
	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	///
	/// HP�o�[�i��i�j�`��
	/// 

	// �X�N���[�����W���Z�b�g
	spriteHPBackground_->SetPosition({
		screenPosition.x - kHPBarSize.x / 2.0f, // HP�o�[�����S�ɂȂ�悤�ɐݒ�,
		screenPosition.y - offset // �I�t�Z�b�g����ɂ��炷
		});
	spriteHPBackground_->Draw();

	///
	///	HP�o�[�i�O�i�j�`��
	/// 
	
	// ����HP�ɉ����ăT�C�Y�ύX
	Float2 hpBarForegroundSize = { kHPBarSize.x * hpRatio, kHPBarSize.y };
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	// �X�N���[�����W���Z�b�g
	spriteHPForeground_->SetPosition({
		screenPosition.x - kHPBarSize.x / 2.0f, // HP�o�[�����S�ɂȂ�悤�ɐݒ�
		screenPosition.y - offset // �I�t�Z�b�g����ɂ��炷
		});

	spriteHPForeground_->Draw();

}

// ---------------------------------------------------------
// �Փˎ��R�[���o�b�N
// ---------------------------------------------------------
void NormalEnemy::OnCollision(Collider* other)
{
	// �Փ˂����R���C�_�[��PlayerBullet�������ꍇ�̏���
	if (other->GetTag() == "PlayerBullet") {
		// HP�����炷
		currentHP_ -= 1;

		// HP��0�ɂȂ����G�����S������
		if (currentHP_ <= 0) {
			isDead_ = true;
		}
	}
}

// ---------------------------------------------------------
// �j�������ۂɌĂ΂��֐�
// ---------------------------------------------------------
void NormalEnemy::OnDestroy()
{
	// �R���W�����}�l�[�W���[����R���C�_�[�̓o�^������
	CollisionManager::GetInstance()->Unregister(collider_.get());
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
