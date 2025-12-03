#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <functional>
#include <memory>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 矩形トランジション制御クラス
// 分割された矩形がランダムに噛み合って開閉する
// =========================================================
class SplitBlockTransition {
public:
	/// <summary>
	/// トランジションの状態を表す列挙体
	/// </summary>
	enum class State {
		None,  /* 未実行 */
		Close, /* 開く */
		Open,  /* 閉じる */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスの取得を行います。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SplitBlockTransition* GetInstance();

	/// <summary>
	/// トランジションの初期化処理を行います。
	/// </summary>
	/// <param name="spriteCommon">スプライト共通処理クラス</param>
	/// <param name="splitCount">分割数</param>
	void Initialize(SpriteCommon* spriteCommon, uint32_t splitCount = 5);

	/// <summary>
	/// 開くトランジションを開始します。
	/// </summary>
	/// <param name="duration">所要時間</param>
	/// <param name="delayBeforeStart">開始までの遅延時間</param>
	void StartOpen(float duration, float delayBeforeStart = 0.0f);

	/// <summary>
	/// 閉じるトランジションを開始します。
	/// </summary>
	/// <param name="duration">所要時間</param>
	/// <param name="onComplete">完了に実行するコールバック関数</param>
	/// <param name="delayAfterComplete">完了までの遅延時間</param>
	void StartClose(float duration, std::function<void()> onComplete, float delayAfterComplete = 0.0f);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// フェードが完了したかを取得します。
	/// </summary>
	/// <returns>フェード完了フラグ</returns>
	bool IsFinished() const { return state_ == State::None; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// トランジション開始時の共通パラメーターを設定します。
	/// </summary>
	/// <param name="state">状態</param>
	/// <param name="duration">アニメーション時間</param>
	void StartTransitionCommon(State state, float duration);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kOpenBlockDelay = 0.05f; /* 角ブロックごとの開始時遅延時間（秒） */

	// =========================================================
	// Member Variables
	// =========================================================

	/// <summary>
	/// 1ブロックの情報（上下のスプライト + 進行度）
	/// </summary>
	struct Block {
		std::unique_ptr<Sprite> top;    /* 上側のスプライト */
		std::unique_ptr<Sprite> bottom; /* 下側のスプライト */
		float delay;                    /* 開始までの遅延 */
		float progress;                 /* 0~1の進行度 */
	};

	// ----- Parameters -----
	State state_ = State::None; /* 現在のトランジション状態 */
	float duration_ = 1.0f;     /* トランジションにかかる時間 */
	float timer_ = 0.0f;        /* 経過時間 */

	float delayBeforeFadeIn_ = 0.0f;         /* 開くトランジション開始までの遅延時間 */
	float delayAfterFadeOutComplete_ = 0.0f; /* 閉じるトランジション完了後の遅延時間 */
	float delayTimerAfterFadeOut_ = 0.0f;    /* 閉じるトランジション完了後の遅延時間タイマー */

	std::vector<Block> blocks_;                            /* 分割ブロック */
	uint32_t splitCount_;                                  /* 分割数 */
	std::unique_ptr<SpriteCommon> spriteCommon_ = nullptr; /* スプライト共通処理クラス */
	std::function<void()> onFadeComplete_;                 /* 閉じるトランジション完了後のコールバック */
};