#include "NormalEnemy.h"

// Engine
#include <Collider/CollisionManager.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void NormalEnemy::Initialize(const Float3& position, ModelManager::ModelData* model)
{
	///
	/// オブジェクト生成
	///
	
	objectEnemy_ = std::make_unique<Object3D>();
	objectEnemy_->model_ = model;
	objectEnemy_->transform_.translate = position;

	///
	///	コライダー生成
	/// 
	
	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void NormalEnemy::Update()
{
	// コライダー更新
	UpdateCollider();
	// オブジェクト更新
	objectEnemy_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void NormalEnemy::Draw()
{
	// オブジェクト描画
	objectEnemy_->Draw();


#ifdef _DEBUG
	ImGui::Begin("enemy collider");

	ImGui::DragFloat3("translate", &objectEnemy_->transform_.translate.x, 0.01f);

	ImGui::DragFloat3("center", &collider_->center_.x);
	ImGui::DragFloat("radius", &radius_);

	ImGui::End();
#endif
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void NormalEnemy::OnCollision(Collider* other)
{
	// 衝突したコライダーがPlayerBulletだった場合の処理
	if (other->GetTag() == "PlayerBullet") {
		// テスト用
		objectEnemy_->transform_.scale.x = 10.0f;
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateCollider()
{
	// 中心点
	collider_->center_ = objectEnemy_->transform_.translate;
	// 半径
	collider_->radius_ = radius_;
}
