#include "GroundWarning.h"

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <TimeManager.h>

void GroundWarning::Initialize(const Cygnus::Float3& position, const Cygnus::Float3& direciton, Cygnus::ModelManager::ModelData* model) {
	// ---------------------------------------------------------
	// オブジェクト生成・初期設定
	// ---------------------------------------------------------
	objectBullet_ = std::make_unique<Cygnus::Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate_ = position;
	objectBullet_->transform_.scale_ = {kRadius, kRadius, kRadius};

	// ---------------------------------------------------------
	// コライダー生成・登録
	// ---------------------------------------------------------
	auto sphere = std::make_unique<Cygnus::SphereCollider>();
	sphere->SetTag("GroundWarning");
	sphere->SetFollowTarget(&objectBullet_->transform_.translate_);
	sphere->SetRadius(kRadius);
	sphere->SetOwner(this);

	collider_ = std::move(sphere);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	// ---------------------------------------------------------
	// パラメーター設定
	// ---------------------------------------------------------
	damage_ = kDamage;              // 攻撃力
	speed_ = 0.0f;                  // 弾速
	velocity_ = {0.0f, 0.0f, 0.0f}; // 速度ベクトル
}

void GroundWarning::Update() {
	// ---------------------------------------------------------
	// 寿命更新
	// ---------------------------------------------------------

	// 経過時間の更新
	elapsedTime_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	// 経過時間が寿命に達したら削除
	if (elapsedTime_ > kMaxLifeTime) {
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// コライダー有効化処理
	// ---------------------------------------------------------

	// 遅延時間分だけ経過したらコライダーを有効化 + パーティクル発生
	if (!colliderEnabled_ && elapsedTime_ >= kHitDelay) {
		colliderEnabled_ = true;                                                                                              // コライダーを有効化
		Cygnus::ParticleEffectManager::GetInstance()->Emit("explodeSmoke", GetTranslate() + kParticleOffset, kExplodeSmokeCount);     // 煙パーティクル
		Cygnus::ParticleEffectManager::GetInstance()->Emit("explodeScatter", GetTranslate() + kParticleOffset, kExplodeScatterCount); // 飛散パーティクル
	}

	// ---------------------------------------------------------
	// コライダー・行列更新処理
	// ---------------------------------------------------------

	// コライダー有効化時のみ更新
	if (colliderEnabled_)
		collider_->Update();
	// オブジェクト更新
	objectBullet_->UpdateMatrix();
}

void GroundWarning::Draw() {}

void GroundWarning::OnCollision(Cygnus::Collider* other) {
	// ---------------------------------------------------------
	// プレイヤーとの衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Player") {
		FinishLifeCycle();
	}
}
