#include "OverheatGauge.h"

// C++
#include <numbers>

// Engine
#include <Engine/3D/Camera.h>

// Application
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>

void OverheatGauge::Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon) {
	uint32_t texture = TextureManager::Load("UI/ringGauge.png");

	// スプライト生成
	sprite_ = std::make_unique<Sprite>();
	sprite_->Initialize(spriteCommon, texture);
	sprite_->SetAnchorPoint(kAnchorPoint);
	sprite_->SetPosition(kInitialPosition);
	sprite_->materialData_->useCircleMask = true;
}

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

	Float3 screenPos = Float3::Transform(Utility::CalculateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	sprite_->SetPosition({ screenX, screenY });

	///
	///	割合に応じて色を変更（緑->黄->橙->赤）
	///

	Float3 color;
	// 緑->黄
	if (overheatRatio < kColorPhase1End) {
		color = Float3::Lerp(kColorGreen, kColorYellow, overheatRatio / kColorPhase1Duration);
		// 黄->橙
	} else if (overheatRatio < kColorPhase2End) {
		color = Float3::Lerp(kColorYellow, kColorOrange, (overheatRatio - kColorPhase1End) / kColorPhase2Duration);
		// 橙->赤
	} else if (overheatRatio < kColorPhase3End) {
		color = Float3::Lerp(kColorOrange, kColorRed, (overheatRatio - kColorPhase2End) / kColorPhase3Duration);
		// 赤
	} else {
		color = kColorRed;
	}

	sprite_->SetColor({ color.x, color.y, color.z, 1.0f });
}

void OverheatGauge::Draw() { sprite_->Draw(); }