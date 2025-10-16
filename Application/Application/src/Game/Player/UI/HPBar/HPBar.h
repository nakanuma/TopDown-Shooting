#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// プレイヤーのHPバーUIクラス
// =========================================================
class HPBar {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// UIの初期化処理を行います。
	/// </summary>
	/// <param name="dxBase">DirectX基盤クラス</param>
	/// <param name="spriteCommon">スプライト共通クラス</param>
	void Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update(const Player* player);

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	// HPバーの最大サイズ
	const Float2 kHPBarSize = {300.0f, 30.0f};					/* HPバーの最大サイズ */
	const Float2 kHPBarPosition = {
	    /* HPバーの位置 */
	    Window::GetWidth() / 2.0f - kHPBarSize.x / 2.0f,		/* 画面の中央 - サイズ半分で中央揃え */
	    (Window::GetHeight() / 8.0f) * 7.0f						/* 画面縦サイズの 7/8 の位置へ設定 */
	};

	// ----- Sprite -----
	std::unique_ptr<Sprite> spriteHPBackground_;				/* HPバースプライト（後景） */
	std::unique_ptr<Sprite> spriteHPForeground_;				/* HPバースプライト（前景） */
};