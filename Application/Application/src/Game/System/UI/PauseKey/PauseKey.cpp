#include "PauseKey.h"

// Engine
#include <TextureManager.h>

void PauseKey::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	uint32_t texEscKey = Cygnus::TextureManager::Load("UI/key_esc.png");
	uint32_t texPauseText = Cygnus::TextureManager::Load("UI/pauseText.png");

	// ESCキースプライト生成
	spriteEscapeKey_ = std::make_unique<Cygnus::Sprite>();
	spriteEscapeKey_->Initialize(spriteCommon, texEscKey);
	spriteEscapeKey_->SetAnchorPoint({0.5f, 0.5f});
	spriteEscapeKey_->SetPosition(kEscKeyInitPos);

	// Pause文字スプライト生成
	spritePauseText_ = std::make_unique<Cygnus::Sprite>();
	spritePauseText_->Initialize(spriteCommon, texPauseText);
	spritePauseText_->SetAnchorPoint({ 0.5f, 0.5f });
	spritePauseText_->SetPosition(kPauseTextInitPos);
}

void PauseKey::Update() {
	spriteEscapeKey_->Update();
	spritePauseText_->Update();
}

void PauseKey::Draw() {
	spriteEscapeKey_->Draw();
	spritePauseText_->Draw();
}