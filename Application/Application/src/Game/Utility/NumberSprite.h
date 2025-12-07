#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 指定した数字をスプライトで表示するクラス
// リザルト表示などで使用
// =========================================================
class NumberSprite {
public:
	/// <summary>
	/// スプライトの初期化処理を行います。
	/// </summary>
	/// <param name="value">表示する数値</param>
	/// <param name="decimalPlaces">桁数</param>
	void Initialize(float value, uint32_t decimalPlaces = 0);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update(Cygnus::Float2 position);

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kDigitSize = {32.0f, 32.0f}; /* 1文字のサイズ */
	static constexpr Cygnus::Float2 kAnchorPoint = {0.5f, 0.5f}; /* アンカーポイント（中心） */

	static constexpr uint32_t kDecimalPointIndex = 10;     /* 小数点のテクスチャインデックス */
	static constexpr float kDecimalPointWidthRatio = 0.2f; /* 小数点の幅比率 */
	static constexpr float kDigitOneWidthRatio = 0.4f;     /* 数字1の幅比率 */
	static constexpr float kDefaultDigitWidthRatio = 0.8f; /* デフォルトの数字幅比率 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_; /* スプライト共通処理 */

	// ----- Parameters -----
	uint32_t digitCount_ = 0;             /* 桁数 */
	std::vector<uint32_t> digitValues_{}; /* 各桁の数字 */

	// ----- Container -----
	std::vector<std::unique_ptr<Cygnus::Sprite>> digits_{}; /* スプライトを格納するコンテナ */
};
