#pragma once

// Engine
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Engine/3D/Object3D.h>

/// <summary>
/// 障害物の共通クラス
/// </summary>
class Obstacle : public ICollisionCallback {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, const Float3& colliderSize, ModelManager::ModelData* model);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// タグの取得（コライダーに設定してあるタグ）
	/// </summary>
	std::string GetTag() const { return collider_->GetTag(); }

	/// <summary>
	/// 現在位置の取得
	/// </summary>
	Float3& GetTranslate() const { return object_->transform_.translate; }

	/// <summary>
	/// スケールの取得
	/// </summary>
	Float3& GetScale() const { return object_->transform_.scale; }

	/// <summary>
	/// 破棄される際に呼ぶ関数
	/// </summary>
	virtual void OnDestroy() { CollisionManager::GetInstance()->Unregister(collider_.get()); }

	/// <summary>
	/// コライダーの有効化状態設定
	/// </summary>
	void SetActiveCollider(bool active) { if (collider_) { collider_->SetActive(active); }; }

protected:
	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// オブジェクト
	std::unique_ptr<Object3D> object_;

	// ---------------------------------------------------------
	// コライダー
	// ---------------------------------------------------------

	// コライダー
	std::unique_ptr<Collider> collider_;
};