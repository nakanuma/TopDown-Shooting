#pragma once

// Engine
#include <Engine/3D/Object3D.h>

/// <summary>
/// 基底クラス
/// </summary>
class Bullet {
public:
	virtual void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;

protected:
	// オブジェクト
	std::unique_ptr<Object3D> objectBullet_;

	// パラメーター
	float speed_;
	Float3 velocity_;
};
