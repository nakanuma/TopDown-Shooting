#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <array>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// プレイヤーのレティクルUIクラス
// =========================================================
class Reticle {
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
	static constexpr float kBaseOffset = 5.0f;		/* 待機状態での中心からの距離 */
	static constexpr float kMovementScale = 20.0f;	/* 移動速度に応じた最大加算距離 */
	static constexpr float kAnimSpeed = 0.2f;		/* 開閉の滑らかさ */

	// =========================================================
	// Member Variables
	// =========================================================
	std::array<std::unique_ptr<Cygnus::Sprite>, 4> sprites_;	/* スプライト配列 */
	float currentOffset_ = kBaseOffset;	/* 現在のオフセット距離 */
};
