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

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	パラメーター設定
	/// 
	
	speed_ = 1.0f;
	velocity_ = direciton * speed_;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void PlayerBullet::Update() { 
	// 移動処理
	objectBullet_->transform_.translate += velocity_;

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

#ifdef _DEBUG
	ImGui::Begin("playerBullet Collider");

	ImGui::DragFloat3("center", &collider_->center_.x);
	ImGui::DragFloat("radius", &collider_->radius_);

	ImGui::End();
#endif
}

void PlayerBullet::OnCollision(Collider* other)
{
	// 衝突したコライダーがNormalEnemyだった場合の処理
	if (other->GetTag() == "NormalEnemy") {
		// テスト用
		objectBullet_->transform_.scale.y = 10.0f;
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void PlayerBullet::UpdateCollider()
{
	// 中心座標
	collider_->center_ = objectBullet_->transform_.translate;
	// 半径
	collider_->radius_ = radius_;
}