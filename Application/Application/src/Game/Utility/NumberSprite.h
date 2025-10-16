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
	void Update(Float2 position);

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<SpriteCommon> spriteCommon_;			/* スプライト共通処理 */

	// ----- Parameters -----
	Float2 digitSize_{ 32.0f, 32.0f };						/* 1文字のサイズ */
	uint32_t digitCount_ = 0;								/* 桁数 */
	std::vector<uint32_t> digitValues_{};					/* 各桁の数字 */

	// ----- Container -----
	std::vector<std::unique_ptr<Sprite>> digits_{};			/* スプライトを格納するコンテナ */
};
