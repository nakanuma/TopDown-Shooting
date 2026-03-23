#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 敵のHPバーUIクラス
// =========================================================
class EnemyHPBar
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
	/// <param name="worldPos">ワールド座標</param>
	/// <param name="hpRatio">HP割合</param>
	void Update(const Cygnus::Float3& worldPos, float hpRatio);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kHPBarSize = { 100.0f, 20.0f };		/* HPバーのサイズ */
	static constexpr float kHPBarOffsetY = 90.0f;						/* HPバーのY軸オフセット */

	static constexpr Cygnus::Float4 kHPBarBackgroundColor = { 0.0f, 0.0f, 0.0f, 1.0f };	/* HPバー背景色 */
	static constexpr Cygnus::Float4 kHPBarForegroundColor = { 0.0f, 1.0f, 0.5f, 1.0f };	/* HPバー前景色 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> spriteHPBackground_; /* HPバー後景スプライト */
	std::unique_ptr<Cygnus::Sprite> spriteHPForeground_; /* HPバー前景スプライト */
};