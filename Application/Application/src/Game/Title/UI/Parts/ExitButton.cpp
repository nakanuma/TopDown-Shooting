#include "ExitButton.h"

// Engine
#include <TextureManager.h>
#include <Input.h>

// Application
#include <src/Game/Transition/FadeTransition.h>

void ExitButton::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// 終了ボタンスプライト生成
	uint32_t textureStart = Cygnus::TextureManager::Load("UI/exitButton.png");
	sprite_ = std::make_unique<Cygnus::Sprite>();
	sprite_->Initialize(spriteCommon, textureStart);
	sprite_->SetPosition(kExitButtonPosition);
	sprite_->SetAnchorPoint(kAnchorPoint);

	baseSize_ = sprite_->GetSize(); // スプライト初期サイズを保持
	isClicked_ = false;
}

void ExitButton::Update() {
	isClicked_ = false; // クリックしたフレームのみ判定されるよう毎フレームリセット

	// フェード中は更新スキップ
	if (!FadeTransition::GetInstance()->IsFinished()) return;

	// マウス位置を取得
	Cygnus::Float2 mousePos = {
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().x),
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().y)
	};

	// 当たり判定
	Cygnus::Float2 pos = sprite_->GetPosition();
	Cygnus::Float2 size = sprite_->GetSize();

	bool isHover = (
		mousePos.x >= pos.x - size.x / 2.0f && mousePos.x <= pos.x + size.x / 2.0f &&
		mousePos.y >= pos.y - size.y / 2.0f && mousePos.y <= pos.y + size.y / 2.0f
		);

	// ホバー時のアニメーション処理
	float targetScale = isHover ? kScaleHover : kScaleDefault;
	sprite_->SetSize({ baseSize_.x * targetScale, baseSize_.y * targetScale });

	Cygnus::Float4 targetColor = isHover ? kColorHover : kColorDefault;
	sprite_->SetColor(targetColor);

	// クリック判定
	if (isHover && Cygnus::Input::GetInstance()->IsTriggerMouse(0)) {
		isClicked_ = true;
	}

	// 更新
	sprite_->Update();
}

void ExitButton::Draw() {
	// フェード中は描画スキップ
	if (!FadeTransition::GetInstance()->IsFinished()) return;

	// 描画
	sprite_->Draw();
}