#include "Mouse.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <Input/Input.h>
#include <TimeManager.h>
#include <Easing.h>

void Mouse::Initialize(Cygnus::SpriteCommon* spriteCommon) { 
	uint32_t texBody = Cygnus::TextureManager::Load("UI/mouse_body.png");
	uint32_t texMiddle = Cygnus::TextureManager::Load("UI/mouse_middle.png");
	uint32_t texLeft = Cygnus::TextureManager::Load("UI/mouse_left.png");
	uint32_t texRight = Cygnus::TextureManager::Load("UI/mouse_right.png");

	uint32_t texShootText_ = Cygnus::TextureManager::Load("UI/shootText.png");

	uint32_t texBackGround = Cygnus::TextureManager::Load("UI/mouse_backGround.png");

	// 各パーツの初期設定
	SetUpPart(spriteCommon, body_, texBody);
	SetUpPart(spriteCommon, middle_, texMiddle);
	SetUpPart(spriteCommon, left_, texLeft);
	SetUpPart(spriteCommon, right_, texRight);

	SetUpPart(spriteCommon, shootText_, texShootText_);

	spriteBackGround_ = std::make_unique<Cygnus::Sprite>();
	spriteBackGround_->Initialize(spriteCommon, texBackGround);
	spriteBackGround_->SetAnchorPoint({0.5f, 0.5f});
	spriteBackGround_->SetPosition(kInitMousePos);
}

void Mouse::Update() { 
	auto input = Cygnus::Input::GetInstance();
	float dt = Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	POINT move = input->GetMouseMove();

	// マウスの移動量を現在のオフセットに加算
	offset_.x += static_cast<float>(move.x) * kMoveSensitivity;
	offset_.y += static_cast<float>(move.y) * kMoveSensitivity;

	// 移動限界の設定
	offset_.x = std::clamp(offset_.x, -kMaxOffset, kMaxOffset);
	offset_.y = std::clamp(offset_.y, -kMaxOffset, kMaxOffset);

	// 常に初期位置に戻ろうとする力を加える
	offset_.x -= offset_.x * kReturnSpeed;
	offset_.y -= offset_.y * kReturnSpeed;

	// 非常に小さい値になったら完全に0にする
	if (std::abs(offset_.x) < 0.01f) offset_.x = 0.0f;
	if (std::abs(offset_.y) < 0.01f) offset_.y = 0.0f;

	// 左クリック入力で色のアニメーション処理
	if (input->IsPressMouse(0)) {
		left_.animationT += kColorAnimSpeed * dt;
	} else {
		left_.animationT -= kColorAnimSpeed * dt;
	}
	left_.animationT = std::clamp(left_.animationT, 0.0f, 1.0f);

	// イージング適用
	float easedT = Cygnus::Easing::EaseInOutQuad(left_.animationT);

	// 色の線形補間
	Cygnus::Float4 newColor;
	newColor = kDefaultColor + (kPushedColor - kDefaultColor) * easedT;
	left_.sprite->SetColor(newColor);

	// 各パーツの座標を更新して反映
	auto updatePart = [&](MousePart& part) { 
		Cygnus::Float2 finalPos = {
			kInitMousePos.x + offset_.x, 
			kInitMousePos.y + offset_.y
		}; 
		part.sprite->SetPosition(finalPos);
		part.sprite->Update();
	};

	updatePart(body_);
	updatePart(middle_);
	updatePart(left_);
	updatePart(right_);

	updatePart(shootText_);

	spriteBackGround_->Update();
}

void Mouse::Draw() { 
	spriteBackGround_->Draw();

	body_.sprite->Draw();
	middle_.sprite->Draw();
	left_.sprite->Draw();
	right_.sprite->Draw();

	shootText_.sprite->Draw();
}

void Mouse::SetUpPart(Cygnus::SpriteCommon* spriteCommon, MousePart& part, uint32_t texture) { 
	part.sprite = std::make_unique<Cygnus::Sprite>(); 
	part.sprite->Initialize(spriteCommon, texture);
	part.sprite->SetAnchorPoint({0.5f, 0.5f});
	part.sprite->SetPosition(kInitMousePos);
}
