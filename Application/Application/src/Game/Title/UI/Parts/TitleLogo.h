#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// タイトルロゴクラス
// =========================================================
class TitleLogo
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="spriteCommon">スプライト基盤機能</param>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

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
	static constexpr Cygnus::Float2 kTitlePosition = { 640.0f, 140.0f };		/* タイトル位置 */
	static constexpr Cygnus::Float2 kAnchorPoint = { 0.5f, 0.5f };				/* アンカーポイント（中心） */

	static constexpr float kTitleFloatSpeed = 1.2f;								/* sinfの係数 */
	static constexpr float kTitleFloatAmplitude = 5.0f;							/* sinfの結果にかける係数（移動量） */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> sprite_; /* スプライト */
};