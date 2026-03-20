#include "MouseCursor.h"

// Engine
#include <TextureManager.h>
#include <Input.h>

void MouseCursor::Initialize() {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// スプライト基盤生成
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

	// スプライト生成
	uint32_t texture = Cygnus::TextureManager::Load("UI/cursor.png");
	spriteCursor_ = std::make_unique<Cygnus::Sprite>();
	spriteCursor_->Initialize(spriteCommon_.get(), texture);

	baseSize_ = spriteCursor_->GetSize(); // 初期サイズを保持

	currentScale_ = 1.0f;
	currentColor = kNormalColor;
}

void MouseCursor::Update() { 
	// カーソルの位置に追従
	Cygnus::Float2 mousePos = {
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().x), 
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().y)
	};
	spriteCursor_->SetPosition(mousePos);

	// スケールと色の目標値
	float targetScale = 1.0f;
	Cygnus::Float4 targetColor = kNormalColor;

	// 左クリック押下時に目標値を設定
	if(Cygnus::Input::GetInstance()->IsPressMouse(0)) {
		targetScale = kClickScale;
		targetColor = kClickColor;
	}

	// 線形補間で値を更新
	currentScale_ += (targetScale - currentScale_) * kAnimSpeed;
	currentColor.x += (targetColor.x - currentColor.x) * kAnimSpeed;
	currentColor.y += (targetColor.y - currentColor.y) * kAnimSpeed;
	currentColor.z += (targetColor.z - currentColor.z) * kAnimSpeed;

	Cygnus::Float2 newSize = baseSize_ * currentScale_;

	spriteCursor_->SetSize(newSize);
	spriteCursor_->SetColor(currentColor);

	spriteCursor_->Update(); 
}

void MouseCursor::Draw() { 
	spriteCursor_->Draw(); 
}