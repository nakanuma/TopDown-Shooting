#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Title/UI/Parts/TitleLogo.h>
#include <src/Game/Title/UI/Parts/StartButton.h>
#include <src/Game/Title/UI/Parts/ExitButton.h>
#include <src/Game/Utility/UI/MouseCursor.h>

// =========================================================
// タイトルシーンUI管理クラス
// =========================================================
class TitleUIManager
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// スタートボタンがクリックされたかを取得
	/// </summary>
	/// <returns></returns>
	bool IsClickedStartButton() const { return startButton_->IsClicked(); }

	/// <summary>
	/// 終了ボタンがクリックされたかを取得
	/// </summary>
	/// <returns></returns>
	bool IsClickedExitButton() const { return exitButton_->IsClicked(); }

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_;	/* スプライト共通処理 */

	std::unique_ptr<TitleLogo> titleLogo_;		/* タイトルロゴ */
	std::unique_ptr<StartButton> startButton_;	/* スタートボタン */
	std::unique_ptr<ExitButton> exitButton_;	/* 終了ボタン */
	std::unique_ptr<MouseCursor> cursor_;		/* マウスカーソル */
};