#include "Obstacle.h"

void Obstacle::Initialize(const Float3& position, const Float3& scale, const Float3& rotate, const Float3& colliderSize, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	object_ = std::make_unique<Object3D>();
	object_->model_ = model;
	object_->transform_.translate = position;
	object_->materialCB_.data_->useEnvironmentMap = true;
	object_->materialCB_.data_->environmentStrength = 0.1f;

	Float3 size = colliderSize;

	// 横向き配置かどうかを判定
	if (std::abs(rotate.z - DegToRad(-90.0f)) < 0.01f) { // Blender上で横向き（-90度）になっているか確認
		// オブジェクトを横向きにする
		object_->transform_.rotate.y -= DegToRad(90.0f);

		// コライダーのxとzを入れ替え
		std::swap(size.x, size.z);
	}

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("Obstacle");
	collider_->SetOwner(this);

	auto aabb = std::make_unique<AABBCollider>();
	aabb->SetTag("Obstacle");
	aabb->SetFollowTarget(&object_->transform_.translate);
	aabb->SetSize(size);
	aabb->SetOwner(this);

	collider_ = std::move(aabb);
	CollisionManager::GetInstance()->Register(collider_.get());

	// コライダー更新（常に更新する必要は無いため初期化時に1度のみ）
	collider_->Update();
}

void Obstacle::Update() {
	object_->UpdateMatrix();
	object_->UpdateShadowMatrix();
}

void Obstacle::Draw() { object_->Draw(); }

void Obstacle::DrawShadow() { object_->DrawShadow(); }