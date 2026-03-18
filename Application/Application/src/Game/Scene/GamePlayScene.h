#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <BaseScene.h>
#include <Camera.h>
#include <Engine/Collider/CollisionManager.h>
#include <Engine/Texture/PostEffectManager.h>
#include <Engine/Util/TimeManager.h>
#include <Input.h>
#include <LightManager.h>
#include <ModelManager.h>
#include <Object3D.h>
#include <SoundManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <TextureManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Camera/FollowCamera.h>
#include <src/Game/EmissiveObject/EmissiveObject.h>
#include <src/Game/Enemy/EnemyManager.h>
#include <src/Game/Field/Field.h>
#include <src/Game/GameState/Manager/GameStateManager.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Manager/ObstacleManager.h>
#include <src/Game/Pause/PauseMenu.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Teleporter/TeleporterManager.h>
#include <src/Game/EventTrigger/EventManager.h>

// =========================================================
// ゲームプレイシーンクラス
// =========================================================
class GamePlayScene : public Cygnus::BaseScene {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// ゲームシーンの初期化処理を行います。
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// ゲームシーンの終了処理を行います。
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

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	// =========================================================
	// Accessor
	// =========================================================

	Cygnus::SpriteCommon* GetSpriteCommon() const { return spriteCommon_.get(); }

	FollowCamera* GetFollowCamera() const { return followCamera_.get(); }
	Cygnus::Camera* GetCamera() const { return camera_.get(); }

	Field* GetField() const { return field_.get(); }
	Player* GetPlayer() const { return player_.get(); }
	EnemyManager* GetEnemyManager() const { return enemyManager_.get(); }
	ObstacleManager* GetObstacleManager() const { return obstacleManager_.get(); }
	TeleporterManager* GetTeleportManager() const { return teleporterManager_.get(); }
	EventManager* GetEventManager() const { return eventManager_.get(); }

	bool HasBossIntroPlayed() const { return hasBossIntroPlayed_; }
	void SetBossIntroPlayed(bool flag) { hasBossIntroPlayed_ = flag; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// リザルトシーンへの遷移を行います。
	/// </summary>
	void TransitionToResult();

	/// <summary>
	/// タイトルシーンへの遷移を行います。
	/// </summary>
	void TransitionToTitle();

	/// <summary>
	/// ゲーム状態の初期化を行います。
	/// </summary>
	void InitializeGameStates();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitialCameraPosition = {0.0f, 50.0f, -55.0f};            /* 初期カメラ位置 */
	static constexpr Cygnus::Float3 kInitialCameraRotation = {Cygnus::PIf / 4.0f, 0.0f, 0.0f}; /* 初期カメラ回転角 */
	static constexpr float kCameraFovY = 0.45f;                                                /* カメラの視野角 */

	static constexpr float kSplitBlockOpenDuration = 0.5f; /* 分割ブロックトランジション開始時の演出時間 */
	static constexpr float kSplitBlockOpenDelay = 1.0f;    /* 分割ブロックトランジション開始時の遅延時間 */

	static constexpr float kFadeOutDuration = 0.5f; /* フェードアウトの演出時間 */
	static constexpr float kFadeOutDelay = 0.25f;   /* フェードアウト開始前の遅延時間 */

	static constexpr Cygnus::Float3 kDirectionalLightDirection = {0.367f, -0.653f, -0.662f}; /* 平行光源の方向 */
	static constexpr float kDirectionalLightIntensity = 1.0f;                                /* 平行光源の強度 */

	static constexpr Cygnus::Float3 kShadowBoundingBoxExtents = {30.0f, 10.0f, 30.0f}; /* シャドウマップ用AABBの範囲 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Cygnus::Camera> camera_ = nullptr;             /* 3Dカメラクラス */
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_ = nullptr; /* スプライト共通処理 */
	Cygnus::Input* input_ = nullptr;                               /* 入力管理クラス */
	Cygnus::LightManager* lightManager_ = nullptr;                 /* 各ライト管理クラス */

	// ----- Loader -----
	std::unique_ptr<Loader> loader_; /* ステージデータの管理クラス */

	// ----- Object -----
	std::unique_ptr<Field> field_;                         /* 床オブジェクト */
	std::unique_ptr<Player> player_;                       /* プレイヤーオブジェクト */
	std::unique_ptr<EnemyManager> enemyManager_;           /* 敵管理クラス */
	std::unique_ptr<ObstacleManager> obstacleManager_;     /* 障害物管理クラス */
	std::unique_ptr<TeleporterManager> teleporterManager_; /* テレポーター管理クラス */
	std::unique_ptr<EventManager> eventManager_;           /* イベントトリガー管理クラス */

	// ----- Others -----
	std::unique_ptr<FollowCamera> followCamera_;                   /* 追従カメラ管理クラス */
	std::unique_ptr<Cygnus::PostEffectManager> postEffectManager_; /* ポストエフェクト管理クラス */
	uint32_t shadowMapHandle_;                                     /* シャドウマップテクスチャ */

	std::unique_ptr<GameStateManager> stateManager_; /* ゲーム状態管理クラス */

	bool isTransitioning_ = false; /* リザルトシーンへの遷移中かどうか */

	bool isPaused_ = false;                /* ポーズ中フラグ */
	std::unique_ptr<PauseMenu> pauseMenu_; /* ポーズメニュークラス */

	bool hasBossIntroPlayed_ = false; /* ボス登場演出が再生されたかを記録 */
};
