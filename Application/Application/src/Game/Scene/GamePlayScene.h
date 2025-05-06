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
#include <Collider/CollisionManager.h>

// Application
#include <src/Game/Field/Field.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Enemy/Manager/EnemySpawnManager.h>

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

	// 床
	std::unique_ptr<Field> field_;
	// プレイヤー
	std::unique_ptr<Player> player_;
	// 敵のスポーンマネージャー
	std::unique_ptr<EnemySpawnManager> enemySpawnManager_;
};

