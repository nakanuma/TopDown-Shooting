#include "InputGuideUIManager.h"

// Engine
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Player/Player.h>

void InputGuideUIManager::Initialize() {
	///
	///	基盤機能
	///

	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// スプライト基盤
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

	///
	///	UI生成・初期化
	///

	// 移動キー
	movementKeys_ = std::make_unique<MovementKeys>();
	movementKeys_->Initialize(spriteCommon_.get());

	// マウス
	mouse_ = std::make_unique<Mouse>();
	mouse_->Initialize(spriteCommon_.get());

	// ポーズキー
	pauseKey_ = std::make_unique<PauseKey>();
	pauseKey_->Initialize(spriteCommon_.get());
}

void InputGuideUIManager::Update(const Player* player) {
	// 移動キー
	movementKeys_->Update(player);
	// マウス
	mouse_->Update();
	// ポーズキー
	pauseKey_->Update();
}

void InputGuideUIManager::Draw() {
	// 移動キー
	movementKeys_->Draw();
	// マウス
	mouse_->Draw();
	// ポーズキー
	pauseKey_->Draw();
}
