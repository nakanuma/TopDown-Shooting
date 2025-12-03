#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <functional>
#include <memory>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "SplitBlockTransition.h"
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// フェードトランジション制御クラス
// =========================================================
class FadeTransition {
public:
	/// <summary>
	/// フェード処理の状態を表す列挙体
	/// </summary>
	enum class State {
		None,    /* 未実行 */
		FadeIn,  /* フェードイン */
		FadeOut, /* フェードアウト */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスの取得を行います。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static FadeTransition* GetInstance();

	/// <summary>
	/// フェードの初期化処理を行います。
	/// </summary>
	/// <param name="spriteCommon">スプライト共通処理クラス</param>
	void Initialize(SpriteCommon* spriteCommon);

	/// <summary>
	/// フェードインを開始します。（暗い画面から徐々に見えるように）
	/// </summary>
	/// <param name="duration">フェード時間</param>
	/// <param name="delayBeforeStart">開始までの遅延時間</param>
	void StartFadeIn(float duration, float delayBeforeStart = 0.0f);

	/// <summary>
	/// フェードアウトを開始します。（徐々に暗くなるように）
	/// </summary>
	/// <param name="duration">フェード時間</param>
	/// <param name="onComplete">完了に実行するコールバック関数</param>
	/// <param name="delayAfterComplete">完了までの遅延時間</param>
	void StartFadeOut(float duration, std::function<void()> onComplete, float delayAfterComplete = 0.0f);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// フェードが完了したかを取得します。
	/// </summary>
	/// <returns>フェード完了フラグ</returns>
	bool IsFinished() const { return state_ == State::None; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// トランジション開始時の共通パラメーターを設定します。
	/// </summary>
	/// <param name="state">状態</param>
	/// <param name="duration">アニメーション時間</param>
	void StartTransitionCommon(State state, float initialAlpha, float duration);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float4 kColorBlack = {0.0f, 0.0f, 0.0f, 1.0f}; /* 黒色 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	State state_ = State::None; /* 現在のフェード状態 */
	float alpha_ = 0.0f;        /* 現在のアルファ値 */
	float duration_ = 0.0f;     /* フェード所要時間 */
	float timer_ = 0.0f;        /* フェードタイマー */

	float delayBeforeFadeIn_ = 0.0f; /* フェードイン開始前の待機時間 */

	float delayAfterFadeOutComplete_ = 0.0f; /* フェードアウト完了後の待機時間 */
	float delayTimerAfterFadeOut_ = 0.0f;    /* フェードアウト完了してからの待機タイマー */

	// ----- Sprite -----
	std::unique_ptr<Sprite> sprite_; /* スプライト */

	// ----- Others -----
	std::function<void()> onFadeComplete_; /* フェード完了後のコールバック関数 */
};
