#pragma once

// C++
#include <functional>
#include <memory>

// Engine
#include <SpriteCommon.h>
#include <Sprite.h>
#include "SplitBlockTransition.h"

/// <summary>
/// フェード
/// </summary>
class FadeTransition
{
public:
	enum class State {
		None,
		FadeIn,
		FadeOut,
	};

	/// <summary>
	/// インスタンス取得
	/// </summary>
	static FadeTransition* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon);

	/// <summary>
	/// フェードイン開始（暗い画面から徐々に見えるように）
	/// </summary>
	void StartFadeIn(float duration, float delayBeforeStart = 0.0f);

	/// <summary>
	/// フェードアウト開始（徐々に暗くなるように）
	/// </summary>
	void StartFadeOut(float duration, std::function<void()> onComplete, float delayAfterComplete = 0.0f);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// フェードが終わったか確認
	/// </summary>
	bool IsFinished() const { return state_ == State::None; }

private:
	State state_ = State::None;
	float alpha_ = 0.0f;
	float duration_ = 1.0f;
	float timer_ = 0.0f;

	float delayBeforeFadeIn_ = 0.0f; // フェードイン開始前の待機時間

	float delayAfterFadeOutComplete_ = 0.0f; // フェードアウト完了後の待機時間
	float delayTimerAfterFadeOut_ = 0.0f; // フェードアウト完了してからの待機タイマー

	std::unique_ptr<Sprite> sprite_;
	std::function<void()> onFadeComplete_;
};

