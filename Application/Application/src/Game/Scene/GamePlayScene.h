#pragma once 

// Engine
#include "BaseScene.h"
#include "Camera.h"
#include "DebugCamera.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Input.h"
#include "LightManager.h"
#include <Engine/Collider/CollisionManager.h>
#include <Engine/Util/TimeManager.h>

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Field/Field.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Enemy/Manager/EnemyManager.h>
#include <src/Game/Obstacle/Manager/ObstacleManager.h>

#include <src/Game/Particles/Spark/SparkParticle_Shrink.h>

#include <src/Game/Camera/FollowCamera.h>

class GamePlayScene : public BaseScene
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

private:
#ifdef _DEBUG
	bool useDebugCamera = false;    // デバッグカメラが有効か
	Transform savedCameraTransform; // 通常カメラのTransformを保持

	void DebugCameraUpdate(Input* input);
#endif

private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<DebugCamera> debugCamera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	// レンダーテクスチャ
	uint32_t renderTexture_;

	///
	/// ↓ ゲームシーン用
	///

	/* ステージデータ */

	// ローダー
	std::unique_ptr<Loader> loader_;

	/* オブジェクト関連 */

	// 床
	std::unique_ptr<Field> field_;
	// プレイヤー
	std::unique_ptr<Player> player_;
	// 敵の管理クラス
	std::unique_ptr<EnemyManager> enemyManager_;
	// 障害物の管理クラス
	std::unique_ptr<ObstacleManager> obstacleManager_;

	/* その他 */

	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	/* パーティクル用モデル（あとで適切な位置に整理） */
	ModelManager::ModelData modelSparkShrink_;
};

