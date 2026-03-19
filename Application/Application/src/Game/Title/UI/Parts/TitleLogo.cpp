#include "TitleLogo.h"

// Engine
#include <TextureManager.h>
#include <TimeManager.h>

void TitleLogo::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// タイトルロゴスプライト生成
	uint32_t textureTitle = Cygnus::TextureManager::Load("UI/title.png");
	sprite_ = std::make_unique<Cygnus::Sprite>();
	sprite_->Initialize(spriteCommon, textureTitle);
	sprite_->SetPosition(kTitlePosition);
	sprite_->SetAnchorPoint(kAnchorPoint);
}

void TitleLogo::Update() {
	// 上下移動
	static float floatTimer = 0.0f;
	floatTimer += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float floatAmount = sinf(floatTimer * kTitleFloatSpeed) * kTitleFloatAmplitude;
	sprite_->SetPosition({ kTitlePosition.x, kTitlePosition.y + floatAmount });

	// 更新
	sprite_->Update();
}

void TitleLogo::Draw() {
	// 描画
	sprite_->Draw();
}