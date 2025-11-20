#include "SplitBlockTransition.h"

// Engine
#include <Easing.h>
#include <RandomGenerator.h>
#include <TextureManager.h>
#include <TimeManager.h>
#include <Window/MyWindow.h>

SplitBlockTransition* SplitBlockTransition::GetInstance() {
	static SplitBlockTransition instance;
	return &instance;
}

void SplitBlockTransition::Initialize(SpriteCommon* spriteCommon, uint32_t splitCount) {
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(DirectXBase::GetInstance());

	splitCount_ = splitCount;
	blocks_.clear();

	// 画面サイズ
	const float screenWidth = static_cast<float>(Window::GetWidth());
	const float screenHeight = static_cast<float>(Window::GetHeight());
	const float blockWidth = screenWidth / splitCount_;
	const float blockHeight = screenHeight / 2.0f; // 画面縦サイズの半分

	uint32_t topRect = TextureManager::Load("UI/topRect.png");
	uint32_t bottomRect = TextureManager::Load("UI/bottomRect.png");

	// カウント数だけスプライトを生成
	for (uint32_t i = 0; i < splitCount_; ++i) {

		Block block;
		block.top = std::make_unique<Sprite>();
		block.bottom = std::make_unique<Sprite>();

		block.top->Initialize(spriteCommon_.get(), topRect);       // 上側のテクスチャを割り当て
		block.bottom->Initialize(spriteCommon_.get(), bottomRect); // 下側のテクスチャを割り当て

		block.top->SetSize({blockWidth, blockHeight});
		block.bottom->SetSize({blockWidth, blockHeight});

		block.top->SetPosition({i * blockWidth, -blockHeight});
		block.bottom->SetPosition({i * blockWidth, screenHeight});

		// 遅延時間を設定
		block.delay = i * kOpenBlockDelay;
		block.progress = 0.0f;

		blocks_.emplace_back(std::move(block));
	}
}

void SplitBlockTransition::StartOpen(float duration, float delayBeforeStart) {
	// 各種パラメーターを開始状態に設定
	state_ = State::Open;
	duration_ = duration;
	timer_ = 0.0f;
	delayBeforeFadeIn_ = delayBeforeStart;

	for (auto& block : blocks_) {
		block.progress = 0.0f;
	}
}

void SplitBlockTransition::StartClose(float duration, std::function<void()> onComplete, float delayAfterComplete) {
	// 各種パラメーターを開始状態に設定
	state_ = State::Close;
	duration_ = duration;
	timer_ = 0.0f;
	delayAfterFadeOutComplete_ = delayAfterComplete;
	delayTimerAfterFadeOut_ = 0.0f;
	onFadeComplete_ = onComplete;

	for (auto& block : blocks_) {
		block.progress = 0.0f;
	}
}

void SplitBlockTransition::Update() {
	if (state_ == State::None)
		return;

	timer_ += TimeManager::GetInstance()->GetDeltaTime();

	// 開くトランジション開始前の遅延時間中はスキップ
	if (state_ == State::Open && timer_ < delayBeforeFadeIn_) {
		return;
	}

	// 全てのブロックのアニメーションが終了したかのフラグ
	bool allFinished = true;

	// 各ブロックの進行度を更新
	for (auto& block : blocks_) {
		float localTime = timer_ - block.delay - delayBeforeFadeIn_;
		if (localTime < 0.0f) {
			allFinished = false;
			continue;
		}

		float t = std::clamp(localTime / duration_, 0.0f, 1.0f);
		block.progress = t;

		if (t < 1.0f) {
			allFinished = false;
		}

		block.top->Update();
		block.bottom->Update();
	}

	// 全ブロックのアニメーションが完了したら現在の状態を切り替え
	if (allFinished) {
		if (state_ == State::Close) {
			delayTimerAfterFadeOut_ += TimeManager::GetInstance()->GetDeltaTime();
			// 閉じるトランジション完了後の遅延時間が終わったら未実行状態へ
			if (delayTimerAfterFadeOut_ >= delayAfterFadeOutComplete_) {
				state_ = State::None;
				if (onFadeComplete_)
					onFadeComplete_();
			}
		} else {
			state_ = State::None;
		}
	}
}

void SplitBlockTransition::Draw() {
	if (state_ == State::None)
		return;

	const float screenHeight = static_cast<float>(Window::GetHeight());

	// 全てのブロックを描画
	for (auto& block : blocks_) {
		float t = block.progress;
		float easedT;

		float topX = block.top->GetPosition().x;
		float bottomX = block.bottom->GetPosition().x;

		float topY, bottomY;

		// 開く（中央から外側へ）
		if (state_ == State::Open) {
			easedT = Easing::EaseInOutExpo(t);

			topY = std::round(-block.top->GetSize().y * easedT);
			bottomY = std::round(screenHeight - block.bottom->GetSize().y * (1.0f - easedT));
			// 閉じる（外側から中央へ）
		} else {
			easedT = Easing::EaseOutBounce(t);

			topY = std::round(-block.top->GetSize().y * (1.0f - easedT));
			bottomY = std::round(screenHeight - block.bottom->GetSize().y * easedT);
		}
		block.top->SetPosition({topX, topY});
		block.bottom->SetPosition({bottomX, bottomY});

		block.top->Draw();
		block.bottom->Draw();
	}
}