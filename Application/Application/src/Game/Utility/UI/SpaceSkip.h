#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// SPACEキーのスキップUIを表示するクラス
// スタート演出中・ボス登場演出中に使用
// =========================================================
class SpaceSkip {
public:
	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// スキップ完了フラグを取得します。
	/// </summary>
	/// <returns></returns>
	bool IsSkipped() const { return isSkipped_; }

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float2 kSpaceKeyPosition = {1000.0f, 600.0f}; /* SPACEキーの位置 */
	static constexpr Cygnus::Float2 kSkipTextPosition = {1110.0f, 580.0f}; /* SKIPテキストの位置 */

	static constexpr float kSkipHoldTime = 0.5f; /* スキップに要するSPACEキーホールド時間 */

	// =========================================================
	// Member Variables
	// =========================================================

	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_; /* スプライト共通処理 */

	std::unique_ptr<Cygnus::Sprite> spriteSpace_; /* SPACEキースプライト */
	std::unique_ptr<Cygnus::Sprite> spriteSpaceOverlay_; /* SPACEキーオーバーレイスプライト */
	std::unique_ptr<Cygnus::Sprite> spriteSkipText_; /* SKIPテキストスプライト */

	float spaceHoldTimer_ = 0.0f; /* スペース押下時間カウント */
	bool isSkipped_ = false;
};
