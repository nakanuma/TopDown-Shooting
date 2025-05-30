#include "RemainingBullets.h"

// Engine
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Player/Player.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void RemainingBullets::Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon) 
{
	///
	/// 残弾表示（後景）
	/// 
	
	uint32_t textureRemainingBulletsBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteRemainingBulletsBackground_ = std::make_unique<Sprite>();
	spriteRemainingBulletsBackground_->Initialize(spriteCommon, textureRemainingBulletsBackground);
	spriteRemainingBulletsBackground_->SetSize(kRemainingBulletsSize);
	spriteRemainingBulletsBackground_->SetColor({0.25f, 0.25f, 0.25f, 0.5f}); // 灰

	///
	/// 残弾表示（前景）
	/// 
	
	uint32_t textureRemainingBulletsForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteRemainingBulletsForeground_ = std::make_unique<Sprite>();
	spriteRemainingBulletsForeground_->Initialize(spriteCommon, textureRemainingBulletsForeground);
	spriteRemainingBulletsForeground_->SetSize(kRemainingBulletsSize);
	spriteRemainingBulletsForeground_->SetColor({0.0f, 0.0f, 0.0f, 0.5f}); // 黒（半透明）

	///
	/// 残弾表示（アイコン）
	/// 
	
	uint32_t textureRemainingBulletsIcon = TextureManager::Load("resources/Images/UI/remainingBulletsIcon.png", dxBase->GetDevice());
	spriteRemainingBulletsIcon_ = std::make_unique<Sprite>();
	spriteRemainingBulletsIcon_->Initialize(spriteCommon, textureRemainingBulletsIcon);
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void RemainingBullets::Update(const Player* player) 
{
	///
	/// 残弾表示（後景）
	/// 
	
	spriteRemainingBulletsBackground_->SetPosition(kRemainingBulletsPosition);
	spriteRemainingBulletsBackground_->Update();

	///
	/// 残弾表示（前景）
	/// 
	
	float reloadingRatio = player->GetReloadTimer() / player->GetMaxReloadTime();
	Float2 remainingBulletsForegroundSize = {kRemainingBulletsSize.x * reloadingRatio, kRemainingBulletsSize.y};
	spriteRemainingBulletsForeground_->SetSize(remainingBulletsForegroundSize);

	spriteRemainingBulletsForeground_->SetPosition(kRemainingBulletsPosition);
	spriteRemainingBulletsForeground_->Update();

	///
	/// 残弾表示（アイコン）
	/// 
	
	spriteRemainingBulletsIcon_->SetPosition(kRemainingBulletsPosition);
	spriteRemainingBulletsIcon_->Update();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void RemainingBullets::Draw() 
{
	///
	///	残弾表示（後景）描画
	///

	spriteRemainingBulletsBackground_->Draw();

	///
	///	残弾表示（前景）描画
	///

	spriteRemainingBulletsForeground_->Draw();

	///
	///	残弾表示（アイコン）描画
	///

	spriteRemainingBulletsIcon_->Draw();
}
