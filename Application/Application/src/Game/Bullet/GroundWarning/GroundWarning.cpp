#include "GroundWarning.h"

// Engine
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void GroundWarning::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model)
{
	///
	///	オブジェクト生成
	/// 

	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate = position;
	objectBullet_->transform_.scale = { radius_, radius_, radius_ };
	objectBullet_->materialCB_.data_->color = { 1.0f, 1.0f, 1.0f, 0.5f };

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("GroundWarning");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	パラメーター設定
	///

	// 攻撃力
	damage_ = 20;

	// 速さ
	speed_ = 0.0f;

	// 速度
	velocity_ = { 0.0f, 0.0f, 0.0f };
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void GroundWarning::Update()
{
	// 時間経過による削除
	elapsedTime_ += 1.0f / 60.0f;
	if (elapsedTime_ > kMaxLifeTime) {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}

	// 指定秒数経過したらコライダー有効化
	if (!colliderEnabled_ && elapsedTime_ >= hitDelay_) {
		colliderEnabled_ = true;

		Float3 offset = { 0.0f, 1.5f, 0.0f };
		// 煙パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("warningSmoke", objectBullet_->transform_.translate + offset, 15);
		// 飛散パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("warningScatter", objectBullet_->transform_.translate + offset, 25);
	}
	// コライダー更新
	if (colliderEnabled_) {
		UpdateCollider();
	}
	// オブジェクト更新
	objectBullet_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void GroundWarning::Draw()
{
	// オブジェクト描画
	/*objectBullet_->Draw();*/
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void GroundWarning::OnCollision(Collider* other)
{
	// vs Player
	if (other->GetTag() == "Player") {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void GroundWarning::UpdateCollider()
{
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
		// 中心
		sphere->center_ = objectBullet_->transform_.translate;
		// 半径
		sphere->radius_ = radius_;
	}
}
