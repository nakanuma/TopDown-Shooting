#include "HPBar.h"

// Engine
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Player/Player.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void HPBar::Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon) 
{
	///
	/// HPバー（後景）
	///
	
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon, textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({0.0f, 0.0f, 0.0f, 1.0f}); // 黒

	///
	/// HPバー（前景）
	///
	
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon, textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({1.0f, 0.2f, 0.2f, 1.0f}); // 赤
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void HPBar::Update(const Player* player) {
	///
	/// HPバー（後景）
	/// 
	
	spriteHPBackground_->SetPosition(kHPBarPosition);
	spriteHPBackground_->Update();

	///
	/// HPバー（前景）
	/// 
	
	// 現在HPに応じてサイズを変更
	float hpRatio = static_cast<float>(player->GetCurrentHP()) / static_cast<float>(player->GetMaxHP()); // HP割合

	Float2 hpBarForegroundSize = {kHPBarSize.x * hpRatio, kHPBarSize.y};
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	spriteHPForeground_->SetPosition(kHPBarPosition);
	spriteHPForeground_->Update();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void HPBar::Draw() {
	///
	///	HPバー（後景）
	/// 

	spriteHPBackground_->Draw();

	///
	///	HPバー（前景）
	/// 
	
	spriteHPForeground_->Draw();
}
