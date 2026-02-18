#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 敵のリロードアイコンUIクラス
// =========================================================
class EnemyReloadIcon {
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
	void Update(const Cygnus::Float3& worldPos, bool isReloading);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kIconOffsetY = 120.0f; /* アイコンのY軸オフセット */

	static constexpr float kStartScaleRatio = 0.8f; /* 開始時の倍率 */
	static constexpr float kEndScaleRatio = 1.0f;   /* 終了時の倍率 */
	static constexpr float kScaleDuration = 0.3f;   /* 拡大にかける時間 */

	static constexpr float kFadeStartDelay = 1.5f; /* フェード開始までの待機時間 */
	static constexpr float kFadeDuration = 0.2f;   /* フェードにかける時間 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> sprite_; /* スプライト */

	Cygnus::Float2 baseSize_ = {0.0f, 0.0f}; /* スプライトの基準サイズ */

	bool isVisible_ = false; /* 表示フラグ */
	float timer_ = 0.0f;     /* アニメーション用タイマー */

	bool wasReloading_ = false; /* 前フレームのリロード状態 */
};
