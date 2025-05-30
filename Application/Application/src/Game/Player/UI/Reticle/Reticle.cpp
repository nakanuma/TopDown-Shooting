#include "Reticle.h"

// Engine
#include <Engine/Texture/TextureManager.h>
#include <Engine/3D/Camera.h>

// Application
#include <src/Game/Utility/Utility.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void Reticle::Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon) {
	///
	///	クロスへア（十字線）
	/// 

	uint32_t textureTarget = TextureManager::Load("resources/Images/UI/crosshair.png", dxBase->GetDevice());
	spriteCrosshair_ = std::make_unique<Sprite>();
	spriteCrosshair_->Initialize(spriteCommon, textureTarget);
	spriteCrosshair_->SetAnchorPoint({0.5f, 0.5f});
	spriteCrosshair_->SetSize({64.0f, 64.0f});
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void Reticle::Update() 
{
	///
	///	クロスヘア（十字線）
	/// 

	spriteCrosshair_->Update();

	// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定
	Float3 screenPos = Float3::Transform(Utility::CalclateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	spriteCrosshair_->SetPosition({screenX, screenY});
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void Reticle::Draw() 
{
	///
	///	クロスヘア（十字線）
	/// 
	
	spriteCrosshair_->Draw();
}
