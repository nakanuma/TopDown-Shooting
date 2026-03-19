#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 終了ボタン（タイトルシーン）クラス
// =========================================================
class ExitButton
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

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// ボタンがクリックされたかを取得
	/// </summary>
	/// <returns></returns>
	bool IsClicked() const { return isClicked_; }

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kExitButtonPosition = { 640.0f, 600.0f };	/* 終了ボタン位置 */
	static constexpr Cygnus::Float2 kAnchorPoint = { 0.5f, 0.5f };				/* アンカーポイント（中心） */

	// アニメーション用
	static constexpr float kScaleDefault = 1.0f;	/* 通常スケール */
	static constexpr float kScaleHover = 1.2f;		/* 拡大時最大スケール */
	static constexpr Cygnus::Float4 kColorDefault = { 0.8f, 0.8f, 0.8f, 1.0f };	/* 通常色 */
	static constexpr Cygnus::Float4 kColorHover = { 1.0f, 1.0f, 1.0f, 1.0f };	/* ホバー時色 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> sprite_; /* スプライト */

	Cygnus::Float2 baseSize_;	/* スプライト初期サイズ保持用 */
	bool isClicked_ = false;	/* ボタンがクリックされたかのフラグ */
};

