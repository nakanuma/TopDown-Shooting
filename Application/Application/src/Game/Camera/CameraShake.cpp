#include "CameraShake.h"

// C++
#include <random>

// Engine
#include <TimeManager.h>
#include <RandomGenerator.h>

CameraShake* CameraShake::GetInstance() {
	static CameraShake instance;
	return &instance;
}

void CameraShake::Update() {
	if (isShaking_) {
		elapsedTime_ += TimeManager::GetInstance()->GetDeltaTime();
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

void CameraShake::StartShake(float duration, float intensity) {
	duration_ = duration;   // 継続時間セット
	intensity_ = intensity; // 強度セット
	elapsedTime_ = 0.0f;    // 経過時間リセット
	isShaking_ = true;      // シェイク開始
}

void CameraShake::ApplyShake() {
	// 残り時間に基づいて強度を線形に減少させる
	float remainingTime = duration_ - elapsedTime_; // 残り時間
	float currentIntensity = intensity_ * (remainingTime / duration_);

	// 現在の強度に基づいてオフセットをランダムに設定
	Float3 dis = RandomGenerator::GetInstance()->RandomValue(
		{-currentIntensity, -currentIntensity, -currentIntensity},
		{currentIntensity, currentIntensity, currentIntensity}
	);
	offset_ = Float3(dis.x, dis.y, dis.z);
}
