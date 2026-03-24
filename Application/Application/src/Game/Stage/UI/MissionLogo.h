#pragma once

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
enum class StageType;

// ---------------------------------------------------------
// Engine includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// ステージ開始/クリア時のテロップ演出クラス
// =========================================================
class MissionLogo
{
public:
	/// <summary>
	/// アニメーションの状態
	/// </summary>
	enum class AnimationState {
		None,				/* 無し */
		StartMission,		/* 開始時 */
		ObjectiveClear		/* クリア時 */
	};

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
	void DrawUI();

	/// <summary>
	/// アニメーションを開始します。
	/// </summary>
	/// <param name="state">状態（開始/クリア）</param>
	/// <param name="type">ステージの目標</param>
	void Start(AnimationState state, StageType type, float delayTime = 0.0f);

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// アニメーション再生中か確認します。
	/// </summary>
	/// <returns></returns>
	bool IsPlaying() const { return currentState_ != AnimationState::None; }

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kDisplayDuration = 2.0f;	/* 表示時間 */

	static constexpr float kFadeDuration = 0.5f;	/* フェードにかける時間 */
	static constexpr float kSlideDistance = 50.0f;	/* アウト時のスライド距離 */
	static constexpr float kScaleStart = 1.5f;		/* 開始時の拡大率 */
	static constexpr Cygnus::Float2 kBasePos = {640.0f, 360.0f};	/* 初期位置 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_;	/* スプライト共通処理 */

	AnimationState currentState_ = AnimationState::None;	/* 現在の状態 */
	StageType stageType_;	/* ステージ目標 */
	float timer_ = 0.0f;	/* タイマー */
	float startDelayTimer_ = 0.0f;	/* 最初の遅延時間 */

	Cygnus::Float2 baseSize_ = {0.0f, 0.0f};	/* 基本サイズを保持 */
	Cygnus::Float4 color_ = {1.0f, 1.0f, 1.0f, 1.0f};	/* 色 */

	// スプライト
	std::unique_ptr<Cygnus::Sprite> spriteKill_;	/* 目標文字スプライト（「全ての敵を倒せ」） */
	std::unique_ptr<Cygnus::Sprite> spriteDestroy_;	/* 目標文字スプライト（「全ての発電機を壊せ」） */
	std::unique_ptr<Cygnus::Sprite> spriteBoss_;	/* 目標文字スプライト（「ボスを倒せ」） */

	std::unique_ptr<Cygnus::Sprite> spriteClear_;		/* ステージクリアスプライト */
};

