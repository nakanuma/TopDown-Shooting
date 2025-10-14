#pragma once

// Engine
#include "BaseScene.h"
#include "Camera.h"
#include "DebugCamera.h"
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

// Application
#include <src/Game/Utility/NumberSprite.h>

/// <summary>
/// リザルトシーン
/// </summary>
class ResultScene : public BaseScene {
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

	///
	///	スプライト
	///

	// 通常スプライト
	std::unique_ptr<Sprite> spriteBackGround_;
	std::unique_ptr<Sprite> spriteTitleButton_;
	std::unique_ptr<Sprite> spriteRecord_;

	// 数字表示用スプライト
	std::unique_ptr<NumberSprite> spriteTotalDamage_;
	std::unique_ptr<NumberSprite> spriteDefeated_;
	std::unique_ptr<NumberSprite> spriteHitRate_;
	std::unique_ptr<NumberSprite> spriteClearTime_;

	///
	/// その他
	///

	int32_t shadowMapHandle_;
};
