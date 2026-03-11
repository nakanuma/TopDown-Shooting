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
}

void MouseCursor::Update() { 
	// カーソルの位置に追従
	Cygnus::Float2 mousePos = {
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().x), 
		static_cast<float>(Cygnus::Input::GetInstance()->GetMousePosition().y)
	};

	spriteCursor_->SetPosition(mousePos);
	spriteCursor_->Update(); 
}

void MouseCursor::Draw() { 
	spriteCursor_->Draw(); 
}