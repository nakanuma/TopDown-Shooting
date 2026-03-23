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
	void Start(AnimationState state, StageType type);

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

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_;	/* スプライト共通処理 */

	AnimationState currentState_ = AnimationState::None;	/* 現在の状態 */
	StageType stageType_;	/* ステージ目標 */
	float timer_ = 0.0f;	/* タイマー */

	// スプライト
	std::unique_ptr<Cygnus::Sprite> spriteKill_;	/* 目標文字スプライト（「全ての敵を倒せ」） */
	std::unique_ptr<Cygnus::Sprite> spriteDestroy_;	/* 目標文字スプライト（「全ての発電機を壊せ」） */
	std::unique_ptr<Cygnus::Sprite> spriteBoss_;	/* 目標文字スプライト（「ボスを倒せ」） */

	std::unique_ptr<Cygnus::Sprite> spriteClear_;		/* ステージクリアスプライト */
};

