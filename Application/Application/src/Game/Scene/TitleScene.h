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
#include <Engine/Util/TimeManager.h>
#include <Engine/Texture/PostEffectManager.h>

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Manager/ObstacleManager.h>
#include <src/Game/Field/Field.h>

// タイトルシーン
class TitleScene : public BaseScene {
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

	// 中心を向きながらカメラ回転
	void UpdateOrbitCamera(const Float3& target, float radius, float height, float speed);

private:
#ifdef _DEBUG
	bool useDebugCamera = false;    // デバッグカメラが有効か
	Transform savedCameraTransform; // 通常カメラのTransformを保持

	void DebugCameraUpdate(Input* input);
#endif
	/// <summary>
	/// カーソルがクライアント領域内にあるか確認
	/// </summary>
	bool IsInsideClientCursor();

private:
	std::unique_ptr<Camera> camera = nullptr;
	std::unique_ptr<DebugCamera> debugCamera = nullptr;
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;
	std::unique_ptr<SoundManager> soundManager = nullptr;
	Input* input = nullptr;
	LightManager* lightManager = nullptr;

	///
	/// スプライト
	///
	
	std::unique_ptr<Sprite> spriteTitle_;
	std::unique_ptr<Sprite> spriteStartButton_;

	///
	///	オブジェクト
	/// 

	// 床
	std::unique_ptr<Field> field_;
	// 障害物の管理クラス
	std::unique_ptr<ObstacleManager> obstacleManager_;

	///
	/// その他
	///

	// ローダー
	std::unique_ptr<Loader> loader_;

	// シャドウマップ
	int32_t shadowMapHandle_;
};
