#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <BaseScene.h>
#include <Camera.h>
#include <Input.h>
#include <LightManager.h>
#include <ModelManager.h>
#include <Object3D.h>
#include <SoundManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <TextureManager.h>
#include <Engine/Collider/CollisionManager.h>
#include <Engine/Texture/PostEffectManager.h>
#include <Engine/Util/TimeManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Field/Field.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Manager/ObstacleManager.h>
#include <src/Game/Utility/UI/MouseCursor.h>

// =========================================================
// タイトルシーンクラス
// =========================================================
class TitleScene : public Cygnus::BaseScene {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// タイトルシーンの初期化処理を行います。
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// リザルトシーンの終了処理を行います。
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// シーンの描画処理を行います。
	/// </summary>
	void Draw() override;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 中心を向きながらカメラ回転を行います。
	/// </summary>
	/// <param name="target">ターゲット座標</param>
	/// <param name="radius">回転半径</param>
	/// <param name="height">カメラの高さ</param>
	/// <param name="speed">回転速度</param>
	void UpdateOrbitCamera(const Cygnus::Float3& target, float radius, float height, float speed);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitialCameraPosition = { 0.0f, 30.0f, -50.0f };	/* 初期カメラ位置 */
	static constexpr Cygnus::Float3 kInitialCameraRotation = { 0.5f, 0.0f, 0.0f };		/* 初期カメラ回転角 */
	static constexpr float kCameraFovY = 0.45f;											/* カメラの視野角 */

	static constexpr Cygnus::Float2 kTitlePosition = { 640.0f, 140.0f };		/* タイトル位置 */
	static constexpr Cygnus::Float2 kStartButtonPosition = { 640.0f, 580.0f };	/* スタートボタン位置 */
	static constexpr Cygnus::Float2 kAnchorPoint = { 0.5f, 0.5f };				/* アンカーポイント（中心） */

	static constexpr float kFadeInDuration = 0.5f;			/* フェードインの演出時間 */
	static constexpr float kFadeInDelay = 0.25f;			/* フェードイン開始時の遅延時間 */
	static constexpr float kSplitBlockCloseDuration = 1.0f;	/* 分割ブロックトランジション開始時の演出時間 */
	static constexpr float kSplitBlockCloseDelay = 0.5f;	/* 分割ブロックトランジション開始時の遅延時間 */

	static constexpr Cygnus::Float3 kDirectionalLightDirection = { 0.367f, -0.653f, -0.662f };	/* 平行光源の方向 */
	static constexpr float kDirectionalLightIntensity = 0.4f;									/* 平行光源の強度 */

	static constexpr Cygnus::Float3 kCameraTargetPosition = { 0.0f, 0.0f, 0.0f };	/* カメラのターゲット位置 */
	static constexpr float kOrbitCameraRadius = 50.0f;								/* カメラ回転の半径 */
	static constexpr float kOrbitCameraHeight = 30.0f;								/* カメラ回転時の高さ */
	static constexpr float kOrbitCameraSpeed = 0.25f;								/* カメラ回転の速度 */

	static constexpr float kTitleFloatSpeed = 1.2f;								/* sinfの係数 */
	static constexpr float kTitleFloatAmplitude = 4.0f;							/* sinfの結果にかける係数（移動量） */
	static constexpr Cygnus::Float2 kTitleBasePosition = { 640.0f, 140.0f };	/* タイトルスプライトの基準位置 */

	static constexpr float kStartButtonBlinkSpeed = 4.0f;						/* sinfの係数 */
	static constexpr float kStartButtonBlinkOffset = 1.0f;						/* sinfの結果に足すオフセット */
	static constexpr float kStartButtonBlinkScale = 2.0f;						/* sinfの結果を割る値 */
	static constexpr Cygnus::Float4 kStartButtonBaseColor = { 1.0f, 1.0f, 1.0f, 1.0f };	/* スタートボタンの色 */

	static constexpr int32_t kSmokeEmitInterval = 5;							/* 煙パーティクルを発生させる間隔（フレーム） */
	static constexpr int32_t kSmokeEmitCount = 1;								/* 煙パーティクルの発生数 */
	static constexpr Cygnus::Float3 kSmokePosition1 = { 12.0f, 4.0f, -12.0f };	/* 煙パーティクル発生位置1 */
	static constexpr Cygnus::Float3 kSmokePosition2 = { -14.0f, 4.0f, 12.0f };	/* 煙パーティクル発生位置2 */
	static constexpr Cygnus::Float3 kSmokePosition3 = { 34.0f, 4.0f, 8.0f };	/* 煙パーティクル発生位置3 */

	static constexpr int32_t kSparkEmitCount = 5;								/* 火花パーティクルの発生数 */
	static constexpr Cygnus::Float3 kSparkPosition1 = { 25.0f, 4.0f, 16.0f };	/* 火花パーティクル発生位置1 */
	static constexpr Cygnus::Float3 kSparkPosition2 = { -31.0f, 3.0f, 11.0f };	/* 火花パーティクル発生位置2 */
	static constexpr Cygnus::Float3 kSparkPosition3 = { -31.0f, 4.0f, -26.0f };	/* 火花パーティクル発生位置3 */
	static constexpr Cygnus::Float3 kSparkDirection1 = { 1.0f, 0.0f, 0.0f };	/* 火花パーティクル方向1 */
	static constexpr Cygnus::Float3 kSparkDirection2 = { -1.0f, 0.0f, 0.0f };	/* 火花パーティクル方向2 */
	static constexpr Cygnus::Float3 kSparkDirection3 = { -1.0f, 0.0f, 0.0f };	/* 火花パーティクル方向3 */
	static constexpr int32_t kMinSparkInterval = 20;							/* 火花パーティクルの最小発生間隔 */
	static constexpr int32_t kMaxSparkInterval = 50;							/* 火花パーティクルの最大発生間隔 */

	static constexpr Cygnus::Float3 kShadowBoundingBoxExtents = { 30.0f, 10.0f, 30.0f };	/* シャドウマップ用AABBの範囲 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Cygnus::Camera> camera_ = nullptr;						/* 3Dカメラクラス */
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_ = nullptr;			/* スプライト共通処理クラス */
	Cygnus::Input* input_ = nullptr;										/* 入力管理クラス */
	Cygnus::LightManager* lightManager_ = nullptr;							/* 各ライト管理クラス */
	std::unique_ptr<Cygnus::PostEffectManager> postEffectManager_;			/* ポストエフェクト管理クラス */

	// ----- Sprite -----
	std::unique_ptr<Cygnus::Sprite> spriteTitle_;					/* タイトルロゴスプライト */
	std::unique_ptr<Cygnus::Sprite> spriteStartButton_;				/* スタートボタンスプライト */
	std::unique_ptr<MouseCursor> cursor_;                                /* カーソルUI */

	// ----- Objects -----
	std::unique_ptr<Field> field_;							/* 床オブジェクト */
	std::unique_ptr<ObstacleManager> obstacleManager_;		/* 障害物管理クラス */

	// ----- Others -----
	std::unique_ptr<Loader> loader_;						/* ステージデータの管理クラス */
	int32_t shadowMapHandle_;								/* シャドウマップテクスチャ */
};
