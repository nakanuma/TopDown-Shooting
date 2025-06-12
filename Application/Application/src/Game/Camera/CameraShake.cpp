#include "CameraShake.h"

// C++
#include <random>

// ---------------------------------------------------------
// �V���O���g���C���X�^���X�̎擾
// ---------------------------------------------------------
CameraShake* CameraShake::GetInstance() 
{ 
	static CameraShake instance;
	return &instance;
}

// ---------------------------------------------------------
// �X�V����
// ---------------------------------------------------------
void CameraShake::Update() 
{
	if (isShaking_) {
		elapsedTime_ += kDeltaTime;
		// �V�F�C�N�I��
		if (elapsedTime_ >= duration_) {
			isShaking_ = false;
			offset_ = Float3(0.0f, 0.0f, 0.0f); // �I�t�Z�b�g�̃��Z�b�g
			// �V�F�C�N��
		} else {
			ApplyShake();
		}
	}
}

// ---------------------------------------------------------
// �V�F�C�N�J�n
// ---------------------------------------------------------
void CameraShake::StartShake(float duration, float intensity) 
{
	duration_ = duration; // �p�����ԃZ�b�g
	intensity_ = intensity; // ���x�Z�b�g
	elapsedTime_ = 0.0f; // �o�ߎ��ԃ��Z�b�g
	isShaking_ = true; // �V�F�C�N�J�n
}

// ---------------------------------------------------------
// �V�F�C�N�K�p
// ---------------------------------------------------------
void CameraShake::ApplyShake() 
{
	// �c�莞�ԂɊ�Â��ċ��x����`�Ɍ���������
	float remainingTime = duration_ - elapsedTime_; // �c�莞��
	float currentIntensity = intensity_ * (remainingTime / duration_);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-currentIntensity, currentIntensity);
	offset_ = Float3(dis(gen), dis(gen), dis(gen));
}
