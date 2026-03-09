#include "HomingMissile.h"

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/CollisionManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <TimeManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Player/Player.h>

void HomingMissile::Initialize(const Cygnus::Float3& position, const Cygnus::Float3& direciton, Cygnus::ModelManager::ModelData* model) {
	// ---------------------------------------------------------
	// オブジェクト生成・初期設定
	// ---------------------------------------------------------
	objectBullet_ = std::make_unique<Cygnus::Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate_ = position;
	objectBullet_->transform_.scale_ = kMissileScale;
	objectBullet_->materialCB_.data_->color = kMissileColor;

	// 進行方向から向きを計算して回転を設定
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate_ = {pitch, yaw, 0.0f};

	// ---------------------------------------------------------
	// コライダー生成・登録
	// ---------------------------------------------------------

	auto obb = std::make_unique<Cygnus::OBBCollider>();
	obb->SetTag("HomingMissile");
	obb->SetFollowTarget(&objectBullet_->transform_.translate_);
	obb->SetFollowRotation(&objectBullet_->transform_.rotate_);
	obb->SetSize(kColliderSize);
	obb->SetOwner(this);

	collider_ = std::move(obb);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	// ---------------------------------------------------------
	// パラメーター設定
	// ---------------------------------------------------------
	damage_ = kDamage;              // 攻撃力
	speed_ = kSpeed;                // 弾速
	velocity_ = direciton * speed_; // 速度ベクトル
}

void HomingMissile::Update() {
	// ---------------------------------------------------------
	// 位置更新
	// ---------------------------------------------------------

	// プレイヤー方向
	Cygnus::Float3 toTarget = targetPlayer_->GetTranslate() - objectBullet_->transform_.translate_;
	toTarget = Cygnus::Float3::Normalize(toTarget);

	// 現在の移動方向ベクトル
	Cygnus::Float3 currentDir = Cygnus::Float3::Normalize(velocity_);

	// 方向の補間
	Cygnus::Float3 newDir = Cygnus::Float3::Lerp(currentDir, toTarget, kTurnSpeed);
	newDir = Cygnus::Float3::Normalize(newDir);
	newDir.y = 0.0f;

	// 補間した方向で速度更新
	velocity_ = newDir * speed_;

	// 移動
	objectBullet_->transform_.translate_ += velocity_;

	// ---------------------------------------------------------
	// 弾の向きを進行方向へ向ける処理
	// ---------------------------------------------------------

	float yaw = std::atan2(newDir.x, newDir.z);
	float pitch = -std::asin(newDir.y);
	objectBullet_->transform_.rotate_ = {pitch, yaw, 0.0f};

	// パーティクル発生（後方から出るよう調整）
	Cygnus::Float3 offset = newDir * kSmokeOffsetDistance;
	Cygnus::ParticleEffectManager::GetInstance()->Emit("missileSmoke", objectBullet_->transform_.translate_ + offset, kMissileSmokeCount);

	// ---------------------------------------------------------
	// 寿命更新
	// ---------------------------------------------------------
	elapsedTime_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 経過時間が寿命に達したら削除
	if (elapsedTime_ > kMaxLifeTime) {
		// ヒット時パーティクル発生
		EmitHitParticles();
		// ライフサイクル終了
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// コライダー・行列更新処理
	// ---------------------------------------------------------
	collider_->Update();
	objectBullet_->UpdateMatrix();
}

void HomingMissile::Draw() {
	// ---------------------------------------------------------
	// オブジェクト描画
	// ---------------------------------------------------------
	objectBullet_->Draw();
}

void HomingMissile::OnCollision(Cygnus::Collider* other) {
	// ---------------------------------------------------------
	// プレイヤー・障害物との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Player" || other->GetTag() == "Obstacle") {
		// ヒット時パーティクル発生
		EmitHitParticles();
		// ライフサイクル終了
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// プレイヤーの弾との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "PlayerBullet") {
		// ヒット時パーティクル発生
		EmitHitParticles();
		// 死亡状態にする
		isDead_ = true;
	}
}

void HomingMissile::EmitHitParticles() {
	// 煙パーティクル発生
	Cygnus::ParticleEffectManager::GetInstance()->Emit("explodeSmoke", objectBullet_->transform_.translate_, kExplodeSmokeCount);
	// 飛散パーティクル発生
	Cygnus::ParticleEffectManager::GetInstance()->Emit("explodeScatter", objectBullet_->transform_.translate_, kExplodeScatterCount);
}
