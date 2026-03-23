#include "SpaceSkip.h"

// Engine
#include <TextureManager.h>
#include <Input/Input.h>
#include <TimeManager.h>

void SpaceSkip::Initialize() {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// スプライト基盤生成
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

	// スプライト生成
	uint32_t texSpaceKey = Cygnus::TextureManager::Load("UI/key_space.png");
	spriteSpace_ = std::make_unique<Cygnus::Sprite>();
	spriteSpace_->Initialize(spriteCommon_.get(), texSpaceKey);
	spriteSpace_->SetPosition(kSpaceKeyPosition);

	uint32_t texWhite = Cygnus::TextureManager::Load("white.png");
	spriteSpaceOverlay_ = std::make_unique<Cygnus::Sprite>();
	spriteSpaceOverlay_->Initialize(spriteCommon_.get(), texWhite);
	spriteSpaceOverlay_->SetPosition(kSpaceKeyPosition);
	spriteSpaceOverlay_->SetColor({0.0f, 0.0f, 0.0f, 0.75f}); // 黒半透明
	spriteSpaceOverlay_->SetSize(spriteSpace_->GetSize()); // オーバーレイはSPACEキーのテクスチャサイズに合わせる

	uint32_t texSkipText = Cygnus::TextureManager::Load("UI/skipText.png");
	spriteSkipText_ = std::make_unique<Cygnus::Sprite>();
	spriteSkipText_->Initialize(spriteCommon_.get(), texSkipText);
	spriteSkipText_->SetPosition(kSkipTextPosition);
	spriteSkipText_->SetAnchorPoint({0.5f, 0.5f});
}

void SpaceSkip::Update() {
	// SPACE押下でスキップ
	if (Cygnus::Input::GetInstance()->PushKey(DIK_SPACE)) {
		spaceHoldTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime(); // 押下時間のカウント

		// 指定時間押し続けていたらスキップフラグを立てる
		if (spaceHoldTimer_ >= kSkipHoldTime) {
			isSkipped_ = true;
		}
	} else {
		// 離したらタイマーリセット
		spaceHoldTimer_ = 0.0f;
	}

	// オーバーレイのサイズを進捗率に合わせて変更
	Cygnus::Float2 spaceKeySize = spriteSpace_->GetSize();
	float progress = spaceHoldTimer_ / kSkipHoldTime;
	spriteSpaceOverlay_->SetSize({spaceKeySize.x * progress, spaceKeySize.y});

	spriteSpace_->Update();
	spriteSpaceOverlay_->Update();
	spriteSkipText_->Update();
}

void SpaceSkip::Draw() {
	spriteSpace_->Draw();
	spriteSpaceOverlay_->Draw();
	spriteSkipText_->Draw();
}