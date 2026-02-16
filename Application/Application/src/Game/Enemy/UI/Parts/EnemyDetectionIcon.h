#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 敵のプレイヤー発見アイコンUIクラス
// =========================================================
class EnemyDetectionIcon
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
	void Update(const Cygnus::Float3& worldPos);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// アニメーション演出を開始します。
	/// </summary>
	void Play();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kDetectionIconOffsetY = 120.0f;

	static constexpr float kFlashDuration = 0.25f;	/* 点滅時間 */
	static constexpr float kHoldDuration = 1.0f;	/* 維持時間 */
	static constexpr float kFadeDuration = 0.2f;	/* フェード時間 */

	static constexpr float kBlinkInterval = 0.025f;	/* 点滅速度 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> spriteDetectionIcon_;	/* 発見アイコンスプライト */

	bool isActive_ = false;	/* アニメーション中か */
	float timer_ = 0.0f;	/* アニメーション用タイマー */
};

