#pragma once

// Engine
#include <Engine/3D/Object3D.h>
#include <Collider/Collider.h>

/// <summary>
/// 敵の基底クラス
/// </summary>
class Enemy
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const Float3& position, ModelManager::ModelData* model) = 0;

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

	// 敵オブジェクト
	std::unique_ptr<Object3D> objectEnemy_;

	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// 球体コライダー
	std::unique_ptr<SphereCollider> collider_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

};