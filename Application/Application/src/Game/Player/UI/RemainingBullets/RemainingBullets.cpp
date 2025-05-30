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
	spriteRemainingBulletsBackground_->SetSize(kRemainingBulletsGroundSize);
	spriteRemainingBulletsBackground_->SetColor({0.25f, 0.25f, 0.25f, 0.5f}); // 灰（半透明）

	///
	/// 残弾表示（前景）
	/// 
	
	uint32_t textureRemainingBulletsForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteRemainingBulletsForeground_ = std::make_unique<Sprite>();
	spriteRemainingBulletsForeground_->Initialize(spriteCommon, textureRemainingBulletsForeground);
	spriteRemainingBulletsForeground_->SetSize(kRemainingBulletsGroundSize);
	spriteRemainingBulletsForeground_->SetColor({0.0f, 0.0f, 0.0f, 0.5f}); // 黒（半透明）

	///
	/// 残弾表示（アイコン）
	/// 
	
	uint32_t textureRemainingBulletsIcon = TextureManager::Load("resources/Images/UI/remainingBulletsIcon.png", dxBase->GetDevice());
	spriteRemainingBulletsIcon_ = std::make_unique<Sprite>();
	spriteRemainingBulletsIcon_->Initialize(spriteCommon, textureRemainingBulletsIcon);

	///
	///	残弾数
	/// 
	
	uint32_t textureNumbers = TextureManager::Load("resources/Images/UI/numbers.png", dxBase->GetDevice());
	for (auto& sprite : spriteRemainingBullets_) {
		sprite = std::make_unique<Sprite>();
		sprite->Initialize(spriteCommon, textureNumbers);
		sprite->SetSize(kRemainingBulletsNumSize);
	}
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void RemainingBullets::Update(const Player* player) 
{
	///
	/// 残弾表示（後景）
	/// 
	
	spriteRemainingBulletsBackground_->SetPosition(kRemainingBulletsGroundPosition);
	spriteRemainingBulletsBackground_->Update();

	///
	/// 残弾表示（前景）
	/// 
	
	// 残りリロード時間に応じて左から右へ
	float reloadingRatio = player->GetReloadTimer() / player->GetMaxReloadTime();
	if (!player->IsReloading()) {
		reloadingRatio = 1.0f;
	}
	Float2 remainingBulletsForegroundSize = {kRemainingBulletsGroundSize.x * reloadingRatio, kRemainingBulletsGroundSize.y};
	spriteRemainingBulletsForeground_->SetSize(remainingBulletsForegroundSize);

	spriteRemainingBulletsForeground_->SetPosition(kRemainingBulletsGroundPosition);
	spriteRemainingBulletsForeground_->Update();

	///
	/// 残弾表示（アイコン）
	/// 
	
	spriteRemainingBulletsIcon_->SetPosition(kRemainingBulletsGroundPosition);
	spriteRemainingBulletsIcon_->Update();

	///
	///	残弾数
	/// 
	
	int32_t bulletDigit[2];
	int32_t bulletTemp = player->GetCurrentAmmo();
	for (int32_t i = 0; i < kMaxRemainingBulletsDigit; ++i) {
		bulletDigit[i] = bulletTemp % 10;
		bulletTemp /= 10;
	}

	for (int32_t i = 0; i < kMaxRemainingBulletsDigit; ++i) {
		Float2 digitPosition = kRemainingBulletNumPosition + Float2{static_cast<float>(i) * 20, 0.0f}; // 20ピクセル間隔
		
		spriteRemainingBullets_[i]->SetTextureLeftTop({0.0f + bulletDigit[kMaxRemainingBulletsDigit - 1 - i] * kRemainingBulletsNumSize.x, 0.0f});
		spriteRemainingBullets_[i]->SetTextureSize(kRemainingBulletsNumSize);
		spriteRemainingBullets_[i]->SetPosition(digitPosition);
		spriteRemainingBullets_[i]->Update();
	}
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void RemainingBullets::Draw() 
{
	///
	///	残弾表示（後景）
	///

	spriteRemainingBulletsBackground_->Draw();

	///
	///	残弾表示（前景）
	///

	spriteRemainingBulletsForeground_->Draw();

	///
	///	残弾表示（アイコン）
	///

	spriteRemainingBulletsIcon_->Draw();

	///
	/// 残弾数	
	/// 
	
	for (auto& sprite : spriteRemainingBullets_) {
		sprite->Draw();
	}
}
