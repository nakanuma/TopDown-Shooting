#pragma once

// Engine
#include <Engine/3D/Object3D.h>

/// <summary>
/// 弾の基底クラス
/// </summary>
class Bullet {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

protected:
	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// 弾オブジェクト
	std::unique_ptr<Object3D> objectBullet_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 速さ
	float speed_;

	// 速度
	Float3 velocity_;
};
