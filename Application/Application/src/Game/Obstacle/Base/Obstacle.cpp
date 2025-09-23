#include "Obstacle.h"

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void Obstacle::Initialize(const Float3& position, const Float3& scale, const Float3& rotate, const Float3& colliderSize, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	/// 

	object_ = std::make_unique<Object3D>();
	object_->model_ = model;
	object_->transform_.translate = position;

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("Obstacle");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	// コライダー設定
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = object_->transform_.translate;
		Float3 size = colliderSize;

		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void Obstacle::Update() {
	object_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void Obstacle::Draw() {
	object_->Draw();
}