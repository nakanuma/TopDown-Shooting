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
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Camera/FollowCamera.h>
#include <src/Game/Enemy/Manager/EnemyManager.h>
#include <src/Game/Field/Field.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Manager/ObstacleManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Teleporter/TeleporterManager.h>
#include <src/Game/Sequence/GameStartSequence.h>
#include <src/Game/Sequence/GameOverSequence.h>

// =========================================================
// ゲームプレイシーンクラス
// =========================================================
class GamePlayScene : public BaseScene {
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

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Camera> camera = nullptr;				/* 3Dカメラクラス */
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;	/* スプライト共通処理 */
	std::unique_ptr<SoundManager> soundManager = nullptr;	/* サウンド管理クラス */
	Input* input = nullptr;									/* 入力管理クラス */
	LightManager* lightManager = nullptr;					/* 各ライト管理クラス */

	// ----- Loader -----
	std::unique_ptr<Loader> loader_;						/* ステージデータの管理クラス */

	// ----- Object -----
	std::unique_ptr<Field> field_;							/* 床オブジェクト */
	std::unique_ptr<Player> player_;						/* プレイヤーオブジェクト */
	std::unique_ptr<EnemyManager> enemyManager_;			/* 敵管理クラス */
	std::unique_ptr<ObstacleManager> obstacleManager_;		/* 障害物管理クラス */
	std::unique_ptr<TeleporterManager> teleporterManager_;	/* テレポーター管理クラス */

	// ----- Others -----
	std::unique_ptr<FollowCamera> followCamera_;			/* 追従カメラ管理クラス */
	std::unique_ptr<PostEffectManager> postEffectManager_;	/* ポストエフェクト管理クラス */
	uint32_t shadowMapHandle_;								/* シャドウマップテクスチャ */
	std::unique_ptr<GameStartSequence> gameStartSequence_;  /* ゲームスタート時の演出制御クラス */
	std::unique_ptr<GameOverSequence> gameOverSequence_;	/* ゲームオーバー時の演出制御クラス */
};
