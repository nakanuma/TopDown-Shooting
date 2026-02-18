#include "EnemyDetectionIcon.h"

// Engine
#include <TextureManager.h>
#include <TimeManager.h>

// Application
#include <src/Game/Utility/Utility.h>

void EnemyDetectionIcon::Initialize(Cygnus::SpriteCommon* spriteCommon) {
	// 発見アイコンスプライト生成
	uint32_t texture = Cygnus::TextureManager::Load("UI/enemy_detectionIcon.png");
	spriteDetectionIcon_ = std::make_unique<Cygnus::Sprite>();
	spriteDetectionIcon_->Initialize(spriteCommon, texture);
	spriteDetectionIcon_->SetAnchorPoint({0.5f, 0.5f});
}

void EnemyDetectionIcon::Update(const Cygnus::Float3& worldPos) {
	if (!isActive_) return; // アニメーション中でなければスキップ

	// タイマー更新
	timer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 点滅アニメーション処理
	float alpha = 1.0f;
	// 1.点滅
	if(timer_ < kFlashDuration) {
		// 点滅時間ごとに表示/非表示を切り替え
		uint32_t blinkCount = static_cast<uint32_t>(timer_ / kBlinkInterval);
		alpha = (blinkCount % 2 == 0) ? 1.0f : 0.0f;
	// 2.表示状態を維持
	} else if(timer_ < kFlashDuration + kHoldDuration) {
		alpha = 1.0f;
	// 3.フェードで徐々に非表示
	} else {
		float fadeTimer = timer_ - (kFlashDuration + kHoldDuration);
		alpha = 1.0f - (fadeTimer / kFadeDuration);
	}

	// ワールド座標をスクリーン座標に変換
	Cygnus::Float3 screenPos = Utility::WorldToScreen(worldPos);
	// スプライトの位置を計算
	Cygnus::Float2 spritePos = {screenPos.x, screenPos.y - kDetectionIconOffsetY};	/* オフセット分上にずらす */

	// 発見アイコンスプライト更新
	spriteDetectionIcon_->SetPosition(spritePos);
	spriteDetectionIcon_->SetColor({1.0f, 1.0f, 1.0f, alpha});
	spriteDetectionIcon_->Update();
}

void EnemyDetectionIcon::Draw() {
	if(!isActive_) return; // アニメーション中でなければスキップ

	// 発見アイコンスプライト描画
	spriteDetectionIcon_->Draw();
}

void EnemyDetectionIcon::Play() {
	isActive_ = true;
	timer_ = 0.0f;
}