#include "EnemyBullet.h"

// Engine
#include <Collider/CollisionManager.h>
#include <Engine/3D/LineDrawer.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Externals
#include <ImguiWrapper.h>

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
	objectBullet_->transform_.rotate = {pitch, yaw, 0.0f};

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

	// 前フレーム位置には現在位置と同じ値を入れておく
	previousPos_ = objectBullet_->transform_.translate;

	// 攻撃力
	damage_ = 5;

	// 速さ
	speed_ = 1.8f;

	// 速度
	velocity_ = direciton * speed_;
}

void EnemyBullet::Update() {
	// 前フレーム位置を保存
	previousPos_ = objectBullet_->transform_.translate;

	// 移動処理
	objectBullet_->transform_.translate += velocity_;

	// 履歴に追加
	trailPoints_.push_back(objectBullet_->transform_.translate);
	if (trailPoints_.size() > kMaxTrailPoints) {
		trailPoints_.pop_front();
	}

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

void EnemyBullet::Draw() {
	// オブジェクト描画
	objectBullet_->Draw();

	// 弾道の描画
	DrawTrail();
}

void EnemyBullet::OnCollision(Collider* other) {
	Float3 bulletPos = this->objectBullet_->transform_.translate;

	// vs Player
	if (other->GetTag() == "Player") {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}

	// vs Obstacle
	if (other->GetTag() == "Obstacle") {
		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}
}

void EnemyBullet::UpdateCollider() {
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
		// 中心
		sphere->center_ = objectBullet_->transform_.translate;
		// 半径
		sphere->radius_ = radius_;
	}
}

void EnemyBullet::DrawTrail() {
	Float4 headColor = {1.0f, 1.0f, 1.0f, 1.0f};
	Float4 tailColor = {1.0f, 1.0f, 1.0f, 0.0f};

	for (size_t i = 1; i < trailPoints_.size(); ++i) {
		float t0 = static_cast<float>(i - 1) / (trailPoints_.size()); // 古い
		float t1 = static_cast<float>(i) / (trailPoints_.size() - 1); // 新しい

		// 線の両端の色を補間
		Float4 c0 = Float4::Lerp(tailColor, headColor, t0);
		Float4 c1 = Float4::Lerp(tailColor, headColor, t1);

		LineDrawer::GetInstance()->RegisterTracer(trailPoints_[i - 1], trailPoints_[i], 0.5f, c1, c0);
	}
}
