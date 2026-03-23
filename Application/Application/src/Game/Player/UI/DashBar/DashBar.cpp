#include "DashBar.h"

// Engine
#include <Engine/Texture/TextureManager.h>
#include <Easing.h>

// Application
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>


void DashBar::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	uint32_t textureHPBackground = Cygnus::TextureManager::Load("white.png");
	spriteDashBar_ = std::make_unique<Cygnus::Sprite>();
	spriteDashBar_->Initialize(spriteCommon, textureHPBackground);
	spriteDashBar_->SetSize(kDashBarSize);
	spriteDashBar_->SetColor(kDashBarColor);

	spriteDashBar_->SetPosition({360.0f, 360.0f});
}

void DashBar::Update(const Player* player) {
	// クールダウンの進捗率にイージングを適用し、スプライトの現在幅を計算（）
	progress_ = player->GetDashCooldownRatio();

	float newWidth = kDashBarSize.x * progress_;

	// ワールド座標をスクリーン座標に変換
	Cygnus::Float3 screenPos = Utility::WorldToScreen(player->GetTranslate());
	// スプライトの位置を計算
	Cygnus::Float2 spritePos = {screenPos.x - (kDashBarSize.x / 2.0f), screenPos.y - kDashBarOffsetY};

	spriteDashBar_->SetSize({newWidth, kDashBarSize.y}); // ダッシュクールダウン残り時間に応じてサイズ変更
	spriteDashBar_->SetPosition(spritePos);
	spriteDashBar_->Update();
}

void DashBar::Draw() {
	// クールダウン中でないなら描画スキップ
	if (progress_ <= 0.0f) {
		return;
	}

	spriteDashBar_->Draw();
}