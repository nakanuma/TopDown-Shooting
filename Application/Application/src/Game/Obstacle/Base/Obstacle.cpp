#include "Obstacle.h"

void Obstacle::Initialize(
    const Cygnus::Float3& position, const Cygnus::Float3& scale, const Cygnus::Float3& rotate, bool isCollider, const Cygnus::Float3& colliderSize, Cygnus::ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	object_ = std::make_unique<Cygnus::Object3D>();
	object_->model_ = model;
	object_->transform_.translate_ = position;

	Cygnus::Float3 size = colliderSize;

	// 横向き配置かどうかを判定
	if (std::abs(rotate.z - Cygnus::DegToRad(kHorizontalRotationAngle)) < kRotationTolerance) { // Blender上で横向き（-90度）になっているか確認
		// オブジェクトを横向きにする
		object_->transform_.rotate_.y -= Cygnus::DegToRad(kHorizontalAdjustmentAngle);
		// コライダーのxとzを入れ替え
		std::swap(size.x, size.z);
	}

	///
	///	コライダー生成
	///

	// コライダーの生成
	if (isCollider) {
		auto aabb = std::make_unique<Cygnus::AABBCollider>();
		aabb->SetTag("Obstacle");
		aabb->SetFollowTarget(&object_->transform_.translate_);
		aabb->SetSize(size);
		aabb->SetOwner(this);

		collider_ = std::move(aabb);
		Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

		// コライダー更新（動かないため常に更新する必要は無いので初期化時に1度のみ）
		collider_->Update();

	// 生成しない場合はnullptr
	} else {
		collider_ = nullptr;
	}
}

void Obstacle::Update() {
	object_->UpdateMatrix();
	object_->UpdateShadowMatrix();
}

void Obstacle::Draw() { object_->Draw(); }

void Obstacle::DrawShadow() { object_->DrawShadow(); }