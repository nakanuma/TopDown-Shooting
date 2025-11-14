#include "GameClearSequence.h"

// Engine
#include <ImguiWrapper.h>
#include <TimeManager.h>
#include <Camera.h>
#include <Easing.h>

// Application
#include <src/Game/Camera/CameraShake.h>

void GameClearSequence::Initialize() {

}

void GameClearSequence::Start()
{
	phase_ = Phase::Rotate; // ゲームクリア演出の開始
	timer_ = 0.0f;

	// 演出開始時のカメラ情報を保存
	savedCameraPos_ = Camera::GetCurrent()->transform_.translate_;
	savedCameraRot_ = Camera::GetCurrent()->transform_.rotate_;

	// カメラシェイクを開始（カメラ回転 + 爆発フェーズまで）
	CameraShake::GetInstance()->StartShake(kCameraRotateDuration + kExplodeDuration, 0.4f);
}

void GameClearSequence::Update() {
	// ゲームクリア演出が行われていない間はスキップ
	if (!IsActive()) return;

	// タイマー更新
	if (!IsFinished()) {
		timer_ += TimeManager::GetInstance()->GetDeltaTime();
	}

	// フェーズ毎の更新処理
	switch (phase_) {
	case Phase::Rotate:
		// カメラ回転時の更新処理
		UpdateRotate();
		break;
	case Phase::Explode:
		// 一定時間経過で次のフェーズへ
		if (timer_ > kExplodeDuration) {
			timer_ = 0.0f;
			phase_ = Phase::ClearText;
		}

		break;
	case Phase::ClearText:
		// 一定時間経過で次のフェーズへ
		if (timer_ > 1.0f) {
			timer_ = 0.0f;
			phase_ = Phase::Finish;
		}

		break;
	case Phase::Finish:
		break;
	}
}

void GameClearSequence::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("GameClearSeqence");

	ImGui::Text("Timer : %.2f", timer_);

	// フェーズ名の表示
	const char* phaseStr = "";
	switch (phase_)
	{
	case GameClearSequence::Phase::None: phaseStr = "None"; break;
	case GameClearSequence::Phase::Rotate: phaseStr = "Rotate"; break;
	case GameClearSequence::Phase::Explode: phaseStr = "Explode"; break;
	case GameClearSequence::Phase::ClearText: phaseStr = "ClearText"; break;
	case GameClearSequence::Phase::Finish: phaseStr = "Finish"; break;
	}
	ImGui::Text("Current Phase : %s", phaseStr);

	ImGui::End();
#endif
}

void GameClearSequence::UpdateRotate(){
	// 回転の進行度
	float t = std::clamp(timer_ / kCameraRotateDuration, 0.0f, 1.0f);

	// ボスの位置を取得
	Float3 bossPos = boss_->GetTranslate();
	// ボスのY軸回転を取得
	float bossForwardAngle = boss_->GetRotate().y;

	// 開始角度（ボスの正面 + 45度）
	float startAngle = bossForwardAngle + PIf / 4.0f;
	// 終了角度（ボスの正面 - 45度）
	float endAngle = bossForwardAngle - PIf / 4.0f;

	// 現在の角度を補間
	float currentAngle = Easing::Lerp(startAngle, endAngle, Easing::EaseOutSine(t));

	// カメラの高さを補間
	float currentHeight = Easing::Lerp(kCameraHeightStart, kCameraHeightEnd, t);

	// ボス中心の円周上のカメラ位置を計算
	Float3 cameraPos = {
		bossPos.x + sinf(currentAngle) * kCameraDistance,
		bossPos.y + bossPos.y + currentHeight,
		bossPos.z + cosf(currentAngle) * kCameraDistance
	};

	// カメラをボスの方向に向ける
	Float3 direction = bossPos - cameraPos;
	direction = Float3::Normalize(direction);

	// カメラの回転を計算
	Float3 cameraRot = {
		asinf(-direction.y),
		atan2f(direction.x, direction.z),
		kCameraRotZOffset
	};

	// カメラに適用
	Camera::GetCurrent()->transform_.translate_ = cameraPos + CameraShake::GetInstance()->GetOffset(); // カメラシェイクも加算
	Camera::GetCurrent()->transform_.rotate_ = cameraRot;

	// 回転終了で次のフェーズへ
	if (timer_ > kCameraRotateDuration) {
		timer_ = 0.0f;
		phase_ = Phase::Explode;

		// カメラの位置と回転を元に戻す
		Camera::GetCurrent()->transform_.translate_ = savedCameraPos_;
		Camera::GetCurrent()->transform_.rotate_ = savedCameraRot_;
	}
}