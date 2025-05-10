#include "PlayerBullet.h"

// Engine
#include <Collider/CollisionManager.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void PlayerBullet::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	/// 

	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate = position;

	///
	///	コライダー生成
	/// 

	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("PlayerBullet");
	collider_->SetOwner(this);

	/*collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("PlayerBullet");
	collider_->SetOwner(this);*/

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	パラメーター設定
	/// 

	// 攻撃力
	damage_ = 1;

	// 速さ
	speed_ = 1.0f;

	// 速度
	velocity_ = direciton * speed_;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void PlayerBullet::Update() {
	// 移動処理
	objectBullet_->transform_.translate += velocity_;

	// 時間経過による削除
	elapsedTime_ += 1.0f / 60.0f;
	if (elapsedTime_ > kMaxLifeTime) {
		isDead_ = true;
	}

	// コライダー更新処理
	UpdateCollider();
	// オブジェクト更新
	objectBullet_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void PlayerBullet::Draw() {
	// オブジェクト描画
	objectBullet_->Draw();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void PlayerBullet::OnCollision(Collider* other)
{
	// 衝突したコライダーがNormalEnemyだった場合の処理
	if (other->GetTag() == "NormalEnemy") {
		// 死亡させる
		isDead_ = true;
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void PlayerBullet::UpdateCollider()
{
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
		// 中心
		sphere->center_ = objectBullet_->transform_.translate;
		// 半径
		sphere->radius_ = radius_;
	}

	//if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
	//	Float3 center = objectBullet_->transform_.translate;
	//	Float3 size = objectBullet_->transform_.scale;

	//	// min
	//	aabb->min_ = center - size;
	//	aabb->max_ = center + size;
	//}
}