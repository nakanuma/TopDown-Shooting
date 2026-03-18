#include "BossIntroSequence.h"

// C++
#include <algorithm>

// Engine
#include <TimeManager.h>
#include <Camera.h>
#include <ImguiWrapper.h>
#include <ParticleEffect/ParticleEffectManager.h>
#include <Easing.h>

// Application
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Scene/GamePlayScene.h>

void BossIntroSequence::Initialize(Cygnus::SpriteCommon* spriteCommon, Player* player, GamePlayScene* scene) { 
	player_ = player; 
	scene_ = scene;

	// オブジェクト生成
	objectCrumblingWall_ = std::make_unique<Cygnus::Object3D>();
	objectCrumblingWall_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("CrumblingWall");
	objectCrumblingWall_->transform_.scale_ = {1.0f, 1.5f, 1.0f};
	objectCrumblingWall_->transform_.translate_ = { 156.0f, 3.5f, 68.0f }; // 初期位置

	// SPACEでスキップのUI生成
	spaceSkip_ = std::make_unique<SpaceSkip>();
	spaceSkip_->Initialize();
}

void BossIntroSequence::Start() {
	phase_ = Phase::Fade1; // 最初のフェーズを設定
}

void BossIntroSequence::Update() {
	// SPACEホールドが完了したらスキップ
	if (spaceSkip_->IsSkipped()) {
		Skip();
	}

	// オブジェクト更新
	objectCrumblingWall_->UpdateMatrix();
	objectCrumblingWall_->UpdateShadowMatrix();

	// SPACEスキップUI更新
	if (phase_ != Phase::Fade1 && phase_ != Phase::Fade2) { // フェードのフェーズでは更新しない
		spaceSkip_->Update();
	}

	// タイマー更新
	timer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	switch (phase_) {
	case Phase::Fade1: {
		if (!isFadeStarted_) {
			isFadeStarted_ = true; // フェードしていることを記録

			// フェードアウト（暗転を開始）
			FadeTransition::GetInstance()->StartFadeOut(kFadeDuration, [this]() {
				/* 暗転した瞬間の処理（カメラ切り替え）*/

				// プレイヤーを所定位置へ移動（ムービー位置を合わせるため）
				player_->SetTranslate({156.0f, 2.0f, 40.0f});
				player_->SetIsMoving(false); // 歩行アニメーション停止

				// プレイヤー斜め後ろの視点へカメラを設定
				Cygnus::Float3 playerPos = player_->GetTranslate();

				float distance = 20.0f;
				float sideOffset = -10.0f;
				float height = 4.0;

				Cygnus::Float3 cameraPos;
				cameraPos.x = playerPos.x - sideOffset;
				cameraPos.y = playerPos.y + height;
				cameraPos.z = playerPos.z - distance;

				float rotY = -0.3f;
				float rotX = 0.1f;

				Cygnus::Camera::GetCurrent()->transform_.translate_ = cameraPos;
				Cygnus::Camera::GetCurrent()->transform_.rotate_ = {rotX, rotY, 0.0f};

				// ボスの方向を向く
				Cygnus::Float3 newPlayerPos = player_->GetTranslate();
				Cygnus::Float3 bossPos = scene_->GetEnemyManager()->GetBoss()->GetTranslate();
				float dx = bossPos.x - newPlayerPos.x;
				float dz = bossPos.z - newPlayerPos.z;
				float targetRot = std::atan2f(dx, dz);
				player_->SetRotate({0.0f, targetRot, 0.0f});

				shakeBaseCameraPos_ = Cygnus::Camera::GetCurrent()->transform_.translate_; // 現在のカメラ位置を保存

				// 暗転が終わったのでフェードインを開始
				FadeTransition::GetInstance()->StartFadeIn(kFadeDuration, 0.0f);
			});
		}

		// フェードが終了したので状態の更新
		if (isFadeStarted_ && FadeTransition::GetInstance()->IsFinished()) {
			phase_ = Phase::RearCamera; // 次のフェーズへ移行
			isFadeStarted_ = false;     // この後使い回すのでフラグをリセットしておく
			timer_ = 0.0f;              // タイマーリセット
		}
	}

		break;
	case Phase::RearCamera: {
		// プレイヤーの歩行を止める
		player_->SetVelovity({0.0f, 0.0f, 0.0f});

		// ボスの方向を向く
		Cygnus::Float3 playerPos = player_->GetTranslate();
		Cygnus::Float3 bossPos = scene_->GetEnemyManager()->GetBoss()->GetTranslate();
		float dx = bossPos.x - playerPos.x;
		float dz = bossPos.z - playerPos.z;
		float targetRot = std::atan2f(dx, dz);
		player_->SetRotate({0.0f, targetRot, 0.0f});

		// 状態の更新
		phase_ = Phase::Shaking; // 次のフェーズへ移行
		timer_ = 0.0f;
	}

		break;

	case Phase::Shaking:
		// 1回目のシェイク
		if(!isShaked1_ && timer_ >= kShakeTime1) {
			CameraShake::GetInstance()->StartShake(kShakeDuration1, kShakeIntensity1);
			isShaked1_ = true;
		}

		// 2回目のシェイク
		if (!isShaked2_ && timer_ >= kShakeTime2) {
			CameraShake::GetInstance()->StartShake(kShakeDuration2, kShakeIntensity2);
			isShaked2_ = true;
		}

		// 3回目のシェイク
		if (!isShaked3_ && timer_ >= kShakeTime3) {
			CameraShake::GetInstance()->StartShake(kShakeDuration3, kShakeIntensity3);
			isShaked3_ = true;

			Cygnus::ParticleEffectManager::GetInstance()->Emit("wallCollapse", objectCrumblingWall_->transform_.translate_, 200); // 壁崩壊パーティクル発生
			isWallCrumbled_ = true; // 壁が崩壊したことを記録
		}

		// 現在カメラにシェイクを適用
		Cygnus::Camera::GetCurrent()->transform_.translate_ = shakeBaseCameraPos_ + CameraShake::GetInstance()->GetOffset();

		// 時間を過ぎたら終了
		if (timer_ > kShakePhaseEndDuration) {
			phase_ = Phase::ShootDownMissile; // 次のフェーズへ移行
			timer_ = 0.0f;             // タイマーリセット
		}

		break;

	case Phase::ShootDownMissile:
		// ボスがミサイルを撃つ
		if (!hasLaunchMissile_) {
			scene_->GetEnemyManager()->GetBoss()->LaunchMissile();
			hasLaunchMissile_ = true;
		}

		// プレイヤーが射撃する
		if (!hasPlayerShooted_ && timer_ > kPlayerShootDuration) {
			player_->Fire();
			hasPlayerShooted_ = true;
		}

		// 時間を過ぎたら終了
		if (timer_ > kShootDownMissilePhaseEndDuration) {
			phase_ = Phase::Fade2; // 次のフェーズへ移行
			timer_ = 0.0f; // タイマーリセット
		}

		break;

	case Phase::Fade2:
		if (!isFadeStarted_) {
			isFadeStarted_ = true; // フェードしていることを記録

			// フェードアウト（暗転を開始）
			FadeTransition::GetInstance()->StartFadeOut(kFadeDuration, [this]() {
				/* 暗転した瞬間の処理（トップダウンカメラへ切り替え）*/
				Cygnus::Camera::GetCurrent()->transform_.translate_ = kTopdownCameraPos;
				Cygnus::Camera::GetCurrent()->transform_.rotate_ = kTopdownCameraRot;

				// 暗転が終わったのでフェードインを開始
				FadeTransition::GetInstance()->StartFadeIn(kFadeDuration, 0.0f);
				});
		}
		// フェードが終了したので状態の更新
		if (isFadeStarted_ && FadeTransition::GetInstance()->IsFinished()) {
			scene_->GetEnemyManager()->GetBoss()->SetActive(true); // ボスを有効化状態にする

			phase_ = Phase::Finish; // フェーズ終了
		}

		break;

	case Phase::Finish:
		break;
	}
}

void BossIntroSequence::Draw() {
	// 壁が崩れているなら描画スキップ
	if(isWallCrumbled_) return;

	objectCrumblingWall_->Draw();
}

void BossIntroSequence::DrawShadow() {
	// 壁が崩れているなら描画スキップ
	if(isWallCrumbled_) return;

	objectCrumblingWall_->DrawShadow();
}

void BossIntroSequence::DrawUI() {
	// SPACEでスキップUI描画
	if (phase_ != Phase::Fade1 && phase_ != Phase::Fade2) { // フェードのフェーズでは描画しない
		spaceSkip_->Draw();
	}
}

void BossIntroSequence::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("BossIntroSequence");

	if (ImGui::Button("Skip")) {
		Skip();
	}

	ImGui::Text("Timer : %.2f", timer_);

	// フェーズ名の表示
	const char* phaseStr = "";
	switch (phase_) {
	case BossIntroSequence::Phase::None:
		phaseStr = "None";
		break;
	case BossIntroSequence::Phase::Fade1:
		phaseStr = "Fade1";
		break;
	case BossIntroSequence::Phase::RearCamera:
		phaseStr = "RearCamera";
		break;
	case BossIntroSequence::Phase::Shaking:
		phaseStr = "Shaking";
		break;
	case BossIntroSequence::Phase::CameraPan:
		phaseStr = "CameraPan";
		break;
	case BossIntroSequence::Phase::ShootDownMissile:
		phaseStr = "ShootDownMissile";
		break;
	case BossIntroSequence::Phase::Fade2:
		phaseStr = "Fade2";
		break;
	case BossIntroSequence::Phase::Finish:
		phaseStr = "Finish";
		break;
	}
	ImGui::Text("Current Phase : %s", phaseStr);
	ImGui::End();
#endif
}

void BossIntroSequence::Skip() {
	if (phase_ != Phase::Finish) {
		// カメラをトップダウンに設定
		Cygnus::Camera::GetCurrent()->transform_.translate_ = kTopdownCameraPos;
		Cygnus::Camera::GetCurrent()->transform_.rotate_ = kTopdownCameraRot;

		// 壁崩壊済みフラグを立てる
		isWallCrumbled_ = true;

		// ボスを有効化する
		scene_->GetEnemyManager()->GetBoss()->SetActive(true);

		// 終了フェーズにする
		phase_ = Phase::Finish;
		timer_ = 0.0f;

		return;
	}
}
