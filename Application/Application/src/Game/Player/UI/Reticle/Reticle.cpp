#include "Reticle.h"

// C++
#include <algorithm>

// Engine
#include <Engine/3D/Camera.h>
#include <Engine/Texture/TextureManager.h>

// Application
#include <src/Game/Utility/Utility.h>

void Reticle::Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon) {
	///
	///	クロスへア（十字線）
	///

	uint32_t textureTarget = TextureManager::Load("UI/crosshair.png");
	spriteCrosshair_ = std::make_unique<Sprite>();
	spriteCrosshair_->Initialize(spriteCommon, textureTarget);
	spriteCrosshair_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteCrosshair_->SetSize({ 64.0f, 64.0f });
}

void Reticle::Update() {
	///
	///	クロスヘア（十字線）
	///

	spriteCrosshair_->Update();

	// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定
	Float3 screenPos = Float3::Transform(Utility::CalculateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	// クライアント領域内に収める（画面外へ出たら端へ沿わせる）
	screenX = std::clamp(screenX, 0.0f, screenWidth);
	screenY = std::clamp(screenY, 0.0f, screenHeight);

	spriteCrosshair_->SetPosition({ screenX, screenY });
}

void Reticle::Draw() {
	///
	///	クロスヘア（十字線）
	///

	spriteCrosshair_->Draw();
}
