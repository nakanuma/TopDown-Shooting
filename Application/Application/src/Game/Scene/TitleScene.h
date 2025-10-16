#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include "BaseScene.h"
#include "Camera.h"
#include "Input.h"
#include "LightManager.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include <Engine/Collider/CollisionManager.h>
#include <Engine/Texture/PostEffectManager.h>
#include <Engine/Util/TimeManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Field/Field.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Manager/ObstacleManager.h>

// =========================================================
// タイトルシーンクラス
// =========================================================
class TitleScene : public BaseScene {
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
	void UpdateOrbitCamera(const Float3& target, float radius, float height, float speed);

	/// <summary>
	/// カーソルがクライアント領域内にあるか確認します。
	/// </summary>
	/// <returns>カーソル領域内判定フラグ</returns>
	bool IsInsideClientCursor();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Camera> camera = nullptr;				/* 3Dカメラクラス */
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;	/* スプライト共通処理クラス */
	std::unique_ptr<SoundManager> soundManager = nullptr;	/* サウンド管理クラス */
	Input* input = nullptr;									/* 入力管理クラス */
	LightManager* lightManager = nullptr;					/* 各ライト管理クラス */

	// ----- Sprite -----
	/// Todo : 適切なクラスへの整理
	std::unique_ptr<Sprite> spriteTitle_;					/* タイトルロゴスプライト */
	std::unique_ptr<Sprite> spriteStartButton_;				/* スタートボタンスプライト */

	// ----- Objects -----
	std::unique_ptr<Field> field_;							/* 床オブジェクト */
	std::unique_ptr<ObstacleManager> obstacleManager_;		/* 障害物管理クラス */

	// ----- Others -----
	std::unique_ptr<Loader> loader_;						/* ステージデータの管理クラス */
	int32_t shadowMapHandle_;								/* シャドウマップテクスチャ */
};
