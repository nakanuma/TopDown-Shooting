#include "CameraShake.h"

// C++
#include <random>

// ---------------------------------------------------------
// シングルトンインスタンスの取得
// ---------------------------------------------------------
CameraShake* CameraShake::GetInstance() 
{ 
	static CameraShake instance;
	return &instance;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void CameraShake::Update() 
{
	if (isShaking_) {
		elapsedTime_ += kDeltaTime;
		// シェイク終了
		if (elapsedTime_ >= duration_) {
			isShaking_ = false;
			offset_ = Float3(0.0f, 0.0f, 0.0f); // オフセットのリセット
			// シェイク中
		} else {
			ApplyShake();
		}
	}
}

// ---------------------------------------------------------
// シェイク開始
// ---------------------------------------------------------
void CameraShake::StartShake(float duration, float intensity) 
{
	duration_ = duration; // 継続時間セット
	intensity_ = intensity; // 強度セット
	elapsedTime_ = 0.0f; // 経過時間リセット
	isShaking_ = true; // シェイク開始
}

// ---------------------------------------------------------
// シェイク適用
// ---------------------------------------------------------
void CameraShake::ApplyShake() 
{
	// 残り時間に基づいて強度を線形に減少させる
	float remainingTime = duration_ - elapsedTime_; // 残り時間
	float currentIntensity = intensity_ * (remainingTime / duration_);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-currentIntensity, currentIntensity);
	offset_ = Float3(dis(gen), dis(gen), dis(gen));
}
