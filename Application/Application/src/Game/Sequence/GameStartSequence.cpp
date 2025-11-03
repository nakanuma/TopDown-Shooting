#include "GameStartSequence.h"

// Engine
#include <ImguiWrapper.h>
#include <TimeManager.h>
#include <Camera.h>
#include <Easing.h>
#include <ParticleEffect/ParticleEffectManager.h>
#include <RandomGenerator.h>

// Application
#include <src/Game/Camera/CameraShake.h>

void GameStartSequence::Initialize(SpriteCommon* spriteCommon) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 初期カメラを設定
	Camera::GetCurrent()->transform.translate = initCameraPos_;
	Camera::GetCurrent()->transform.rotate = initCameraRot_;

	// オブジェクト生成
	objectCrumblingWall_ = std::make_unique<Object3D>();
	objectCrumblingWall_->model_ = &ModelManager::GetInstance()->GetModel("CrumblingWall");
	objectCrumblingWall_->transform_.translate = { 36.0f, 2.5f, 0.0f }; // 初期位置

	objectDynamite_ = std::make_unique<Object3D>();
	objectDynamite_->model_ = &ModelManager::GetInstance()->GetModel("Dynamite");
	objectDynamite_->transform_.translate = { 36.0f, 2.5f, -1.5f }; // 初期位置

	// スプライト生成
	uint32_t textureWhite = TextureManager::Load("white.png");
	spriteTopLetterBox_ = std::make_unique<Sprite>();
	spriteTopLetterBox_->Initialize(spriteCommon, textureWhite);
	spriteTopLetterBox_->SetAnchorPoint({0.5f, 0.5f});
	spriteTopLetterBox_->SetColor({0.01f, 0.01f, 0.01f, 1.0f});
	spriteTopLetterBox_->SetSize(kLetterBoxSize);
	spriteTopLetterBox_->SetPosition(topBoxStartPos_);


	spriteBottomLetterBox_ = std::make_unique<Sprite>();
	spriteBottomLetterBox_->Initialize(spriteCommon, textureWhite);
	spriteBottomLetterBox_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteBottomLetterBox_->SetColor({ 0.01f, 0.01f, 0.01f, 1.0f });
	spriteBottomLetterBox_->SetSize(kLetterBoxSize);
	spriteBottomLetterBox_->SetPosition(bottomBoxStartPos_);

	// ダイナマイトの可視状態（初期は見えるように）
	isDynamiteVisible_ = true;
}

void GameStartSequence::Update() {
	// タイマー更新
	timer_ += TimeManager::GetInstance()->GetDeltaTime();

	// オブジェクト更新
	objectCrumblingWall_->UpdateMatrix();
	objectCrumblingWall_->UpdateShadowMatrix();

	objectDynamite_->UpdateMatrix();
	objectDynamite_->UpdateShadowMatrix();

	// スプライト更新
	spriteTopLetterBox_->Update();
	spriteBottomLetterBox_->Update();

	// フェーズ毎の更新処理
	switch (phase_) {
	case Phase::Intro:
		// 一定時間経過で次のフェーズへ
		if (timer_ > 2.0f) {
			timer_ = 0.0f;
			phase_ = Phase::Brink;
		}

		break;
	case Phase::Brink:
		// ダイナマイト点滅時処理
		UpdateBlink();

		break;
	case Phase::Explosion:
		// ダイナマイト爆発時処理
		UpdateExplosion();

		break;
	case Phase::Transition:
		// プレイ視点移行処理
		UpdateTransition();

		break;
	case Phase::Finish:
		// 終了させる
		isFinished_ = true;

		break;
	}
}

void GameStartSequence::Draw() {
	// 爆発終了後は描画スキップ
	if (isExplode_) return;

	objectCrumblingWall_->Draw();

	if (isDynamiteVisible_) {
		objectDynamite_->Draw();
	}
}

void GameStartSequence::DrawShadow() {
	// 爆発終了後は描画スキップ
	if (isExplode_) return;

	objectCrumblingWall_->DrawShadow();

	if (isDynamiteVisible_) {
		objectDynamite_->DrawShadow();
	}
}

void GameStartSequence::DrawUI() {
	spriteTopLetterBox_->Draw();
	spriteBottomLetterBox_->Draw();
}

void GameStartSequence::Debug() {
	ImGui::Begin("GameStartSequence");

	// フェーズ名を表示
	const char* phaseStr = "";
	switch (phase_) {
	case Phase::Intro: phaseStr = "Intro"; break;
	case Phase::Brink: phaseStr = "Brink"; break;
	case Phase::Explosion: phaseStr = "Explosion"; break;
	case Phase::Transition: phaseStr = "Transition"; break;
	case Phase::Finish: phaseStr = "Finish"; break;
	}
	ImGui::Text("Current Phase : %s", phaseStr);

	ImGui::End();
}

void GameStartSequence::UpdateBlink()
{
	// 点滅間隔（経過時間によって変更）
	float blinkInterval = 0.0f;

	// 0~1秒
	if (timer_ < 1.0f) {
		blinkInterval = 0.5f; // ゆっくり
		// 1~2秒
	} else if (timer_ < 2.0f) {
		blinkInterval = 0.25f; // 少し早く
		// 2~3秒
	} else {
		blinkInterval = 0.1f; // かなり早く
	}

	// 点滅周期の計算
	float blinkPhase = std::fmod(timer_, blinkInterval * 2.0f);
	isDynamiteVisible_ = (blinkPhase < blinkInterval);

	// 一定時間経過で次のフェーズへ
	if (timer_ > 3.0f) {
		timer_ = 0.0f;
		phase_ = Phase::Explosion;
	}
}

void GameStartSequence::UpdateExplosion()
{
	// 爆発時に1度のみ行う処理
	if (!isExplode_) {
		// パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("wallCollapse", objectCrumblingWall_->transform_.translate, 200); //壁崩壊パーティクル
		for(int i = 0; i < 50; i++){
			Float3 offset = RandomGenerator::GetInstance()->RandomValue({-2.0f, -2.5f, 0.0f}, {2.0f, 2.5f, 0.0f});
			// 爆発煙パーティクル
			ParticleEffectManager::GetInstance()->Emit("explodeSmoke", objectDynamite_->transform_.translate + offset, 1);
		}

		// カメラシェイク
		CameraShake::GetInstance()->StartShake(1.5f, 1.0f);
	}
	// カメラにシェイクオフセットを加算
	Camera::GetCurrent()->transform.translate = initCameraPos_ + CameraShake::GetInstance()->GetOffset();

	// 爆発終了したことを知らせる
	isExplode_ = true;

	// 一定時間経過で次のフェーズへ
	if (timer_ > 2.0f) {
		timer_ = 0.0f;
		phase_ = Phase::Transition;
	}
}

void GameStartSequence::UpdateTransition()
{
	// 補間進行
	const float speed = 0.5f;
	lerpT_ += TimeManager::GetInstance()->GetDeltaTime() * speed;
	float t = std::clamp(lerpT_, 0.0f, 1.0f);

	// 線形補間でカメラを移動
	float easeT = Easing::EaseOutSine(t);
	Camera::GetCurrent()->transform.translate = Float3::Lerp(initCameraPos_, topdownCameraPos_, easeT);
	Camera::GetCurrent()->transform.rotate = Float3::Lerp(initCameraRot_, topdownCameraRot_, easeT);

	// 線形補間でレターボックスを上下に移動
	float boxEase = Easing::EaseOutQuad(t);
	Float2 topPos = Float2::Lerp(topBoxStartPos_, topBoxEndPos_, boxEase);
	Float2 bottomPos = Float2::Lerp(bottomBoxStartPos_, bottomBoxEndPos_, boxEase);
	spriteTopLetterBox_->SetPosition(topPos);
	spriteBottomLetterBox_->SetPosition(bottomPos);

	// 補間終了で次のフェーズへ
	if (lerpT_ >= 1.0f) {
		lerpT_ = 1.0f;
		phase_ = Phase::Finish;
	}
}