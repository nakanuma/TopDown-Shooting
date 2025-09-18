#include "Obstacle.h"

// ---------------------------------------------------------
// コンテナ
// ---------------------------------------------------------
void Container::Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model)
{
	///
	///	オブジェクト生成
	/// 

	objectObstacle_ = std::make_unique<Object3D>();
	objectObstacle_->model_ = model;
	objectObstacle_->transform_.translate = position;
	objectObstacle_->transform_.rotate = rotate;

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
		Float3 center = objectObstacle_->transform_.translate;
		Float3 size = scale;

		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}

void Container::Update()
{
	objectObstacle_->UpdateMatrix();
}

void Container::Draw()
{
	objectObstacle_->Draw();
}

// ---------------------------------------------------------
// フェンス
// ---------------------------------------------------------
void Fence::Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model)
{
	///
	///	オブジェクト生成
	/// 

	objectObstacle_ = std::make_unique<Object3D>();
	objectObstacle_->model_ = model;
	objectObstacle_->transform_.translate = position;
	objectObstacle_->transform_.rotate = rotate;

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
		Float3 center = objectObstacle_->transform_.translate;
		Float3 size = scale;

		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}

void Fence::Update()
{
	objectObstacle_->UpdateMatrix();
}

void Fence::Draw()
{
	objectObstacle_->Draw();
}

// ---------------------------------------------------------
// 壁
// ---------------------------------------------------------
void Wall::Initialize(const Float3& position, const Float3& scale, const Float3& rotate, ModelManager::ModelData* model)
{
	///
	///	オブジェクト生成
	/// 

	objectObstacle_ = std::make_unique<Object3D>();
	objectObstacle_->model_ = model;
	objectObstacle_->transform_.translate = position;
	objectObstacle_->transform_.rotate = rotate;

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
		Float3 center = objectObstacle_->transform_.translate;
		Float3 size = scale;

		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}

void Wall::Update()
{
	objectObstacle_->UpdateMatrix();
}

void Wall::Draw()
{
	objectObstacle_->Draw();
}
