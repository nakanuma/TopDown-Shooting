#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Object3D.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class Player;
class GamePlayScene;

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
	void Initialize(Cygnus::SpriteCommon* spriteCommon, Player* player, GamePlayScene* scene);

	/// <summary>
	/// ゲームクリア演出を開始します。
	/// </summary>
	void Start();

	/// <summary>
	/// 毎フレーム更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// シャドウマップ描画処理を行います。
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

	/// <summary>
	/// 演出のスキップを行います。
	/// </summary>
	void Skip();

	/// <summary>
	/// 入力操作によるスキップを行います
	/// </summary>
	void HandleSkip();

private:
	/// <summary>
	/// 演出のフェーズ
	/// </summary>
	enum class Phase {
		None,				/* 初期状態 */
		Fade1,				/* フェードインアウト（開始時） */
		RearCamera,			/* 背面カメラ */
		Shaking,			/* 揺れ */
		CameraPan,			/* カメラ水平移動（横から視点） */
		ShootDownMissile,	/* ボスのミサイルを撃ち落とす */
		Fade2,				/* フェードインアウト（終了時） */
		Finish				/* 終了 */
	};

	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kFadeDuration = 1.0f; /* フェード時間 */

	static constexpr float kWalkDuration = 0.25f; /* プレイヤーの歩行時間 */

	// シェイク発生のタイミング
	static constexpr float kShakeTime1 = 1.0f;
	static constexpr float kShakeTime2 = 3.0f;
	static constexpr float kShakeTime3 = 5.5f;
	// シェイクの持続時間と強さ
	static constexpr float kShakeDuration1 = 0.5f;
	static constexpr float kShakeIntensity1 = 0.5f;
	static constexpr float kShakeDuration2 = 1.0f;
	static constexpr float kShakeIntensity2 = 0.75f;
	static constexpr float kShakeDuration3 = 1.75f;
	static constexpr float kShakeIntensity3 = 1.0f;

	static constexpr float kShakePhaseEndDuration = 8.0f; /* シェイクフェーズの終了時間 */

	static constexpr float kPlayerShootDuration = 1.3f; /* プレイヤーが射撃するタイミング */
	static constexpr float kShootDownMissilePhaseEndDuration = 2.5f; /* ミサイル墜落フェーズの終了時間 */

	static constexpr Cygnus::Float3 kTopdownCameraPos = { 156.0f, 51.8f, -6.0f };   /* トップダウン視点カメラ位置 */
	static constexpr Cygnus::Float3 kTopdownCameraRot = { Cygnus::PIf / 4.0f, 0.0f, 0.0f }; /* トップダウン視点カメラ回転 */

	static constexpr float kSkipHoldTime = 1.0f; /* スキップに必要な秒数 */

	// =========================================================
	// Member Variables
	// =========================================================

	Phase phase_ = Phase::None; /* 現在フェーズ */
	float timer_ = 0.0f;        /* 全体のタイマー */

	bool isFadeStarted_ = false; /* 暗転管理用フラグ */

	Player* player_ = nullptr; /* プレイヤーのポインタ */
	GamePlayScene* scene_ = nullptr;	/* ゲームプレイシーンへのポインタ */

	Cygnus::Float3 shakeBaseCameraPos_;	/* シェイク時のカメラ位置保持用 */

	// 各シェイク済みフラグ
	bool isShaked1_ = false;
	bool isShaked2_ = false;
	bool isShaked3_ = false;

	// 壁崩壊済みフラグ
	bool isWallCrumbled_ = false;

	bool hasLaunchMissile_ = false; // ミサイル発射済みフラグ
	bool hasPlayerShooted_ = false; // プレイヤー射撃済みフラグ

	float spaceHoldTimer_ = 0.0f; /* スキップ用のタイマー */

	std::unique_ptr<Cygnus::Object3D> objectCrumblingWall_; /* 壊れそうな壁オブジェクト */
};
