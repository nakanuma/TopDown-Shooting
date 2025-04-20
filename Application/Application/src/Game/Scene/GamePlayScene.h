#pragma once 

// Engine
#include "BaseScene.h"
#include "Camera.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Sprite.h"
#include "ModelManager.h"
#include "Object3D.h"
#include "SoundManager.h"
#include "Input.h"
#include "LightManager.h"

// Application
#include <src/Game/Field/Field.h>

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
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	///
	/// ↓ ゲームシーン用
	///

	// 床
	std::unique_ptr<Field> field_;
};

