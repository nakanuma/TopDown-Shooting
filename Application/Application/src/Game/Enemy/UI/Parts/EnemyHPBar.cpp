#include "EnemyHPBar.h"

// Engine
#include <TextureManager.h>

// Application
#include <src/Game/Utility/Utility.h>

void EnemyHPBar::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// HPバー後景スプライト生成
	uint32_t textureBackground = Cygnus::TextureManager::Load("white.png");
	spriteHPBackground_ = std::make_unique<Cygnus::Sprite>();
	spriteHPBackground_->Initialize(spriteCommon, textureBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor(kHPBarBackgroundColor);

	// HPバー前景スプライト生成
	uint32_t textureForeground = Cygnus::TextureManager::Load("white.png");
	spriteHPForeground_ = std::make_unique<Cygnus::Sprite>();
	spriteHPForeground_->Initialize(spriteCommon, textureForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor(kHPBarForegroundColor);
}

void EnemyHPBar::Update(const Cygnus::Float3& worldPos, float hpRatio) {
	// ワールド座標をスクリーン座標に変換
	Cygnus::Float3 screenPos = Utility::WorldToScreen(worldPos);
	// スプライトの位置を計算
	Cygnus::Float2 spritePos = {
		screenPos.x - (kHPBarSize.x / 2.0f),	// HPバーが敵の中心位置に来るよう調整
		screenPos.y - kHPBarOffsetY				// 敵に重ならないようオフセット分上側へずらす
	};

	// HPバー後景スプライト更新
	spriteHPBackground_->SetPosition(spritePos);
	spriteHPBackground_->Update();

	// HPバー前景スプライト更新
	spriteHPForeground_->SetSize({kHPBarSize.x * hpRatio, kHPBarSize.y}); // HP割合に応じてサイズ変更して残りHPを表現
	spriteHPForeground_->SetPosition(spritePos);
	spriteHPForeground_->Update();
}

void EnemyHPBar::Draw() {
	// HPバー後景スプライト描画
	spriteHPBackground_->Draw();

	// HPバー前景スプライト描画
	spriteHPForeground_->Draw();
}