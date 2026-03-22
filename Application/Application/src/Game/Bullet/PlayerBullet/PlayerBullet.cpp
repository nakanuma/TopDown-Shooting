#include "PlayerBullet.h"

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/CollisionManager.h>
#include <Easing.h>
#include <Engine/3D/LineDrawer.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <Engine/Util/RandomGenerator.h>
#include <MyMath.h>
#include <TimeManager.h>
#include <SoundManager.h>

// ---------------------------------------------------------
// External Includes
// ---------------------------------------------------------
#include <ImguiWrapper.h>

void PlayerBullet::Initialize(const Cygnus::Float3& position, const Cygnus::Float3& direciton, Cygnus::ModelManager::ModelData* model) {
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
	sphere->SetTag("PlayerBullet");
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

	prevPosition_ = position; // 前フレーム位置を初期化
}

void PlayerBullet::Update() {
	// ---------------------------------------------------------
	// 連続衝突判定（レイキャスト）
	// ---------------------------------------------------------
	Cygnus::Float3 currentPos = objectBullet_->transform_.translate_;
	Cygnus::Float3 nextPos = currentPos + velocity_;

	// 前の位置から次の位置までの移動距離
	Cygnus::Float3 movement = nextPos - currentPos;
	float moveDistance = Cygnus::Float3::Length(movement);

	// レイキャストで中間の衝突をチェック
	if (moveDistance > kRadius * kRaycastThreshold) { // 移動距離が半径の半分以上の場合のみチェック
		Cygnus::Float3 rayDirection = Cygnus::Float3::Normalize(movement);

		Cygnus::RayCastHit hit;
		if (Cygnus::CollisionManager::GetInstance()->RayCast(
		        currentPos, rayDirection,
		        moveDistance + kRadius, // 弾の半径分を追加
		        &hit, {"PlayreBullet", "EventTrigger", "PowerGenerator"} // 除外するタグを設定
		        )) {
			// 衝突した位置に弾を移動
			objectBullet_->transform_.translate_ = hit.hitPoint;

			// 位置更新をスキップ
			prevPosition_ = objectBullet_->transform_.translate_;

			collider_->Update();
			objectBullet_->UpdateMatrix();
			return;
		}
	}

	// ---------------------------------------------------------
	// 位置更新
	// ---------------------------------------------------------

	// 次フレーム位置へ移動
	objectBullet_->transform_.translate_ = nextPos;
	// 前フレーム位置を更新
	prevPosition_ = currentPos;

	// ---------------------------------------------------------
	// 前フレーム位置履歴の更新（トレイル用）
	// ---------------------------------------------------------
	trailPoints_.push_back(objectBullet_->transform_.translate_);
	if (trailPoints_.size() > kMaxTrailPoints) {
		trailPoints_.pop_front();
	}

	// ---------------------------------------------------------
	// 寿命更新
	// ---------------------------------------------------------
	elapsedTime_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	// 経過時間が寿命に達したら削除
	if (elapsedTime_ > kMaxLifeTime) {
		// ライフサイクル終了
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// コライダー・行列更新処理
	// ---------------------------------------------------------
	collider_->Update();
	objectBullet_->UpdateMatrix();
}

void PlayerBullet::Draw() {
	// ---------------------------------------------------------
	// トレイル（弾道エフェクト）描画
	// ---------------------------------------------------------
	DrawTrail();
}

void PlayerBullet::OnCollision(Cygnus::Collider* other) {
	Cygnus::Float3 bulletPos = this->GetTranslate();

	// ---------------------------------------------------------
	// 柔らかい敵との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "NormalEnemy") {
		// ヒット時パーティクル発生
		EmitBloodHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();

		// 効果音発生
		Cygnus::SoundManager::GetInstance()->Play("hit_soft_enemy", false, 0.1f);
	}

	// ---------------------------------------------------------
	// 硬い敵との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "ImmobileEnemy" || other->GetTag() == "BossEnemy") {
		// ヒット時パーティクル発生
		EmitHardHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();

		// 効果音発生
		Cygnus::SoundManager::GetInstance()->Play("hit_hard_enemy", false, 0.2f);
	}

	// ---------------------------------------------------------
	// 障害物との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Obstacle" || other->GetTag() == "PowerGenerator") {
		// ヒット時パーティクル発生
		EmitHardHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();

		// 効果音発生
		Cygnus::SoundManager::GetInstance()->Play("hit_obstacle", false, 0.2f);
	}

	// ---------------------------------------------------------
	// ミサイルとの衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "HomingMissile") {
		// ヒット時パーティクル発生
		EmitHardHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();

		// 効果音発生
		Cygnus::SoundManager::GetInstance()->Play("hit_obstacle", false, 0.2f);
	}
}