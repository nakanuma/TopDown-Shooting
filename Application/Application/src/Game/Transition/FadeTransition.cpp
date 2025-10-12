#define NOMINMAX
#include "FadeTransition.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <Engine/Util/TimeManager.h>
#include "SplitBlockTransition.h"

// ---------------------------------------------------------
// インスタンス取得
// ---------------------------------------------------------
FadeTransition* FadeTransition::GetInstance()
{
	static FadeTransition instance;
	return &instance;
}

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void FadeTransition::Initialize(SpriteCommon* spriteCommon)
{
	uint32_t textureWhite = TextureManager::Load("resources/Images/white.png", DirectXBase::GetInstance()->GetDevice());
	Float2 windowSize = { static_cast<float>(Window::GetWidth()), static_cast<float>(Window::GetHeight()) };

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(spriteCommon, textureWhite);
	sprite_->SetSize(windowSize);
	sprite_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
}

// ---------------------------------------------------------
// フェードイン開始（暗い画面から徐々に見えるように）
// ---------------------------------------------------------
void FadeTransition::StartFadeIn(float duration, float delayBeforeStart)
{
	state_ = State::FadeIn;
	alpha_ = 1.0f;
	duration_ = duration;
	timer_ = 0.0f;
	delayBeforeFadeIn_ = delayBeforeStart;
	onFadeComplete_ = nullptr;
}

// ---------------------------------------------------------
// フェードアウト開始（徐々に暗くなるように）
// ---------------------------------------------------------
void FadeTransition::StartFadeOut(float duration, std::function<void()> onComplete, float delayAfterComplete)
{
	state_ = State::FadeOut;
	alpha_ = 0.0f;
	duration_ = duration;
	timer_ = 0.0f;
	delayAfterFadeOutComplete_ = delayAfterComplete;
	delayTimerAfterFadeOut_ = 0.0f;
	onFadeComplete_ = onComplete;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void FadeTransition::Update() 
{
	if (state_ == State::None) return;

	float dt = TimeManager::GetInstance()->GetDeltaTime();

	// フェードイン
	if (state_ == State::FadeIn) {
		// フェードイン前遅延待機
		if (delayBeforeFadeIn_ > 0.0f) {
			delayBeforeFadeIn_ -= dt;
			if (delayBeforeFadeIn_ < 0.0f) delayBeforeFadeIn_ = 0.0f;

			// 待機中は暗く
			alpha_ = 1.0f;
			sprite_->Update();
			return;
		}

		timer_ += dt;
		float t = std::min(timer_ / duration_, 1.0f);
		alpha_ = 1.0f - t; // 徐々に明るく
		if (t >= 1.0f) {
			state_ = State::None;
		}
	} else if (state_ == State::FadeOut) {
		timer_ += dt;
		float t = std::min(timer_ / duration_, 1.0f);
		alpha_ = t; // 徐々に暗く

		if (t >= 1.0f) {
			// フェードアウト完了後の遅延待機
			if (delayAfterFadeOutComplete_ > 0.0f) {
				delayTimerAfterFadeOut_ += dt;
				alpha_ = 1.0f; // 暗いまま

				// 遅延終了
				if (delayTimerAfterFadeOut_ >= delayAfterFadeOutComplete_) {
					state_ = State::None;
					delayAfterFadeOutComplete_ = 0.0f;
					delayTimerAfterFadeOut_ = 0.0f;
					// 関数呼び出し
					if (onFadeComplete_) {
						onFadeComplete_();
					}
				}

				sprite_->Update();
				return;
			} else {
				state_ = State::None;
				if (onFadeComplete_) {
					onFadeComplete_();
				}
			}
		}
	}

	sprite_->Update();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void FadeTransition::Draw() 
{
	if (state_ == State::None && alpha_ <= 0.0f) return;

	sprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha_ });
	sprite_->Draw();
}
