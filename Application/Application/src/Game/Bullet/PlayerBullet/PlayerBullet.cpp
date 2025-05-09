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
// 破棄される際に呼ばれる関数
// ---------------------------------------------------------
void PlayerBullet::OnDestroy()
{
	// コリジョンマネージャーからコライダーの登録を解除
	CollisionManager::GetInstance()->Unregister(collider_.get());
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