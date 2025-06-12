#pragma once

// Engine
#include <Engine/Math/MyMath.h>

/// <summary>
/// �J�����V�F�C�N
/// </summary>
class CameraShake {
public:
	/// <summary>
	/// �V���O���g���C���X�^���X�̎擾
	/// </summary>
	static CameraShake* GetInstance();

	/// <summary>
	/// �X�V����
	/// </summary>
	void Update();

	/// <summary>
	/// �V�F�C�N�J�n
	/// </summary>
	void StartShake(float duration, float intensity);

	/// <summary>
	/// �V�F�C�N�I�t�Z�b�g�擾
	/// </summary>
	/// <returns></returns>
	const Float3& GetOffset() const { return offset_; }

private:
	/// <summary>
	/// �V�F�C�N�K�p
	/// </summary>
	void ApplyShake();

	float duration_ = 0.0f;    // �p������
	float intensity_ = 0.0f;   // ���x
	float elapsedTime_ = 0.0f; // �o�ߎ���
	const float kDeltaTime = 1.0f / 60.0f;
	bool isShaking_ = false; // �V�F�C�N���H
	Float3 offset_;          // �J�����ɉ�����I�t�Z�b�g
};