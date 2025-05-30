#include "PlayerUIManager.h"

// Engine
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Player/Player.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void PlayerUIManager::Initialize() 
{
	///
	///	基盤機能
	///

	DirectXBase* dxBase = DirectXBase::GetInstance();

	// スプライト基盤
	spriteCommon_ = std::make_unique<SpriteCommon>();
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

	// 残弾表示
	remainingBullets_ = std::make_unique<RemainingBullets>();
	remainingBullets_->Initialize(dxBase, spriteCommon_.get());
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void PlayerUIManager::Update(const Player* player) {
	// レティクル
	reticle_->Update();

	// HPバー
	hpBar_->Update(player);

	// 残弾表示
	remainingBullets_->Update(player);
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void PlayerUIManager::Draw() 
{
	// レティクル
	reticle_->Draw();

	// HPバー
	hpBar_->Draw();

	// 残弾表示
	remainingBullets_->Draw();
}
