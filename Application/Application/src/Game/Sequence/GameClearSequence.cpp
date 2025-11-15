#include "GameClearSequence.h"

// Engine
#include <ImguiWrapper.h>
#include <TimeManager.h>
#include <Camera.h>
#include <Easing.h>
#include <Window/MyWindow.h>

// Application
#include <src/Game/Camera/CameraShake.h>

void GameClearSequence::Initialize(SpriteCommon* spriteCommon) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// スプライト生成
	uint32_t textureWhite = TextureManager::Load("white.png");
	spriteBackGround_ = std::make_unique<Sprite>();
	spriteBackGround_->Initialize(spriteCommon, textureWhite);
	spriteBackGround_->SetSize({static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight())}); // 画面サイズに合わせる
	spriteBackGround_->SetColor({0.0f, 0.0f, 0.0f, 0.0f});

	uint32_t textureClearText = TextureManager::Load("UI/clearText.png");
	spriteClearText_ = std::make_unique<Sprite>();
	spriteClearText_->Initialize(spriteCommon, textureClearText);
	spriteClearText_->SetAnchorPoint({0.5f, 0.5f});
	spriteClearText_->SetPosition({ static_cast<float>(Window::GetWidth() / 2.0f), static_cast<float>(Window::GetHeight() / 2.0f) }); // 画面中央
	savedClearTextSize_ = spriteClearText_->GetSize(); // 初期サイズを保存
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
	case Phase::ExplodeAndText:
		// 爆発とクリア文字演出時の更新処理
		UpdateExplodeAndText();
		break;
	case Phase::Finish:
		break;
	}
}

void GameClearSequence::DrawUI() {
	// ゲームクリア演出が行われていない間はスキップ
	if (!IsActive()) return;
	// 爆発とクリア文字演出時のみ描画するよう制限
	if(phase_ != Phase::ExplodeAndText) return;

	spriteBackGround_->Draw();
	spriteClearText_->Draw();
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
	case GameClearSequence::Phase::ExplodeAndText: phaseStr = "ExplodeAndText"; break;
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
		phase_ = Phase::ExplodeAndText;

		// カメラの位置と回転を元に戻す
		Camera::GetCurrent()->transform_.translate_ = savedCameraPos_;
		Camera::GetCurrent()->transform_.rotate_ = savedCameraRot_;
	}
}

void GameClearSequence::UpdateExplodeAndText() {
	///
	/// 背景スプライトの更新
	/// 

	// 背景スプライトのフェードにかかる時間
	const float kFadeDuration = 1.0f;
	// 背景スプライトの最大Alpha値
	const float kMaxAlpha = 0.75f;

	// 現在のアルファ値
	float currentAlpha = 0.0f;
	
	// フェードイン状態
	if(timer_ < kFadeDuration){
		// 進行度の計算
		float t = std::clamp(timer_ / kFadeDuration, 0.0f, 1.0f);
		// 0.0fから最大Alpha値まで増加
		currentAlpha = Easing::Lerp(0.0f, kMaxAlpha, t);

	// フェードアウト状態
	} else if (timer_ > kExplodeDuration - kFadeDuration){
		// フェードアウト開始からの時間
		float fadeOutTimer = timer_ - (kExplodeDuration - kFadeDuration);
		// 進行度の計算
		float t = std::clamp(fadeOutTimer / kFadeDuration, 0.0f, 1.0f);
		// 最大Alpha値から0.0fまで減少
		currentAlpha = Easing::Lerp(kMaxAlpha, 0.0f, t);

	// 中間状態
	} else {
		// 最大Alpha値を維持
		currentAlpha = kMaxAlpha;
	}
	// スプライトにAlpha値を適用
	Float4 currentColor = spriteBackGround_->GetColor();
	currentColor.w = currentAlpha;
	spriteBackGround_->SetColor(currentColor);

	spriteBackGround_->Update();

	///
	/// クリア文字スプライトの更新
	///

	// クリア文字スプライトのフェードにかかる時間
	const float kTextFadeDuration = kFadeDuration / 2.0f;
	// クリア文字スプライトの上へスライドする距離
	const float kSlideDistance = 50.0f;

	// 初期位置
	Float2 basePos = { static_cast<float>(Window::GetWidth() / 2.0f), static_cast<float>(Window::GetHeight() / 2.0f) };
	// 現在の色
	Float4 currentTextColor = spriteClearText_->GetColor();

	// フェードイン状態（サイズ変更アニメーションのみ）
	if(timer_ <= kTextFadeDuration){
		// 進行度の計算
		float t = std::clamp(timer_ / kTextFadeDuration, 0.0f, 1.0f);
		// 1.2倍から等倍へサイズ変更
		Float2 currentSize = Float2::Lerp(savedClearTextSize_ * 1.2f, savedClearTextSize_, Easing::EaseOutBack(t));

		// Alphaを0.0fから1.0fへ増加
		currentTextColor.w = Easing::Lerp(0.0f, 1.0f, t);

		// サイズを適用
		spriteClearText_->SetSize(currentSize);
	// フェードアウト状態（Alpha値変更とスライド移動）
	} else if (timer_ >= kExplodeDuration - kTextFadeDuration){
		// フェードアウト開始からの時間
		float fadeOutTimer = timer_ - (kExplodeDuration - kTextFadeDuration);
		// 進行度の計算
		float t = std::clamp(fadeOutTimer / kTextFadeDuration, 0.0f, 1.0f);

		// Alphaを1.0fから0.0fへ減少
		currentTextColor.w = Easing::Lerp(1.0f, 0.0f, t);

		// スライド移動のY座標を補間して適用
		float currentY = Easing::Lerp(basePos.y, basePos.y - kSlideDistance, t);
		spriteClearText_->SetPosition({basePos.x, currentY});
	} 
	// スプライトにAlphaを設定
	spriteClearText_->SetColor(currentTextColor);

	spriteClearText_->Update();

	// 一定時間経過で次のフェーズへ
	if (timer_ > kExplodeDuration) {
		timer_ = 0.0f;
		phase_ = Phase::Finish;
	}
}
