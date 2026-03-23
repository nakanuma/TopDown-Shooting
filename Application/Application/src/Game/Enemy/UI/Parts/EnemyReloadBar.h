#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 敵のリロードバーUIクラス
// =========================================================
class EnemyReloadBar
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
	void Update(const Cygnus::Float3 worldPos, float reloadRatio, bool isReloading);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kReloadSize = { 100.0f, 10.0f }; /* リロード表示スプライトのサイズ */
	static constexpr float kReloadBarOffsetY = 60.0f;				 /* リロード表示のY軸オフセット */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> spriteReloadBar_; /* リロードバースプライト */

	bool isVisible_;	/* 表示フラグ */
};

