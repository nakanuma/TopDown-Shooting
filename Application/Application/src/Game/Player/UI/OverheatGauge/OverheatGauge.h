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
// プレイヤーのオーバーヒートの円ゲージUIクラス
// =========================================================
class OverheatGauge {
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
	static constexpr Cygnus::Float2 kAnchorPoint = { 0.5f, 0.5f };			/* アンカーポイント（中心） */
	static constexpr Cygnus::Float2 kInitialPosition = { 640.0f, 360.0f };	/* 初期位置 */

	static constexpr float kColorPhase1End = 0.25f;			/* 色変更フェーズ1終了（緑->黄） */
	static constexpr float kColorPhase2End = 0.5f;			/* 色変更フェーズ2終了（黄->橙） */
	static constexpr float kColorPhase3End = 0.75f;			/* 色変更フェーズ3終了（橙->赤） */
	static constexpr float kColorPhase1Duration = 0.25f;	/* 色変更フェーズ1の期間 */
	static constexpr float kColorPhase2Duration = 0.25f;	/* 色変更フェーズ2の期間 */
	static constexpr float kColorPhase3Duration = 0.25f;	/* 色変更フェーズ3の期間 */

	static constexpr Cygnus::Float3 kColorGreen = { 0.5f, 1.0f, 0.0f };		/* 緑色 */
	static constexpr Cygnus::Float3 kColorYellow = { 1.0f, 1.0f, 0.0f };	/* 黄色 */
	static constexpr Cygnus::Float3 kColorOrange = { 1.0f, 0.5f, 0.0f };	/* 橙色 */
	static constexpr Cygnus::Float3 kColorRed = { 1.0f, 0.0f, 0.0f };		/* 赤色 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Sprite> sprite_; /* ゲージ用スプライト */
};
