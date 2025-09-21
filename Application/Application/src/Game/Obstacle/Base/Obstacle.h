#pragma once

// Engine
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Engine/3D/Object3D.h>

/// <summary>
/// 障害物の基底クラス
/// </summary>
class Obstacle {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) = 0;

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

	/// <summary>
	/// 破棄される際に呼ぶ関数
	/// </summary>
	virtual void OnDestroy() { CollisionManager::GetInstance()->Unregister(collider_.get()); }

protected:
	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// オブジェクト
	std::unique_ptr<Object3D> objectObstacle_;

	// ---------------------------------------------------------
	// コライダー
	// ---------------------------------------------------------

	// コライダー
	std::unique_ptr<Collider> collider_;
};

/// <summary>
/// コンテナ
/// </summary>
class Container : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// フェンス
/// </summary>
class Fence : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 壁
/// </summary>
class Wall : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// レンガのパレット積み
/// </summary>
class BrickPallet : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// コンクリートバリア
/// </summary>
class ConcreteBarrier : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// ドラム缶
/// </summary>
class DrumCan : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 貯水タンク
/// </summary>
class WaterTank : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 鉄骨の束
/// </summary>
class SteelBundle : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// 縦型タンク
/// </summary>
class VerticalTank : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// ISOタンク
/// </summary>
class ISOTank : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};

/// <summary>
/// IBCコンテナ
/// </summary>
class IBCContainer : public Obstacle, public ICollisionCallback {
public:
	void Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model) override;
	void Update() override;
	void Draw() override;
};