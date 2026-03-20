#include "GameClearSequence.h"

// Engine
#include <Camera.h>
#include <Easing.h>
#include <ImguiWrapper.h>
#include <ParticleEffect/ParticleEffectManager.h>
#include <RandomGenerator.h>
#include <TimeManager.h>
#include <Window/MyWindow.h>

// Application
#include <src/Game/Camera/CameraShake.h>

void GameClearSequence::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// スプライト生成
	uint32_t textureWhite = Cygnus::TextureManager::Load("white.png");
	spriteBackGround_ = std::make_unique<Cygnus::Sprite>();
	spriteBackGround_->Initialize(spriteCommon, textureWhite);
	spriteBackGround_->SetSize({ static_cast<float>(Cygnus::Window::GetWidth()), static_cast<float>(Cygnus::Window::GetHeight()) }); // 画面サイズに合わせる
	spriteBackGround_->SetColor(kInitialBackgroundColor);

	uint32_t textureClearText = Cygnus::TextureManager::Load("UI/clearText.png");
	spriteClearText_ = std::make_unique<Cygnus::Sprite>();
	spriteClearText_->Initialize(spriteCommon, textureClearText);
	spriteClearText_->SetAnchorPoint(kAnchorPoint);
	spriteClearText_->SetPosition({ static_cast<float>(Cygnus::Window::GetWidth() / 2.0f), static_cast<float>(Cygnus::Window::GetHeight() / 2.0f) }); // 画面中央
	savedClearTextSize_ = spriteClearText_->GetSize();                                                                              // 初期サイズを保存
}

void GameClearSequence::Start() {
	phase_ = Phase::Rotate; // ゲームクリア演出の開始
	timer_ = 0.0f;

	// 演出開始時のカメラ情報を保存
	savedCameraPos_ = Cygnus::Camera::GetCurrent()->transform_.translate_;
	savedCameraRot_ = Cygnus::Camera::GetCurrent()->transform_.rotate_;

	// カメラシェイクを開始（カメラ回転 + 爆発フェーズまで）
	CameraShake::GetInstance()->StartShake(kCameraRotateDuration + kExplodeDuration, kCameraShakeIntensity, CameraShake::ShakePriority::High); // 上書きされないよう優先度高

	// ボスの最終位置を保存
	if (boss_) {
		lastBossPosition_ = boss_->GetTranslate();
	}
}

void GameClearSequence::Update() {
	// タイマー更新
	if (!IsFinished()) {
		timer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
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
	// 爆発とクリア文字演出時のみ描画するよう制限
	if (phase_ != Phase::ExplodeAndText)
		return;

	spriteBackGround_->Draw();
	spriteClearText_->Draw();
}

void GameClearSequence::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("GameClearSeqence");

	ImGui::Text("Timer : %.2f", timer_);

	// フェーズ名の表示
	const char* phaseStr = "";
	switch (phase_) {
	case GameClearSequence::Phase::None:
		phaseStr = "None";
		break;
	case GameClearSequence::Phase::Rotate:
		phaseStr = "Rotate";
		break;
	case GameClearSequence::Phase::ExplodeAndText:
		phaseStr = "ExplodeAndText";
		break;
	case GameClearSequence::Phase::Finish:
		phaseStr = "Finish";
		break;
	}
	ImGui::Text("Current Phase : %s", phaseStr);

	ImGui::End();
#endif
}

void GameClearSequence::UpdateRotate() {
	///
	///	カメラ回転更新
	///

	// 回転の進行度
	float t = std::clamp(timer_ / kCameraRotateDuration, 0.0f, 1.0f);

	// ボスの位置を取得
	Cygnus::Float3 bossPos = boss_->GetTranslate();
	// ボスのY軸回転を取得
	float bossForwardAngle = boss_->GetRotate().y;

	// 開始角度（ボスの正面 + 45度）
	float startAngle = bossForwardAngle + kCameraStartAngleOffset;
	// 終了角度（ボスの正面 - 45度）
	float endAngle = bossForwardAngle - kCameraEndAngleOffset;

	// 現在の角度を補間
	float currentAngle = Cygnus::Easing::Lerp(startAngle, endAngle, Cygnus::Easing::EaseOutSine(t));

	// カメラの高さを補間
	float currentHeight = Cygnus::Easing::Lerp(kCameraHeightStart, kCameraHeightEnd, t);

	// ボス中心の円周上のカメラ位置を計算
	Cygnus::Float3 cameraPos = { bossPos.x + sinf(currentAngle) * kCameraDistance, bossPos.y + bossPos.y + currentHeight, bossPos.z + cosf(currentAngle) * kCameraDistance };

	// カメラをボスの方向に向ける
	Cygnus::Float3 direction = bossPos - cameraPos;
	direction = Cygnus::Float3::Normalize(direction);

	// カメラの回転を計算
	Cygnus::Float3 cameraRot = { asinf(-direction.y), atan2f(direction.x, direction.z), kCameraRotZOffset };

	// カメラに適用
	Cygnus::Camera::GetCurrent()->transform_.translate_ = cameraPos + CameraShake::GetInstance()->GetOffset(); // カメラシェイクも加算
	Cygnus::Camera::GetCurrent()->transform_.rotate_ = cameraRot;

	///
	///	パーティクル発生
	///

	EmitParticle(true);

	// 回転終了で次のフェーズへ
	if (timer_ > kCameraRotateDuration) {
		timer_ = 0.0f;
		phase_ = Phase::ExplodeAndText;

		// カメラの位置と回転を元に戻す
		Cygnus::Camera::GetCurrent()->transform_.translate_ = savedCameraPos_;
		Cygnus::Camera::GetCurrent()->transform_.rotate_ = savedCameraRot_;
	}
}

void GameClearSequence::UpdateExplodeAndText() {
	///
	///	パーティクル発生
	///

	EmitParticle(false);

	///
	/// 背景スプライトの更新
	///

	// 現在のアルファ値
	float currentAlpha = 0.0f;

	// フェードイン状態
	if (timer_ < kBackgroundFadeDuration) {
		// 進行度の計算
		float t = std::clamp(timer_ / kBackgroundFadeDuration, 0.0f, 1.0f);
		// 0.0fから最大Alpha値まで増加
		currentAlpha = Cygnus::Easing::Lerp(0.0f, kBackgroundMaxAlpha, t);

		// フェードアウト状態
	} else if (timer_ > kExplodeDuration - kBackgroundFadeDuration) {
		// フェードアウト開始からの時間
		float fadeOutTimer = timer_ - (kExplodeDuration - kBackgroundFadeDuration);
		// 進行度の計算
		float t = std::clamp(fadeOutTimer / kBackgroundFadeDuration, 0.0f, 1.0f);
		// 最大Alpha値から0.0fまで減少
		currentAlpha = Cygnus::Easing::Lerp(kBackgroundMaxAlpha, 0.0f, t);

		// 中間状態
	} else {
		// 最大Alpha値を維持
		currentAlpha = kBackgroundMaxAlpha;
	}
	// スプライトにAlpha値を適用
	Cygnus::Float4 currentColor = spriteBackGround_->GetColor();
	currentColor.w = currentAlpha;
	spriteBackGround_->SetColor(currentColor);

	spriteBackGround_->Update();

	///
	/// クリア文字スプライトの更新
	///

	// 初期位置
	Cygnus::Float2 basePos = { static_cast<float>(Cygnus::Window::GetWidth() / 2.0f), static_cast<float>(Cygnus::Window::GetHeight() / 2.0f) };
	// 現在の色
	Cygnus::Float4 currentTextColor = spriteClearText_->GetColor();

	// フェードイン状態（サイズ変更アニメーションのみ）
	if (timer_ <= kTextFadeDuration) {
		// 進行度の計算
		float t = std::clamp(timer_ / kTextFadeDuration, 0.0f, 1.0f);
		// 1.2倍から等倍へサイズ変更
		Cygnus::Float2 currentSize = Cygnus::Float2::Lerp(savedClearTextSize_ * kClearTextSizeStartMultiplier, savedClearTextSize_, Cygnus::Easing::EaseOutBack(t));

		// Alphaを0.0fから1.0fへ増加
		currentTextColor.w = Cygnus::Easing::Lerp(0.0f, 1.0f, t);

		// サイズを適用
		spriteClearText_->SetSize(currentSize);
		// フェードアウト状態（Alpha値変更とスライド移動）
	} else if (timer_ >= kExplodeDuration - kTextFadeDuration) {
		// フェードアウト開始からの時間
		float fadeOutTimer = timer_ - (kExplodeDuration - kTextFadeDuration);
		// 進行度の計算
		float t = std::clamp(fadeOutTimer / kTextFadeDuration, 0.0f, 1.0f);

		// Alphaを1.0fから0.0fへ減少
		currentTextColor.w = Cygnus::Easing::Lerp(1.0f, 0.0f, t);

		// スライド移動のY座標を補間して適用
		float currentY = Cygnus::Easing::Lerp(basePos.y, basePos.y - kSlideDistance, t);
		spriteClearText_->SetPosition({ basePos.x, currentY });
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

void GameClearSequence::EmitParticle(bool isScatterEmit)
{
	// タイマーの加算
	particleEmitTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 発生間隔ごとにEmit
	if (particleEmitTimer_ >= kExplodeEmitInterval) {
		Cygnus::Float3 offset = Cygnus::RandomGenerator::GetInstance()->RandomValue(kParticleOffsetMin, kParticleOffsetMax);
		Cygnus::ParticleEffectManager::GetInstance()->Emit("explodeSmoke", lastBossPosition_ + offset, kExplodeSmokeEmitCount); // 煙パーティクル発生

		if (isScatterEmit) {
			Cygnus::ParticleEffectManager::GetInstance()->Emit("explodeScatter", lastBossPosition_ + offset, kExplodeScatterEmitCount); // 爆発飛散パーティクル発生
		}

		// パーティクル発生タイマーのリセット
		particleEmitTimer_ = 0.0f;
	}
}
