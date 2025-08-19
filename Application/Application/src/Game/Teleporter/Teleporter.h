#pragma once

// C++
#include <string>

// Engine
#include <Engine/3D/Object3D.h>
#include <Engine/Collider/Collider.h>
#include <Engine/Collider/CollisionManager.h>

/// <summary>
/// テレポーター
/// </summary>
class Teleporter : public ICollisionCallback
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, ModelManager::ModelData* model);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// ペアIDをセット
	/// </summary>
	/// <param name="id"></param>
	void SetPairID(std::string id) { pairID_ = id; }
	
	/// <summary>
	/// ペアIDを取得
	/// </summary>
	const std::string& GetPairID() const { return pairID_; }

	/// <summary>
	/// ペアのテレポーターをセット
	/// </summary>
	void SetPair(Teleporter* other) { pair_ = other; }

	/// <summary>
	/// 位置の取得
	/// </summary>
	const Float3& GetTranslate() const { return object_->transform_.translate; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

private:
	// オブジェクト
	std::unique_ptr<Object3D> object_;
	// コライダー
	std::unique_ptr<Collider> collider_;
	Float3 colliderSize_ = { 3.0f, 0.5f, 3.0f };

	// 固有ID
	std::string pairID_;
	// ペアになるテレポーターのポインタ
	Teleporter* pair_ = nullptr;
	// 使用可能かどうか
	bool isActive_;
};

