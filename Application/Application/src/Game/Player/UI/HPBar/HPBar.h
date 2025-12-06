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
	void Initialize(Cygnus::DirectXBase* dxBase, Cygnus::SpriteCommon* spriteCommon);

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
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kHPBarSize = { 300.0f, 30.0f };		 /* HPバーの最大サイズ */
	static constexpr Cygnus::Float2 kHPBarPosition = { 490.0f, 630.0f }; /* HPバーの位置（中央） */

	static constexpr Cygnus::Float4 kHPBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f }; /* HPバー背景色（黒） */
	static constexpr Cygnus::Float4 kHPForegroundColor = { 1.0f, 0.2f, 0.2f, 1.0f }; /* HPバー背景色（黒） */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> spriteHPBackground_;				/* HPバースプライト（後景） */
	std::unique_ptr<Cygnus::Sprite> spriteHPForeground_;				/* HPバースプライト（前景） */
};