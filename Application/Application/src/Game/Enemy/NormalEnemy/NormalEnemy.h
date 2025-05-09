#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// �ʏ�G
/// </summary>
class NormalEnemy : public Enemy, public ICollisionCallback
{
public:
	/// <summary>
	/// ����������
	/// </summary>
	void Initialize(const Float3& position, ModelManager::ModelData* model) override;

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update() override;
	
	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UI�`�揈��
	/// </summary>
	void DrawUI() override;

	/// <summary>
	/// �Փˎ��R�[���o�b�N
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// �j�������ۂɌĂ΂��֐�
	/// </summary>
	void OnDestroy() override;

private:
	// ---------------------------------------------------------
	// ��������
	// ---------------------------------------------------------

	/// <summary>
	/// �R���C�_�[�X�V����
	/// </summary>
	void UpdateCollider();

private:
	// ---------------------------------------------------------
	// �p�����[�^�[
	// ---------------------------------------------------------

	float radius_ = 1.0f;
};

