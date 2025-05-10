#include "NormalEnemy.h"

// C++
#include <algorithm>

// Engine
#include <DirectXBase.h>
#include <Collider/CollisionManager.h>
#include <Camera.h>

// Application
#include <src/Game/Utility/Utility.h>
#include <src/Game/Bullet/Base/Bullet.h>

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
	objectEnemy_->transform_.scale = { 6.0f, 1.0f, 1.0f };
	objectEnemy_->transform_.rotate = { 0.0f, 0.7f, 0.0f };

	///
	///	�R���C�_�[����
	/// 

	/*collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);*/

	/*collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);*/

	collider_ = std::make_unique<OBBCollider>();
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
	currentHP_ = 10;
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
		// PlayerBullet��damage���擾
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HP�����炷
		currentHP_ -= damage;

		// HP��0�ɂȂ����G�����S������
		if (currentHP_ <= 0) {
			isDead_ = true;
		}
	}
}

// ---------------------------------------------------------
// �R���C�_�[�X�V����
// ---------------------------------------------------------
void NormalEnemy::UpdateCollider()
{
	//if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
	//	// ���S�_
	//	sphere->center_ = objectEnemy_->transform_.translate;
	//	// ���a
	//	sphere->radius_ = radius_;
	//}

	//if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
	//	Float3 center = objectEnemy_->transform_.translate;
	//	Float3 size = objectEnemy_->transform_.scale;

	//	// min
	//	aabb->min_ = center - size;
	//	aabb->max_ = center + size;
	//}

	if (OBBCollider* obb = dynamic_cast<OBBCollider*>(collider_.get())) {
		// ���S
		obb->center_ = objectEnemy_->transform_.translate;

		// ���T�C�Y
		obb->halfSize_ = objectEnemy_->transform_.scale;

		// ��]�s��̎擾
		Matrix rotMat = Matrix::Rotation(objectEnemy_->transform_.rotate);

		// ���[�J��������]�s�񂩂���o���Đݒ�
		obb->axes_[0] = Float3::Normalize({ rotMat.r[0][0], rotMat.r[1][0], rotMat.r[2][0] });
		obb->axes_[1] = Float3::Normalize({ rotMat.r[0][1], rotMat.r[1][1], rotMat.r[2][1] });
		obb->axes_[2] = Float3::Normalize({ rotMat.r[0][2], rotMat.r[1][2], rotMat.r[2][2] });
	}
}
