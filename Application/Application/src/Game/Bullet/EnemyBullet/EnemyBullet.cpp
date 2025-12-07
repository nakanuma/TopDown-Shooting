#include "EnemyBullet.h"

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/CollisionManager.h>
#include <Engine/3D/LineDrawer.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <TimeManager.h>

// ---------------------------------------------------------
// External Includes
// ---------------------------------------------------------
#include <ImguiWrapper.h>

void EnemyBullet::Initialize(const Cygnus::Float3& position, const Cygnus::Float3& direciton, Cygnus::ModelManager::ModelData* model) {
	// ---------------------------------------------------------
	// オブジェクト生成・初期設定
	// ---------------------------------------------------------
	objectBullet_ = std::make_unique<Cygnus::Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate_ = position;
	objectBullet_->transform_.scale_ = {kRadius, kRadius, kRadius};

	// 進行方向から向きを計算して回転を設定
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate_ = {pitch, yaw, 0.0f};

	// ---------------------------------------------------------
	// コライダー生成・登録
	// ---------------------------------------------------------
	auto sphere = std::make_unique<Cygnus::SphereCollider>();
	sphere->SetTag("EnemyBullet");
	sphere->SetFollowTarget(&objectBullet_->transform_.translate_);
	sphere->SetRadius(kRadius);
	sphere->SetOwner(this);

	collider_ = std::move(sphere);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	// ---------------------------------------------------------
	// パラメーター設定
	// ---------------------------------------------------------
	damage_ = kDamage;              // 攻撃力
	speed_ = kSpeed;                // 弾速
	velocity_ = direciton * speed_; // 速度ベクトル
}

void EnemyBullet::Update() {
	// ---------------------------------------------------------
	// 位置更新
	// ---------------------------------------------------------
	objectBullet_->transform_.translate_ += velocity_;

	// ---------------------------------------------------------
	// 前フレーム位置履歴の更新（トレイル用）
	// ---------------------------------------------------------
	trailPoints_.push_back(objectBullet_->transform_.translate_);
	// 履歴数の上限を超えたら古い要素を削除して一定数に保つ
	if (trailPoints_.size() > kMaxTrailPoints) {
		trailPoints_.pop_front();
	}

	// ---------------------------------------------------------
	// 寿命更新
	// ---------------------------------------------------------
	elapsedTime_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	// 経過時間が寿命に達したら削除
	if (elapsedTime_ > kMaxLifeTime) {
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// コライダー・行列更新処理
	// ---------------------------------------------------------
	collider_->Update();
	objectBullet_->UpdateMatrix();
}

void EnemyBullet::Draw() {
	// ---------------------------------------------------------
	// トレイル（弾道エフェクト）描画
	// ---------------------------------------------------------
	DrawTrail();
}

void EnemyBullet::OnCollision(Cygnus::Collider* other) {
	Cygnus::Float3 bulletPos = this->GetTranslate();

	// ---------------------------------------------------------
	// プレイヤーとの衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Player") {
		// ヒット時パーティクル発生
		EmitBloodHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// 障害物との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Obstacle") {
		// ヒット時パーティクル発生
		EmitHardHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();
	}
}
