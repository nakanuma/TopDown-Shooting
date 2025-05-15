#include "NormalObstacle.h"

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void NormalObstacle::Initialize(const Float3& position, const Float3& scale, ModelManager::ModelData* model) 
{
	///
	/// オブジェクト生成
	///

	objectObstacle_ = std::make_unique<Object3D>();
	objectObstacle_->model_ = model;
	objectObstacle_->transform_.translate = position;
	objectObstacle_->transform_.scale = scale;

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("NormalObstacle");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void NormalObstacle::Update() 
{
	///
	/// コライダー更新処理
	///

	UpdateCollider();

	///
	/// オブジェクト更新処理
	///

	objectObstacle_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void NormalObstacle::Draw() 
{
	// オブジェクト描画
	objectObstacle_->Draw();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void NormalObstacle::OnCollision(Collider* other) 
{

}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void NormalObstacle::UpdateCollider() 
{
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = objectObstacle_->transform_.translate;
		Float3 size = objectObstacle_->transform_.scale;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}
