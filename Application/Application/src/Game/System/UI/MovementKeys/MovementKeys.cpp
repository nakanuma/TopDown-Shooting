#include "MovementKeys.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <TimeManager.h>
#include <Easing.h>
#include <Input/Input.h>

// Application
#include <src/Game/Player/Player.h>

void MovementKeys::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	uint32_t texWhite = Cygnus::TextureManager::Load("white.png");
	uint32_t texWKey = Cygnus::TextureManager::Load("UI/key_w.png");
	uint32_t texAKey = Cygnus::TextureManager::Load("UI/key_a.png");
	uint32_t texSKey = Cygnus::TextureManager::Load("UI/key_s.png");
	uint32_t texDKey = Cygnus::TextureManager::Load("UI/key_d.png");
	uint32_t texSpaceKey = Cygnus::TextureManager::Load("UI/key_space.png");
	uint32_t texMoveText = Cygnus::TextureManager::Load("UI/moveText.png");

	// 各キーの初期設定
	SetUpKey(spriteCommon, keyW_, texWKey, kWKeyInitPos, { 0.0f, -1.0f });
	SetUpKey(spriteCommon, keyA_, texAKey, kAKeyInitPos, { -1.0f, 0.0f });
	SetUpKey(spriteCommon, keyS_, texSKey, kSKeyInitPos, { 0.0f, 1.0f });
	SetUpKey(spriteCommon, keyD_, texDKey, kDKeyInitPos, { 1.0f, 0.0f });
	SetUpKey(spriteCommon, keySpace_, texSpaceKey, kSpaceKeyInitPos, { 0.0f, 0.0f });
	keySpace_.sprite->SetAnchorPoint({0.0f, 0.5f});

	// SPACEキーに重ねる半透明スプライト
	spriteSpaceOverlay_ = std::make_unique<Cygnus::Sprite>();
	spriteSpaceOverlay_->Initialize(spriteCommon, texWhite);
	spriteSpaceOverlay_->SetAnchorPoint({ 0.0f, 0.5f });
	spriteSpaceOverlay_->SetPosition(kSpaceKeyInitPos);
	spriteSpaceOverlay_->SetColor({ 0.0f, 0.0f, 0.0f, 0.75f });
	spriteSpaceOverlay_->SetSize(keySpace_.sprite->GetSize());

	// SPACEキーの座標補正
	Cygnus::Float2 spaceLeftPos = {kSpaceKeyInitPos.x - (kSpaceKeySize.x / 2.0f), kSpaceKeyInitPos.y};
	keySpace_.initPos = spaceLeftPos;
	keySpace_.sprite->SetPosition(spaceLeftPos);
	spriteSpaceOverlay_->SetPosition(spaceLeftPos);

	// MOVE文字スプライト
	spriteMoveText_ = std::make_unique<Cygnus::Sprite>();
	spriteMoveText_->Initialize(spriteCommon, texMoveText);
	spriteMoveText_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteMoveText_->SetPosition(kMoveTextInitPos);
}

void MovementKeys::Update(const Player* player) {
	auto input = Cygnus::Input::GetInstance();
	float dt = Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	// 揺れ計算用タイマー更新
	shakeTimer_ += dt;

	// 各移動キーの更新
	UpdateAnimation(keyW_, input->PushKey(DIK_W));
	UpdateAnimation(keyA_, input->PushKey(DIK_A));
	UpdateAnimation(keyS_, input->PushKey(DIK_S));
	UpdateAnimation(keyD_, input->PushKey(DIK_D));

#pragma region SPACEキー関連
	// SPACEキーのみ特別な更新処理
	bool isSpacePressed = input->PushKey(DIK_SPACE);
	float cooldownRatio = player->GetDashCooldownRatio();
	isDashCoolingDown_ = player->GetDashCooldownRatio() > 0.0f; // クールタイムが残っていればtrue

	UpdateAnimation(keySpace_, isSpacePressed); // 通常のアニメーション処理

	// 揺れとオーバーレイの反映
	float offset = 0.0f;
	if(isSpacePressed && isDashCoolingDown_) {
		offset = std::sinf(shakeTimer_ * kShakeSpeed) * kShakeIntensity;
	}

	Cygnus::Float2 basePos = keySpace_.sprite->GetPosition();
	Cygnus::Float2 finalPos = {basePos.x + offset, basePos.y};

	// 本体に反映
	keySpace_.sprite->SetPosition(finalPos);
	keySpace_.sprite->Update();

	// オーバーレイに反映
	spriteSpaceOverlay_->SetSize({kSpaceKeySize.x * cooldownRatio, kSpaceKeySize.y});
	spriteSpaceOverlay_->SetPosition(finalPos);
	spriteSpaceOverlay_->Update();
#pragma endregion

	spriteMoveText_->Update();
}

void MovementKeys::Draw() {
	keyW_.sprite->Draw();
	keyA_.sprite->Draw();
	keyS_.sprite->Draw();
	keyD_.sprite->Draw();
	keySpace_.sprite->Draw();

	// オーバーレイはクールダウン中のみ描画
	if (isDashCoolingDown_) {
		spriteSpaceOverlay_->Draw();
	}

	spriteMoveText_->Draw();
}

void MovementKeys::SetUpKey(Cygnus::SpriteCommon* spriteCommon, KeyUI& key, uint32_t textureHandle, Cygnus::Float2 initPos, Cygnus::Float2 dir) {
	key.sprite = std::make_unique<Cygnus::Sprite>();
	key.sprite->Initialize(spriteCommon, textureHandle);
	key.sprite->SetAnchorPoint({ 0.5f, 0.5f });
	key.initPos = initPos;
	key.offsetDir = dir;
	key.sprite->SetPosition(initPos);
}

void MovementKeys::UpdateAnimation(KeyUI& key, bool isPressed) {
	float dt = Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// キー入力が行われていればタイマーを増加
	if (isPressed) {
		key.animationT += kAnimSpeed * dt;
		// キー入力が行われていないならタイマーを減少
	} else {
		key.animationT -= kAnimSpeed * dt;
	}
	// 範囲制限
	key.animationT = std::clamp(key.animationT, 0.0f, 1.0f);

	// イージング適用
	float easedT = Cygnus::Easing::EaseInOutQuad(key.animationT);

	///
	///	位置のアニメーション処理
	/// 

	// 座標を計算
	Cygnus::Float2 newPos = {
		key.initPos.x + (key.offsetDir.x * kMoveDistance * easedT),
		key.initPos.y + (key.offsetDir.y * kMoveDistance * easedT)
	};

	// 座標を反映
	key.sprite->SetPosition(newPos);

	///
	///	色のアニメーション処理
	/// 

	// 新しい色を計算
	Cygnus::Float4 newColor;
	newColor = kDefaultColor + (kPushedColor - kDefaultColor) * easedT;
	// 色を適用
	key.sprite->SetColor(newColor);

	// スプライト更新
	key.sprite->Update();
}
