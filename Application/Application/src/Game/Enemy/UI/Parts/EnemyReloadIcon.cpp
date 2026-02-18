#include "EnemyReloadIcon.h"

// C++
#include <algorithm>

// Engine
#include <TextureManager.h>
#include <TimeManager.h>
#include <Easing.h>

// Application
#include <src/Game/Utility/Utility.h>

void EnemyReloadIcon::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// スプライト生成
	uint32_t texture = Cygnus::TextureManager::Load("UI/enemy_reloadIcon.png");
	sprite_ = std::make_unique<Cygnus::Sprite>();
	sprite_->Initialize(spriteCommon, texture);
	sprite_->SetAnchorPoint({0.5f, 0.5f});

	baseSize_ = sprite_->GetSize(); // サイズを保存
}

void EnemyReloadIcon::Update(const Cygnus::Float3& worldPos, bool isReloading) { 
	// リロードが行われた瞬間のみタイマーリセット
	if (isReloading && !wasReloading_) {
		timer_ = 0.0f;
	}
	wasReloading_ = isReloading; // 現在の状態を保存

	isVisible_ = isReloading; // 表示フラグの更新
	if (!isVisible_) return; // 表示しないなら計算スキップ

	// タイマー更新
	timer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// スケール変更アニメーション
	float ratio = kEndScaleRatio;
	if (timer_ < kScaleDuration) {
		float t = std::clamp(timer_ / kScaleDuration, 0.0f, 1.0f);
		float easeT = Cygnus::Easing::EaseOutQuad(t);
		ratio = Cygnus::Easing::Lerp(kStartScaleRatio, kEndScaleRatio, easeT);
	}
	Cygnus::Float2 currentSize = baseSize_ * ratio;

	// アルファ値変更アニメーション
	float alpha = 1.0f;
	if (timer_ > kFadeStartDelay) {
		// フェード開始からのタイマーを計算
		float fadeTimer = timer_ - kFadeStartDelay;
		// 設定した時間でAlpha値を透明に
		float t = std::clamp(fadeTimer / kFadeDuration, 0.0f, 1.0f);
		float easeT = Cygnus::Easing::EaseInQuad(t);
		alpha = 1.0f - easeT;
	}

	// ワールド座標をスクリーン座標に変換
	Cygnus::Float3 screenPos = Utility::WorldToScreen(worldPos);
	// スプライトの位置を計算
	Cygnus::Float2 spritePos = {
	    screenPos.x,
	    screenPos.y - kIconOffsetY	// 敵に重ならないようオフセット分上側へずらす
	};

	// スプライト更新
	sprite_->SetPosition(spritePos);
	sprite_->SetSize(currentSize);
	sprite_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	sprite_->Update(); 
}

void EnemyReloadIcon::Draw() { 
	if (!isVisible_) return; // 表示しないなら描画スキップ

	sprite_->Draw(); 
}