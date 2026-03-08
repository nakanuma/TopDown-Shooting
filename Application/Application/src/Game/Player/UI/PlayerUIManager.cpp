#include "PlayerUIManager.h"

// Engine
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Player/Player.h>

void PlayerUIManager::Initialize() {
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

	// レティクル
	reticle_ = std::make_unique<Reticle>();
	reticle_->Initialize(dxBase, spriteCommon_.get());

	// HPバー
	hpBar_ = std::make_unique<HPBar>();
	hpBar_->Initialize(dxBase, spriteCommon_.get());

	// オーバーヒートゲージ
	overheatGauge_ = std::make_unique<OverheatGauge>();
	overheatGauge_->Initialize(dxBase, spriteCommon_.get());

	// 移動キー
	movementKeys_ = std::make_unique<MovementKeys>();
	movementKeys_->Initialize(spriteCommon_.get());

	// マウス
	mouse_ = std::make_unique<Mouse>();
	mouse_->Initialize(spriteCommon_.get());

	// ポーズキー
	pauseKey_ = std::make_unique<PauseKey>();
	pauseKey_->Initialize(spriteCommon_.get());

	// ダッシュ時クールタイム表示
	dashBar_ = std::make_unique<DashBar>();
	dashBar_->Initialize(spriteCommon_.get());
}

void PlayerUIManager::Update(const Player* player) {
	// レティクル
	reticle_->Update();
	// HPバー
	hpBar_->Update(player);
	// オーバーヒートゲージ
	overheatGauge_->Update(player);
	// 移動キー
	movementKeys_->Update(player);
	// マウス
	mouse_->Update();
	// ポーズキー
	pauseKey_->Update();
	// ダッシュ時クールタイム表示
	dashBar_->Update(player);
}

void PlayerUIManager::Draw() {
	// HPバー
	hpBar_->Draw();
	// レティクル
	reticle_->Draw();
	// オーバーヒートゲージ
	overheatGauge_->Draw();
	// 移動キー
	movementKeys_->Draw();
	// マウス
	mouse_->Draw();
	// ポーズキー
	pauseKey_->Draw();
	// ダッシュ時クールタイム表示
	dashBar_->Draw();
}
