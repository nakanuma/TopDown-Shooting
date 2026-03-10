#pragma once

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
// プレイヤーのダッシュ時クールタイム表示UIクラス
// =========================================================
class DashBar
{
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
	void Update(const Player* player);

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kDashBarSize = { 80.0f, 10.0f };	/* バーのサイズ */
	static constexpr Cygnus::Float4 kDashBarColor = { 0.25f, 0.5f, 1.0f, 1.0f }; /* バーの色 */

	static constexpr float kDashBarOffsetY = -60.0f;	/* プレイヤーからのY軸オフセット */

	// =========================================================
	// Member Variables
	// =========================================================
	float progress_ = 0.0f; /* クールダウン進捗率 */

	std::unique_ptr<Cygnus::Sprite> spriteDashBar_;	/* クールダウン表示スプライト */
};

