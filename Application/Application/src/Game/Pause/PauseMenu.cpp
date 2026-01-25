#include "PauseMenu.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <TimeManager.h>
#include <Easing.h>

void PauseMenu::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// 背景スプライト生成
	uint32_t textureBackGround = Cygnus::TextureManager::Load("white.png");
	Cygnus::Float2 windowSize = { static_cast<float>(Cygnus::Window::GetWidth()), static_cast<float>(Cygnus::Window::GetHeight()) };
	spriteBackGround_ = std::make_unique<Cygnus::Sprite>();
	spriteBackGround_->Initialize(spriteCommon, textureBackGround);
	spriteBackGround_->SetSize(windowSize);
	Cygnus::Float4 backGroundColor = kBackGroundColor;
	backGroundColor.w = 0.0f; // 最初は透明にする
	spriteBackGround_->SetColor(backGroundColor);

	// ポーズ文字スプライト生成
	uint32_t texturePause = Cygnus::TextureManager::Load("UI/pause.png");
	spritePauseText_ = std::make_unique<Cygnus::Sprite>();
	spritePauseText_->Initialize(spriteCommon, texturePause);
	spritePauseText_->SetAnchorPoint({0.5f, 0.5f});
	spritePauseText_->SetPosition(kPauseInitPos);

	// ゲームを続ける文字スプライト生成
	uint32_t textureContinue = Cygnus::TextureManager::Load("UI/pause_continue.png");
	spriteContinue_ = std::make_unique<Cygnus::Sprite>();
	spriteContinue_->Initialize(spriteCommon, textureContinue);
	spriteContinue_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteContinue_->SetPosition(kContinueInitPos);

	// タイトルへ戻る文字スプライト生成
	uint32_t textureBackToTitle = Cygnus::TextureManager::Load("UI/pause_backToTitle.png");
	spriteBackToTitle_ = std::make_unique<Cygnus::Sprite>();
	spriteBackToTitle_->Initialize(spriteCommon, textureBackToTitle);
	spriteBackToTitle_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteBackToTitle_->SetPosition(kBackToTitleInitPos);
}

void PauseMenu::Update() {
	// 背景スプライトの更新
	UpdateBackground();
	// メニュー項目のアニメーション処理
	UpdateMenuAnimation();
}

void PauseMenu::DrawUI() {
	// 背景スプライトの描画
	spriteBackGround_->Draw();
	// ポーズ文字スプライトの描画
	spritePauseText_->Draw();
	// ゲームを続ける文字スプライトの描画
	spriteContinue_->Draw();
	// タイトルへ戻る文字スプライトの描画
	spriteBackToTitle_->Draw();
}

void PauseMenu::Show() {
	isShowing_ = true;
	backGroundTargetAlpha_ = kBackGroundColor.w; // 目標値を半透明に設定
}

void PauseMenu::Hide() {
	isShowing_ = false;
	backGroundTargetAlpha_ = 0.0f; // 目標値を透明に設定
}

void PauseMenu::UpdateBackground() {
	float deltaTime = Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 背景のアルファ値を目標値に向かって徐々に変化させる
	if(backGroundCurrentAlpha_ != backGroundTargetAlpha_) {
		float fadeSpeed = (kBackGroundColor.w / kBackgroundFadeDuration) * deltaTime;

		// ポーズ開始時の処理（Alphaの増加）
		if(backGroundCurrentAlpha_ < backGroundTargetAlpha_) {
			backGroundCurrentAlpha_ += fadeSpeed;

			// 上限に達したら値を固定
			if(backGroundCurrentAlpha_ > backGroundTargetAlpha_) {
				backGroundCurrentAlpha_ = backGroundTargetAlpha_;
			}

		// ポーズ終了時の処理（Alphaの減少）
		} else if (backGroundCurrentAlpha_ > backGroundTargetAlpha_) {
			backGroundCurrentAlpha_ -= fadeSpeed;

			// 下限に達したら値を固定
			if(backGroundCurrentAlpha_ < backGroundTargetAlpha_) {
				backGroundCurrentAlpha_ = backGroundTargetAlpha_;
			}
		}
	}

	Cygnus::Float4 color = kBackGroundColor;
	color.w = backGroundCurrentAlpha_;
	spriteBackGround_->SetColor(color);

	spriteBackGround_->Update();
}

void PauseMenu::UpdateMenuAnimation()
{
	float deltaTime = Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// タイマー更新
	if(isShowing_) {
		// 表示開始でタイマー増加
		animationProgress_ += (1.0f / kMenuFadeDuration) * deltaTime;
	} else {
		// 表示終了でタイマー減少
		animationProgress_ -= (1.0f / kMenuFadeDuration) * deltaTime;
	}
	animationProgress_ = std::clamp(animationProgress_, 0.0f, 1.0f);

	// イージングの適用
	float easeT = Cygnus::Easing::EaseOutQuad(animationProgress_);

	// 時間に応じたオフセットを計算
	float currentOffset = (1.0f - easeT) * kSlideOffset;

	auto updateItem = [&](Cygnus::Sprite* sprite, Cygnus::Float2 basePos) {
		// 座標の更新
		sprite->SetPosition({basePos.x + currentOffset, basePos.y});
		// アルファ更新
		sprite->SetColor({1.0f, 1.0f, 1.0f, animationProgress_});
		sprite->Update();
	};

	updateItem(spritePauseText_.get(), kPauseInitPos);
	updateItem(spriteContinue_.get(), kContinueInitPos);
	updateItem(spriteBackToTitle_.get(), kBackToTitleInitPos);
}
