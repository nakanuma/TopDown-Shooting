#include "EnemyBullet.h"

// Engine
#include <Collider/CollisionManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void EnemyBullet::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate = position;
	objectBullet_->transform_.scale = {radius_, radius_, radius_};

	Float3 dir = Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate = { pitch, yaw, 0.0f };

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("EnemyBullet");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	パラメーター設定
	///

	// 攻撃力
	damage_ = 5;

	// 速さ
	speed_ = 1.8f;

	// 速度
	velocity_ = direciton * speed_;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void EnemyBullet::Update() {
	// 移動処理
	objectBullet_->transform_.translate += velocity_;

	// 時間経過による削除
	elapsedTime_ += 1.0f / 60.0f;
	if (elapsedTime_ > kMaxLifeTime) {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}

	// コライダー更新処理
	UpdateCollider();
	// オブジェクト更新
	objectBullet_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void EnemyBullet::Draw() {
	// オブジェクト描画
	objectBullet_->Draw();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void EnemyBullet::OnCollision(Collider* other) {
	Float3 bulletPos = this->objectBullet_->transform_.translate;

	// vs Player
	if (other->GetTag() == "Player") {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}

	// vs NormalObstacle
	if (other->GetTag() == "NormalObstacle") {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void EnemyBullet::UpdateCollider() {
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
		// 中心
		sphere->center_ = objectBullet_->transform_.translate;
		// 半径
		sphere->radius_ = radius_;
	}
}
