#include "Obstacle.h"

void Obstacle::Initialize(const Float3& position, const Float3& scale, const Float3& rotate, const Float3& colliderSize, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	object_ = std::make_unique<Object3D>();
	object_->model_ = model;
	object_->transform_.translate_ = position;
	object_->materialCB_.data_->useEnvironmentMap = true;
	object_->materialCB_.data_->environmentStrength = kEnvironmentStrength;

	Float3 size = colliderSize;

	// 横向き配置かどうかを判定
	if (std::abs(rotate.z - DegToRad(kHorizontalRotationAngle)) < kRotationTolerance) { // Blender上で横向き（-90度）になっているか確認
		// オブジェクトを横向きにする
		object_->transform_.rotate_.y -= DegToRad(kHorizontalAdjustmentAngle);

		// コライダーのxとzを入れ替え
		std::swap(size.x, size.z);
	}

	///
	///	コライダー生成
	///

	auto aabb = std::make_unique<AABBCollider>();
	aabb->SetTag("Obstacle");
	aabb->SetFollowTarget(&object_->transform_.translate_);
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