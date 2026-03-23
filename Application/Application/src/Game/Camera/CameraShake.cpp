#include "CameraShake.h"

// C++
#include <random>

// Engine
#include <RandomGenerator.h>
#include <TimeManager.h>

CameraShake* CameraShake::GetInstance() {
	static CameraShake instance;
	return &instance;
}

void CameraShake::Update() {
	if (isShaking_) {
		elapsedTime_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		// シェイク終了
		if (elapsedTime_ >= duration_) {
			isShaking_ = false;
			offset_ = Cygnus::Float3(0.0f, 0.0f, 0.0f); // オフセットのリセット
			currentPriority_ = ShakePriority::Low;	// 終了時にリセット
			// シェイク中
		} else {
			ApplyShake();
		}
	}
}

void CameraShake::StartShake(float duration, float intensity, ShakePriority priority) {
	// 発生中の揺れの優先度が高いならスキップ
	if (isShaking_ && ShakePriority::Low < currentPriority_) return;

	duration_ = duration;			// 継続時間セット
	intensity_ = intensity;			// 強度セット
	elapsedTime_ = 0.0f;			// 経過時間リセット
	isShaking_ = true;				// シェイク開始
	mode_ = ShakeMode::Random;		// モード設定
	currentPriority_ = priority;	// 優先度
}

void CameraShake::StartShootShake() {
	// 発生中の揺れの優先度が高いならスキップ
	if(isShaking_ && ShakePriority::Low < currentPriority_) return;

	duration_ = shootShakeDuration;			// 継続時間セット
	intensity_ = shootShakeIntensity;		// 強度セット
	elapsedTime_ = 0.0f;					// 経過時間リセット
	isShaking_ = true;						// シェイク開始
	mode_ = ShakeMode::SineY;				// モード設定
	currentPriority_ = ShakePriority::Low;	// 優先度（低）
}

void CameraShake::ApplyShake() {
	// 進行度
	float progress = elapsedTime_ / duration_;

	// 通常のランダム揺れ
	if(mode_ == ShakeMode::Random) {
		// 残り時間に基づいて強度を線形に減少させる
		float currentIntensity = intensity_ * (1.0f - progress);
		// 現在の強度に基づいてオフセットをランダムに設定
		Cygnus::Float3 dis = Cygnus::RandomGenerator::GetInstance()->RandomValue(
			{ -currentIntensity, -currentIntensity, -currentIntensity }, 
			{ currentIntensity, currentIntensity, currentIntensity }
		);
		offset_ = dis;
	
	// 上下揺れ（プレイヤー射撃時用）
	} else if (mode_ == ShakeMode::SineY) {
		// sin波で揺れを行う
		float sineValue = std::sinf(Cygnus::PIf * progress);
		offset_ = {0.0f, (sineValue * intensity_), 0.0f};
	}
}
