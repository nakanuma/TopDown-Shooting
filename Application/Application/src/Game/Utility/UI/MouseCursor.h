#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 汎用的なカーソルUIを表示するクラス
// タイトル・ポーズ・リザルトで使用
// =========================================================
class MouseCursor {
public:
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

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kNormalScale = 1.0f;	/* 通常サイズ */
	static constexpr float kClickScale = 0.85f;	/* 左クリック押下時サイズ */
	static constexpr float kAnimSpeed = 0.5f;	/* アニメーション速度 */
	static constexpr Cygnus::Float4 kNormalColor = {1.0f, 1.0f, 1.0f, 1.0f};	/* 通常色 */
	static constexpr Cygnus::Float4 kClickColor = { 0.5f, 0.5f, 0.5f, 0.5f };	/* 左クリック押下時色 */

	// =========================================================
	// Member Variables
	// =========================================================

	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_; /* スプライト共通処理 */
	std::unique_ptr<Cygnus::Sprite> spriteCursor_; /* カーソル用スプライト */

	Cygnus::Float2 baseSize_;		/* 元サイズ保持用 */
	float currentScale_ = 1.0f;		/* 現在のサイズ倍率 */
	Cygnus::Float4 currentColor;	/* 現在の色 */
};
