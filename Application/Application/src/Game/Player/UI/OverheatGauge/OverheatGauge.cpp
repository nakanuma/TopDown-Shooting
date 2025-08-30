#include "OverheatGauge.h"

// C++
#include <numbers>

// Engine
#include <Engine/3D/Camera.h>

// Application
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void OverheatGauge::Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon) {
	uint32_t texture = TextureManager::Load("resources/Images/UI/ringGauge.png", dxBase->GetDevice());

	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(spriteCommon, texture);
	sprite_->SetAnchorPoint({ 0.5f, 0.5f });
	sprite_->SetPosition({ 640.0f, 360.0f });
	sprite_->materialData_->useCircleMask = true;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void OverheatGauge::Update(const Player* player) {
	sprite_->Update();

	///
	/// 割合に応じてマスク処理
	/// 
	
	float overheatRatio = std::clamp(player->GetOverheatRatio(), 0.0f, 1.0f);
	sprite_->materialData_->ratio = overheatRatio;

	///
	/// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定
	/// 

	Float3 screenPos = Float3::Transform(Utility::CalclateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	sprite_->SetPosition({ screenX, screenY });

	///
	///	割合に応じて色を変更（緑->黄->橙->赤）
	/// 
	
	const Float3 green = { 0.5f, 1.0f, 0.0f };
	const Float3 yellow = { 1.0f, 1.0f, 0.0f };
	const Float3 orange = { 1.0f, 0.5f, 0.0f };
	const Float3 red = { 1.0f, 0.0f, 0.0f };

	Float3 color;
	// 緑->黄
	if (overheatRatio < 0.25f) {
		color = Float3::Lerp(green, yellow, overheatRatio / 0.25f);
	// 黄->橙
	} else if (overheatRatio < 0.5f) {
		color = Float3::Lerp(yellow, orange, (overheatRatio - 0.25f) / 0.25f);
	// 橙->赤
	} else if (overheatRatio < 0.75f) {
		color = Float3::Lerp(orange, red, (overheatRatio - 0.5f) / 0.25f);
	// 赤
	} else {
		color = red;
	}

	sprite_->SetColor({ color.x, color.y, color.z, 1.0f });
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void OverheatGauge::Draw() {
	sprite_->Draw();
}