#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <functional>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// ポーズメニュークラス
// =========================================================
class PauseMenu
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// UI描画処理を行います。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// ポーズ状態に移行します。
	/// </summary>
	void Show();

	/// <summary>
	/// ポーズ状態を終了します。
	/// </summary>
	void Hide();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 「ゲームを続ける」ボタンを押した際のコールバック関数を設定します。
	/// </summary>
	/// <param name="callback"></param>
	void SetCloseCallback(std::function<void()> callback) { closeCallback_ = callback; }

	/// <summary>
	/// 「タイトルへ戻る」ボタンを押した際のコールバック関数を設定します。
	/// </summary>
	/// <param name="callback"></param>
	void SetTitleCallback(std::function<void()> callback) { titleCallback_ = callback; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 背景の更新処理を行います。
	/// </summary>
	void UpdateBackground();

	/// <summary>
	/// メニュー項目のアニメーション処理を行います。
	/// </summary>
	void UpdateMenuAnimation();

	/// <summary>
	/// メニュー項目のマウス判定処理を行います。
	/// </summary>
	void UpdateButtons();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float4 kBackGroundColor = { 0.0f, 0.0f, 0.0f, 0.5f }; /* 背景色（半透明の黒）*/
	static constexpr float kBackgroundFadeDuration = 0.15f;	/* 背景のフェード速度 */

	static constexpr float kMenuFadeDuration = 0.2f;	/* メニューのフェード速度 */
	static constexpr float kSlideOffset = 100.0f;		/* メニュー項目の右側開始位置オフセット */

	static constexpr float kAnimDelay = 0.2f;	/* アニメーションの遅延時間 */

	static constexpr Cygnus::Float2 kPauseInitPos = {640.0f, 200.0f};		/* ポーズ文字の初期位置 */
	static constexpr Cygnus::Float2 kContinueInitPos = { 640.0f, 350.0f };	/* ゲームを続ける文字の初期位置 */
	static constexpr Cygnus::Float2 kBackToTitleInitPos = {640.0f, 450.0f};	/* タイトルへ戻る文字の初期位置 */

	static constexpr Cygnus::Float4 kColorDefault = { 1.0f, 1.0f, 1.0f, 1.0f };		/* 通常時の色 */
	static constexpr Cygnus::Float4 kColorHover = { 1.0f, 0.25f, 0.0f, 1.0f };		/* ホバー時の色 */
	static constexpr float kScaleDefault = 1.0f;	/* 通常時スケール */
	static constexpr float kScaleHover = 1.1f;		/* ホバー時スケール */

	// =========================================================
	// Member Variables
	// =========================================================
	
	// ----- Sprite -----
	std::unique_ptr<Cygnus::Sprite> spriteBackGround_;  /* 背景スプライト */
	std::unique_ptr<Cygnus::Sprite> spritePauseText_;	/* ポーズ文字スプライト */
	std::unique_ptr<Cygnus::Sprite> spriteContinue_;	/* ゲームを続けるスプライト */
	std::unique_ptr<Cygnus::Sprite> spriteBackToTitle_;	/* タイトルへ戻るスプライト */

	// ----- Parameters -----
	float backGroundCurrentAlpha_ = 0.0f;	/* 現在のアルファ値 */
	float backGroundTargetAlpha_ = 0.0f;	/* 目標のアルファ値 */

	float animationProgress_ = 0.0f;	/* アニメーションの進行度 */

	bool isShowing_ = false;	/* ポーズメニューの表示が行われているかどうか */

	Cygnus::Float2 sizeContinueBase_ = {0.0f, 0.0f};
	Cygnus::Float2 sizeBackToTitleBase_ = {0.0f, 0.0f};

	std::function<void()> closeCallback_;	/* ゲームに戻る際のコールバック関数 */
	std::function<void()> titleCallback_;	/* タイトルへ戻る際のコールバック関数 */
};


