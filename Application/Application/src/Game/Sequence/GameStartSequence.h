#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Object3D.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// ゲームスタート時の演出を制御するクラス
// =========================================================
class GameStartSequence {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

	/// <summary>
	/// 毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// UIの描画処理を行います。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 終了フラグを取得します。
	/// </summary>
	/// <returns></returns>
	bool IsFinished() const { return isFinished_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// ダイナマイト点滅の更新処理を行います。
	/// </summary>
	void UpdateBlink();

	/// <summary>
	/// ダイナマイト爆発時の更新処理を行います。
	/// </summary>
	void UpdateExplosion();

	/// <summary>
	/// プレイ開始移行時の更新処理を行います。
	/// </summary>
	void UpdateTransition();

	/// <summary>
	/// デバッグ用の演出スキップを行います。
	/// </summary>
	void DebugSkip();

private:
	/// <summary>
	/// 演出のフェーズ
	/// </summary>
	enum class Phase {
		Intro,      /* 開始時 */
		Brink,      /* ダイナマイトの点滅 */
		Explosion,  /* 爆発 */
		Transition, /* トップダウン視点へ移行 */
		Finish      /* 終了 */
	};

	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitCameraPos = {21.0f, 14.0f, -35.0f}; /* 初期カメラ位置 */
	static constexpr Cygnus::Float3 kInitCameraRot = {0.367f, 0.39f, 0.0f};  /* 初期カメラ回転 */

	static constexpr Cygnus::Float3 kTopdownCameraPos = {36.0f, 51.8f, -66.0f};   /* トップダウン視点カメラ位置 */
	static constexpr Cygnus::Float3 kTopdownCameraRot = { Cygnus::PIf / 4.0f, 0.0f, 0.0f}; /* トップダウン視点カメラ回転 */

	static constexpr Cygnus::Float2 kLetterBoxSize = {1280.0f, 70.0f}; /* 黒帯のサイズ */

	static constexpr Cygnus::Float2 kTopBoxStartPos = {640.0f, 35.0f};             /* 上部の黒帯の初期位置 */
	static constexpr Cygnus::Float2 kTopBoxEndPos = {640.0f, -35.0f};              /* 上部の黒帯の最終位置（画面外） */
	static constexpr Cygnus::Float2 kBottomBoxStartPos = {640.0f, 720.0f - 35.0f}; /* 下部の黒帯の初期位置 */
	static constexpr Cygnus::Float2 kBottomBoxEndPos = {640.0f, 720.0f + 35.0f};   /* 下部の黒帯の最終位置（画面外） */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	Phase phase_ = Phase::Intro; /* 現在フェーズ */

	float timer_ = 0.0f;      /* タイマー */
	float lerpT_ = 0.0f;      /* 補間用タイマー */
	bool isFinished_ = false; /* 終了フラグ */

	bool isDebugSkip_ = false;	/* デバッグスキップ用フラグ */

	// ----- Objects -----
	std::unique_ptr<Cygnus::Object3D> objectCrumblingWall_; /* 壊れそうな壁オブジェクト */
	std::unique_ptr<Cygnus::Object3D> objectDynamite_;      /* ダイナマイトオブジェクト */

	bool isDynamiteVisible_ = true; /* ダイナマイトの可視状態（何度も切り替わる） */
	bool isExplode_ = false;        /* 爆発終了フラグ */

	// ----- Sprite -----
	std::unique_ptr<Cygnus::Sprite> spriteTopLetterBox_;    /* 上部の黒帯 */
	std::unique_ptr<Cygnus::Sprite> spriteBottomLetterBox_; /* 下部の黒帯 */
};