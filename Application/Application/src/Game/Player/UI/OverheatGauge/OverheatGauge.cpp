#include "OverheatGauge.h"

// C++
#include <numbers>

// Engine
#include <Engine/3D/Camera.h>
#include <TimeManager.h>
#include <Easing.h>

// Application
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>

void OverheatGauge::Initialize(Cygnus::DirectXBase* dxBase, Cygnus::SpriteCommon* spriteCommon) {
	uint32_t texture = Cygnus::TextureManager::Load("UI/ringGauge.png");

	// スプライト生成
	sprite_ = std::make_unique<Cygnus::Sprite>();
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

	Cygnus::Float3 screenPos = Cygnus::Float3::Transform(Utility::CalculateCursorPosition(), Cygnus::Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Cygnus::Window::GetWidth());
	float screenHeight = static_cast<float>(Cygnus::Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	sprite_->SetPosition({ screenX, screenY });

	///
	///	割合に応じて色を変更（緑->黄->橙->赤）
	///

	Cygnus::Float3 color;
	// 緑->黄
	if (overheatRatio < kColorPhase1End) {
		color = Cygnus::Float3::Lerp(kColorGreen, kColorYellow, overheatRatio / kColorPhase1Duration);
		// 黄->橙
	} else if (overheatRatio < kColorPhase2End) {
		color = Cygnus::Float3::Lerp(kColorYellow, kColorOrange, (overheatRatio - kColorPhase1End) / kColorPhase2Duration);
		// 橙->赤
	} else if (overheatRatio < kColorPhase3End) {
		color = Cygnus::Float3::Lerp(kColorOrange, kColorRed, (overheatRatio - kColorPhase2End) / kColorPhase3Duration);
		// 赤
	} else {
		color = kColorRed;
	}

	///
	///	オーバーヒート中の点滅処理
	/// 
	
	float alpha = 1.0f;
	if (player->IsOverHeadted()) {
		overheatBlinkTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		float blink = (std::sinf(overheatBlinkTimer_ * kOverheatedBlinkSpeed) * 0.5f) + 0.5f; // sinで0~1の波を作る

		alpha = Cygnus::Easing::Lerp(kBlinkingAlphaMin, kBlinkingAlphaMax, blink); // 点滅中のアルファ値を最低値と最大値で行き来させる
	} else {
		// オーバーヒート中でなければタイマーリセット
		overheatBlinkTimer_ = 0.0f;
	}

	sprite_->SetColor({ color.x, color.y, color.z, alpha });
}

void OverheatGauge::Draw() { sprite_->Draw(); }