#include "PauseMenu.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <TimeManager.h>
#include <Easing.h>
#include <Input/Input.h>

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
	sizeContinueBase_ = spriteContinue_->GetSize(); // サイズを保持

	// タイトルへ戻る文字スプライト生成
	uint32_t textureBackToTitle = Cygnus::TextureManager::Load("UI/pause_backToTitle.png");
	spriteBackToTitle_ = std::make_unique<Cygnus::Sprite>();
	spriteBackToTitle_->Initialize(spriteCommon, textureBackToTitle);
	spriteBackToTitle_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteBackToTitle_->SetPosition(kBackToTitleInitPos);
	sizeBackToTitleBase_ = spriteBackToTitle_->GetSize(); // サイズを保持

	// カーソルUI生成
	cursor_ = std::make_unique<Cursor>();
	cursor_->Initialize();
}

void PauseMenu::Update() {
	// 背景スプライトの更新
	UpdateBackground();
	// メニュー項目のアニメーション処理
	UpdateMenuAnimation();
	// メニュー項目のマウス判定処理
	if(animationProgress_ > 0.0f) {
		UpdateButtons();
	}

	// カーソルUI更新（ポーズ中のみ）
	if (isShowing_) {
		cursor_->Update();
	}
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

	// カーソルUI描画（ポーズ中のみ）
	if (isShowing_) {
		cursor_->Draw();
	}
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

	auto updateItem = [&](Cygnus::Sprite* sprite, Cygnus::Float2 basePos, int delayIndex) {
		// 各項目の開始タイミングをずらす
		float startThreshold = delayIndex * kAnimDelay;
		float localProgress = (animationProgress_ - startThreshold) / (1.0f - startThreshold);
		localProgress = std::clamp(localProgress, 0.0f, 1.0f);

		// イージング適用
		float easeValue = Cygnus::Easing::EaseOutQuad(localProgress);

		// 座標の更新
		float currentOffset = (1.0f - easeValue) * kSlideOffset;
		sprite->SetPosition({basePos.x + currentOffset, basePos.y});
		// アルファ更新
		sprite->SetColor({1.0f, 1.0f, 1.0f, localProgress });

		sprite->Update();
	};

	updateItem(spritePauseText_.get(), kPauseInitPos, 0);
	updateItem(spriteContinue_.get(), kContinueInitPos, 1);
	updateItem(spriteBackToTitle_.get(), kBackToTitleInitPos, 2);
}

void PauseMenu::UpdateButtons()
{
	// マウス位置の取得
	Cygnus::Float2 mousePos = { 
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().x), 
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().y) 
	};

	auto handleHover = [&](Cygnus::Sprite* sprite, float localAlpha, const Cygnus::Float2& baseSize) {
		// 非表示中なら判定しない
		if(localAlpha <= 0.0f) return;

		Cygnus::Float2 pos = sprite->GetPosition();
		Cygnus::Float2 size = sprite->GetSize();

		// 当たり判定
		bool isHover = (
			mousePos.x >= pos.x - size.x / 2.0f && mousePos.x <= pos.x + size.x / 2.0f && 
			mousePos.y >= pos.y - size.y / 2.0f && mousePos.y <= pos.y + size.y / 2.0f
			);

		// スケール設定
		float scale = isHover ? kScaleHover : kScaleDefault;
		sprite->SetSize({baseSize.x * scale, baseSize.y * scale});
		
		// 色設定
		Cygnus::Float4 targetColor = isHover ? kColorHover : kColorDefault;
		targetColor.w = localAlpha;
		sprite->SetColor(targetColor);

		// クリック判定
		if(isHover && Cygnus::Input::GetInstance()->IsTriggerMouse(0)) {
			// 「ゲームを続ける」ボタンの場合
			if(sprite == spriteContinue_.get()) {
				// コールバック関数の実行
				if(closeCallback_) closeCallback_();
			// 「タイトルへ戻る」ボタンの場合
			} else if (sprite == spriteBackToTitle_.get()) {
				// コールバック関数の実行
				if(titleCallback_) titleCallback_();
			}
		}
	};

	float continueAlpha = std::clamp((animationProgress_ - kAnimDelay) / (1.0f - kAnimDelay), 0.0f, 1.0f);
	float titleAlpha = std::clamp((animationProgress_ - kAnimDelay * 2.0f) / (1.0f - kAnimDelay * 2.0f), 0.0f, 1.0f);

	handleHover(spriteContinue_.get(), continueAlpha, sizeContinueBase_);
	handleHover(spriteBackToTitle_.get(), titleAlpha, sizeBackToTitleBase_);
}
