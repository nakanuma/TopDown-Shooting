#include "GameClearSequence.h"

// Engine
#include <ImguiWrapper.h>
#include <TimeManager.h>
#include <Camera.h>

void GameClearSequence::Initialize() {

}

void GameClearSequence::Start()
{
	phase_ = Phase::Rotate; // ゲームクリア演出の開始
	timer_ = 0.0f;

	// 演出開始時のカメラ情報を保存
	savedCameraPos_ = Camera::GetCurrent()->transform_.translate_;
	savedCameraRot_ = Camera::GetCurrent()->transform_.rotate_;
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
		if (timer_ > 1.0f) {
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
	Camera::GetCurrent()->transform_.rotate_.y += 0.001f; // カメラ更新テスト

	// 一定時間経過で次のフェーズへ
	if (timer_ > 2.0f) {
		timer_ = 0.0f;
		phase_ = Phase::Explode;

		// カメラの位置と回転を元に戻す
		Camera::GetCurrent()->transform_.translate_ = savedCameraPos_;
		Camera::GetCurrent()->transform_.rotate_ = savedCameraRot_;
	}
}