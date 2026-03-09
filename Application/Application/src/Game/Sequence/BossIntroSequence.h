#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// ボス登場演出を制御するクラス
// =========================================================
class BossIntroSequence {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize(Cygnus::SpriteCommon* spriteCommon, Player* player);

	/// <summary>
	/// ゲームクリア演出を開始します。
	/// </summary>
	void Start();

	/// <summary>
	/// 毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// UIの描画処理を行います。
	/// </summary>
	void DrawUI();

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// 終了フラグを取得します
	/// </summary>
	/// <returns>終了したかどうか</returns>
	bool IsFinished() const { return phase_ == Phase::Finish; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================



private:
	/// <summary>
	/// 演出のフェーズ
	/// </summary>
	enum class Phase {
		None,			/* 初期状態 */
		Fade1,			/* フェードインアウト（開始時） */
		FrontCamera,	/* 正面カメラ */
		RearCamera,		/* 背面カメラ */

		Fade2,			/* フェードインアウト（終了時） */
		Finish			/* 終了 */
	};

	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kFadeDuration = 1.0f; /* フェード時間 */

	static constexpr float kWalkDuration = 0.5f; /* プレイヤーの歩行時間 */

	// =========================================================
	// Member Variables
	// =========================================================

	Phase phase_ = Phase::None; /* 現在フェーズ */
	float timer_ = 0.0f;        /* 全体のタイマー */

	bool isFadeStarted_ = false; /* 暗転管理用フラグ */

	Player* player_ = nullptr; /* プレイヤーのポインタ */
};
