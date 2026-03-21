#include "Reticle.h"

// C++
#include <algorithm>

// Engine
#include <Engine/3D/Camera.h>
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>

void Reticle::Initialize(Cygnus::DirectXBase* dxBase, Cygnus::SpriteCommon* spriteCommon) {
	// レティクル生成
	uint32_t texture = Cygnus::TextureManager::Load("UI/reticleLine.png");
	for(int i = 0; i < 4; ++i) {
		sprites_[i] = std::make_unique<Cygnus::Sprite>();
		sprites_[i]->Initialize(spriteCommon, texture);
		sprites_[i]->SetAnchorPoint({0.5f, 0.5f});
	}

	// 各方向へ回転
	sprites_[0]->SetRotation(0.0f);
	sprites_[1]->SetRotation(Cygnus::PIf);
	sprites_[2]->SetRotation(-Cygnus::PIf / 2.0f);
	sprites_[3]->SetRotation(Cygnus::PIf / 2.0f);

	currentOffset_ = kBaseOffset;
}

void Reticle::Update(const Player* player) {
	// 目標のオフセットを決定
	float targetOffset = kBaseOffset;
	if(player->GetIsMoving()) { 
		// ダッシュ中ならオフセットに加算（レティクルが外側に開くように）
		targetOffset = kBaseOffset + kMovementScale;
	}

	// 線形補間でオフセット計算
	currentOffset_ += (targetOffset - currentOffset_) * kAnimSpeed;

	// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定
	Cygnus::Float3 screenPos = Cygnus::Float3::Transform(
		Utility::CalculateCursorPosition(), 
		Cygnus::Camera::GetCurrent()->GetViewProjectionMatrix()
	);

	float screenWidth = static_cast<float>(Cygnus::Window::GetWidth());
	float screenHeight = static_cast<float>(Cygnus::Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	// クライアント領域内に収める（画面外へ出たら端へ沿わせる）
	screenX = std::clamp(screenX, 0.0f, screenWidth);
	screenY = std::clamp(screenY, 0.0f, screenHeight);

	// 4つのパーツを配置
	sprites_[0]->SetPosition({screenX, screenY - currentOffset_});
	sprites_[1]->SetPosition({ screenX, screenY + currentOffset_});
	sprites_[2]->SetPosition({ screenX - currentOffset_, screenY });
	sprites_[3]->SetPosition({ screenX + currentOffset_, screenY });

	// スプライト更新
	for(auto& sprite : sprites_) {
		sprite->Update();
	}
}

void Reticle::Draw() {
	// スプライト描画
	for (auto& sprite : sprites_) {
		sprite->Draw();
	}
}
