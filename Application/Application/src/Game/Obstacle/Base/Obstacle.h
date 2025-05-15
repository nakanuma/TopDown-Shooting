#pragma once

// Engine
#include <Engine/3D/Object3D.h>
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>

/// <summary>
/// 障害物の基底クラス
/// </summary>
class Obstacle {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const Float3& position, const Float3& scale, ModelManager::ModelData* model) = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// タグの取得（コライダーに設定してあるタグ）
	/// </summary>
	std::string GetTag() const { return collider_->GetTag(); }

	/// <summary>
	/// 現在位置の取得
	/// </summary>
	Float3& GetTranslate() const { return objectObstacle_->transform_.translate; }

	/// <summary>
	/// スケールの取得
	/// </summary>
	Float3& GetScale() const { return objectObstacle_->transform_.scale; }

protected:
	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// 敵オブジェクト
	std::unique_ptr<Object3D> objectObstacle_;

	// ---------------------------------------------------------
	// コライダー
	// ---------------------------------------------------------

	// コライダー
	std::unique_ptr<Collider> collider_;
};
