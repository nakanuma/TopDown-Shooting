#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// マウスUIクラス
// =========================================================
class Mouse {
private:
	/// <summary>
	/// マウスのパーツ構造体
	/// </summary>
	struct MousePart {
		std::unique_ptr<Cygnus::Sprite> sprite; /* スプライト */
		float animationT = 0.0f;                /* アニメーションタイマー */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// UIの初期化処理を行います。
	/// </summary>
	/// <param name="dxBase">DirectX基盤クラス</param>
	/// <param name="spriteCommon">スプライト共通クラス</param>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// マウスパーツの初期設定を行います。
	/// </summary>
	/// <param name="spriteCommon">スプライト基盤機能</param>
	/// <param name="part">マウスパーツ構造体</param>
	/// <param name="texture">テクスチャ</param>
	void SetUpPart(Cygnus::SpriteCommon* spriteCommon, MousePart& part, uint32_t texture);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kInitMousePos = {1035.0f, 590.0f}; /* 初期位置 */
	static constexpr float kMoveSensitivity = 0.5f;                    /* マウス移動の反映量 */
	static constexpr float kMaxOffset = 40.0f;                         /* 移動限界（ピクセル） */
	static constexpr float kReturnSpeed = 0.2f;                        /* 初期位置に戻る速度 */

	static constexpr Cygnus::Float4 kDefaultColor = {1.0f, 1.0f, 1.0f, 1.0f}; /* 通常色 */
	static constexpr Cygnus::Float4 kPushedColor = {1.0f, 0.25f, 0.0f, 1.0f}; /* 押されている場合の色 */

	static constexpr float kColorAnimSpeed = 10.0f; /* 色の変化速度 */

	// =========================================================
	// Member Variables
	// =========================================================
	MousePart body_;	/* 本体パーツ */
	MousePart middle_;	/* 中クリックパーツ */
	MousePart left_;	/* 左クリックパーツ */
	MousePart right_;	/* 右クリックパーツ */
	
	MousePart shootText_; /* SHOOTの文字 */

	std::unique_ptr<Cygnus::Sprite> spriteBackGround_; /* 背景 */

	Cygnus::Float2 offset_ = {0.0f, 0.0f}; /* 現在の初期位置からのズレ量 */
};
