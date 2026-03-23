#include "EnemyReloadBar.h"

// Engine
#include <TextureManager.h>

// Application
#include <src/Game/Utility/Utility.h>

void EnemyReloadBar::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// リロードバースプライト生成
	uint32_t texture = Cygnus::TextureManager::Load("white.png");
	spriteReloadBar_ = std::make_unique<Cygnus::Sprite>();
	spriteReloadBar_->Initialize(spriteCommon, texture);
	spriteReloadBar_->SetSize(kReloadSize);
}

void EnemyReloadBar::Update(const Cygnus::Float3 worldPos, float reloadRatio, bool isReloading) {
	isVisible_ = isReloading; // 表示フラグの更新
	if(!isVisible_) return; // 表示しないなら計算スキップ

	// ワールド座標をスクリーン座標に変換
	Cygnus::Float3 screenPos = Utility::WorldToScreen(worldPos);
	// スプライトの位置を計算
	Cygnus::Float2 spritePos = {
		screenPos.x - (kReloadSize.x / 2.0f),	// HPバーが敵の中心位置に来るよう調整
		screenPos.y - kReloadBarOffsetY			// 敵に重ならないようオフセット分上側へずらす
	};

	// リロードバースプライト更新
	spriteReloadBar_->SetSize({kReloadSize.x - (kReloadSize.x * reloadRatio), kReloadSize.y}); // リロード残り時間の割合に応じてサイズ変更
	spriteReloadBar_->SetPosition(spritePos);
	spriteReloadBar_->Update();
}

void EnemyReloadBar::Draw() {
	if(!isVisible_) return; // 表示しないなら描画スキップ

	// リロードバースプライト描画
	spriteReloadBar_->Draw();
}