#pragma once

// C++
#include <functional>
#include <memory>

// Engine
#include <Sprite.h>
#include <SpriteCommon.h>

/// <summary>
/// 分割された矩形がランダムに噛み合って開閉するトランジション制御クラス
/// </summary>
class SplitBlockTransition {
public:
	/// <summary>
	/// トランジションの状態を表す列挙体
	/// </summary>
	enum class State {
		None,  // 未実行
		Close, // 開く
		Open,  // 閉じる
	};

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static SplitBlockTransition* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(SpriteCommon* spriteCommon, uint32_t splitCount = 8);

	/// <summary>
	/// 開くトランジションの開始
	/// </summary>
	void StartOpen(float duration, float delayBeforeStart = 0.0f);

	/// <summary>
	/// 閉じるトランジションの開始
	/// </summary>
	void StartClose(float duration, std::function<void()> onComplete, float delayAfterComplete = 0.0f);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 終了判定
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return state_ == State::None; }

private:
	/// <summary>
	/// 1ブロックの情報（上下のスプライト + 進行度）
	/// </summary>
	struct Block {
		std::unique_ptr<Sprite> top;
		std::unique_ptr<Sprite> bottom;
		float delay;    // 開始までの遅延
		float progress; // 0~1の進行度
	};

	State state_ = State::None; // 現在のトランジション状態
	float duration_ = 1.0f;     // トランジションにかかる時間
	float timer_ = 0.0f;        // 経過時間

	float delayBeforeFadeIn_ = 0.0f;         // 開くトランジション開始までの遅延時間
	float delayAfterFadeOutComplete_ = 0.0f; // 閉じるトランジション完了後の遅延時間
	float delayTimerAfterFadeOut_ = 0.0f;    // 閉じるトランジション完了後の遅延時間タイマー

	std::vector<Block> blocks_; // 分割ブロック
	uint32_t splitCount_;       // 分割数
	std::unique_ptr<SpriteCommon> spriteCommon_ = nullptr;
	std::function<void()> onFadeComplete_; // 閉じるトランジション完了後のコールバック
};