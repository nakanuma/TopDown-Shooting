#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/Types/BossEnemy/BossEnemy.h>

// =========================================================
// ゲームクリア時の演出を制御するクラス
// =========================================================
class GameClearSequence {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

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

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// ゲームクリア演出の有効化状態を取得します。
	/// </summary>
	/// <returns>演出を行っているか</returns>
	bool IsActive() const { return phase_ != Phase::None; }

	/// <summary>
	/// カメラ制御の有効化状態を取得します。
	/// </summary>
	/// <returns>カメラ制御の有効化状態</returns>
	bool IsControllingCamera() const { return IsActive() && phase_ == Phase::Rotate; }

	/// <summary>
	/// 終了フラグを取得します
	/// </summary>
	/// <returns>終了したかどうか</returns>
	bool IsFinished() const { return phase_ == Phase::Finish; }

	/// <summary>
	/// ボスのセットを行います。
	/// </summary>
	/// <param name="boss"></param>
	void SetBoss(BossEnemy* boss) { boss_ = boss; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// カメラ回転時の更新処理を行います。
	/// </summary>
	void UpdateRotate();

	/// <summary>
	/// 爆発とクリア文字演出時の更新処理を行います。
	/// </summary>
	void UpdateExplodeAndText();

	/// <summary>
	/// クリアシーケンスでのパーティクル発生処理を制御します。
	/// </summary>
	/// <param name="isScatterEmit">飛散パーティクルを発生させるか（カメラ回転フェーズではtrue, クリア文字演出時はfalse）</param>
	void EmitParticle(bool isScatterEmit);

private:
	/// <summary>
	/// 演出のフェーズ
	/// </summary>
	enum class Phase {
		None,           /* 初期状態 */
		Rotate,         /* カメラ回転（ボス中心に） */
		ExplodeAndText, /* 爆発とクリア時文字 */
		Finish,         /* 終了状態 */
	};

	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kCameraShakeIntensity = 0.4f; /* カメラシェイクの強度 */

	static constexpr float kCameraRotateDuration = 5.0f;					/* カメラ回転にかける時間 */
	static constexpr float kCameraDistance = 40.0f;							/* ボスからのカメラ距離 */
	static constexpr float kCameraRotZOffset = -0.13f;						/* カメラ回転のZ軸オフセット */
	static constexpr float kCameraHeightStart = 6.0f;						/* カメラ回転時の開始時高さ */
	static constexpr float kCameraHeightEnd = 2.0f;							/* カメラ回転時の終了時高さ */
	static constexpr float kCameraStartAngleOffset = Cygnus::PIf / 4.0f;	/* カメラ回転開始角度オフセット（ボスの正面 + 45度） */
	static constexpr float kCameraEndAngleOffset = Cygnus::PIf / 4.0f;		/* カメラ回転終了角度オフセット（ボスの正面 - 45度） */

	static constexpr float kExplodeDuration = 3.0f; /* 爆発フェーズにかける時間 */

	static constexpr float kExplodeEmitInterval = 0.2f;								/* 爆発パーティクル発生頻度 */
	static constexpr int32_t kExplodeSmokeEmitCount = 15;							/* 煙パーティクルの発生数 */
	static constexpr int32_t kExplodeScatterEmitCount = 5;							/* 爆発飛散パーティクル発生数 */
	static constexpr Cygnus::Float3 kParticleOffsetMin = {-10.0f, -3.0f, -10.0f};	/* パーティクル発生位置オフセット最小値 */
	static constexpr Cygnus::Float3 kParticleOffsetMax = {10.0f, 2.0f, 10.0f};		/* パーティクル発生位置オフセット最大値 */

	static constexpr float kBackgroundFadeDuration = 1.0f; /* 背景スプライトのフェードにかかる時間 */
	static constexpr float kBackgroundMaxAlpha = 0.75f;    /* 背景スプライトの最大Alpha値 */

	static constexpr float kTextFadeDuration = kBackgroundFadeDuration / 2.0f; /* クリア文字スプライトのフェードにかかる時間 */
	static constexpr float kSlideDistance = 50.0f;                             /* クリア文字スプライトの上へスライドする距離 */
	static constexpr float kClearTextSizeStartMultiplier = 1.2f;               /* クリア文字スプライトサイズの初期倍率 */

	static constexpr Cygnus::Float4 kInitialBackgroundColor = {0.0f, 0.0f, 0.0f, 0.0f}; /* 背景の色（黒で透明） */
	static constexpr Cygnus::Float2 kAnchorPoint = {0.5f, 0.5f};                        /* アンカーポイント */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	Phase phase_ = Phase::None; /* 現在フェーズ */
	float timer_ = 0.0f;        /* 全体のタイマー */

	float particleEmitTimer_ = 0.0f; /* パーティクル発生用タイマー */

	Cygnus::Float3 savedCameraPos_; /* 演出開始時のカメラ位置を保存 */
	Cygnus::Float3 savedCameraRot_; /* 演出開始時のカメラ回転を保存 */

	BossEnemy* boss_ = nullptr;			/* ボスのポインタ */
	Cygnus::Float3 lastBossPosition_;   /* ボスの最終位置を保存 */

	Cygnus::Float2 savedClearTextSize_; /* クリア文字スプライトの初期サイズを保存 */

	// -----Sprite-----
	std::unique_ptr<Cygnus::Sprite> spriteBackGround_; /* 黒い背景スプライト */
	std::unique_ptr<Cygnus::Sprite> spriteClearText_;  /* クリア時文字 */
};
