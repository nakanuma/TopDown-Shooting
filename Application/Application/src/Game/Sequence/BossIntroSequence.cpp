#include "BossIntroSequence.h"

// Engine
#include <TimeManager.h>
#include <Camera.h>

// Application
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Player/Player.h>

void BossIntroSequence::Initialize(Cygnus::SpriteCommon* spriteCommon, Player* player) { player_ = player; }

void BossIntroSequence::Start() {
	phase_ = Phase::Fade1; // 最初のフェーズを設定
}

void BossIntroSequence::Update() {
	// タイマー更新
	timer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	switch (phase_) {
	case Phase::Fade1:
		if (!isFadeStarted_) {
			isFadeStarted_ = true; // フェードしていることを記録

			// フェードアウト（暗転を開始）
			FadeTransition::GetInstance()->StartFadeOut(kFadeDuration, [this]() {
				/* 暗転した瞬間の処理（カメラ切り替え）*/

				// プレイヤーを正面に向かせる
				Cygnus::Float3 playerRot = player_->GetRotate();
				playerRot.y = 0.0f;
				player_->SetRotate(playerRot);
	
				// プレイヤーの正面斜め前からの視点に変更
				Cygnus::Float3 playerPos = player_->GetTranslate();

				float distance = 30.0f;
				float sideOffset = 4.0f;
				float height = 2.0f;

				float angle = playerRot.y;
				Cygnus::Float3 cameraPos;
				cameraPos.x = playerPos.x + std::sinf(angle) * distance + std::cosf(angle) * sideOffset;
				cameraPos.y = playerPos.y + height;
				cameraPos.z = playerPos.z + distance;

				float rotY = Cygnus::PIf + 0.2f;
				float rotX = 0.15f;

				Cygnus::Camera::GetCurrent()->transform_.translate_ = cameraPos;
				Cygnus::Camera::GetCurrent()->transform_.rotate_ = { rotX, rotY, 0.0f };

				// 暗転が終わったのでフェードインを開始
				FadeTransition::GetInstance()->StartFadeIn(kFadeDuration, 0.0f);
			});
		}

		// フェードが終了したので次のフェーズへ移行
		if (isFadeStarted_ && FadeTransition::GetInstance()->IsFinished()) {
			phase_ = Phase::FrontCamera;
			isFadeStarted_ = false; // この後使い回すのでフラグをリセットしておく

			timer_ = 0.0f; // タイマーリセット
		}

		break;

	case Phase::FrontCamera:
		// プレイヤー歩行
		if (timer_ < kWalkDuration) {
			float speed = player_->GetMoveSpeed();
			float dt = Cygnus::TimeManager::GetInstance()->GetDeltaTime();

			Cygnus::Float3 velocity = {0.0f, 0.0f, speed * dt};
			player_->SetVelovity(velocity);
		} else {
			// 次のフェーズへ（カメラ切り替え）

			// プレイヤーの正面斜め前からの視点に変更
			Cygnus::Float3 playerPos = player_->GetTranslate();

			float distance = 30.0f;
			float sideOffset = -3.0f;
			float height = 2.0f;

			Cygnus::Float3 cameraPos;
			cameraPos.x = playerPos.x - sideOffset;
			cameraPos.y = playerPos.y + height;
			cameraPos.z = playerPos.z - distance;

			float rotY = -0.1f;
			float rotX = 0.1f;

			Cygnus::Camera::GetCurrent()->transform_.translate_ = cameraPos;
			Cygnus::Camera::GetCurrent()->transform_.rotate_ = {rotX, rotY, 0.0f};

			// 状態の更新
			player_->SetVelovity({0.0f, 0.0f, 0.0f}); // プレイヤー停止
			phase_ = Phase::RearCamera; // 次フェーズへ移行

			timer_ = 0.0f; // タイマーリセット
		}

		break;

	case Phase::RearCamera:
		break;

	case Phase::Fade2:
		break;

	case Phase::Finish:
		break;
	}
}

void BossIntroSequence::DrawUI() {}