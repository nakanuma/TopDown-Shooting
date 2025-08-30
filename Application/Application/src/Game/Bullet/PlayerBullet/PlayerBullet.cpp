#include "PlayerBullet.h"

// Engine
#include <Collider/CollisionManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <Engine/Util/RandomGenerator.h>

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
	objectBullet_->transform_.scale = { radius_, radius_, radius_ };

	Float3 dir = Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate = { pitch, yaw, 0.0f };

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
	damage_ = 10;

	// 速さ
	speed_ = 1.8f;

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
void PlayerBullet::OnCollision(Collider* other) {
	Float3 bulletPos = this->objectBullet_->transform_.translate;
	auto rand = RandomGenerator::GetInstance();

	// vs NormalEnemy
	if (other->GetTag() == "NormalEnemy") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}

	// vs ImmobileEnemy
	if (other->GetTag() == "ImmobileEnemy") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}

	// vs BossEnemy
	if (other->GetTag() == "BossEnemy") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}

	// vs NormalObstacle
	if (other->GetTag() == "NormalObstacle") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter",bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void PlayerBullet::UpdateCollider() {
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
		// 中心
		sphere->center_ = objectBullet_->transform_.translate;
		// 半径
		sphere->radius_ = radius_;
	}
}