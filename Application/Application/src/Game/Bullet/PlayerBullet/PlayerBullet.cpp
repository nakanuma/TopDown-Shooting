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

// ---------------------------------------------------------
// External Includes
// ---------------------------------------------------------
#include <ImguiWrapper.h>

void PlayerBullet::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) {
	// ---------------------------------------------------------
	// オブジェクト生成・初期設定
	// ---------------------------------------------------------
	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate_ = position;
	objectBullet_->transform_.scale_ = {kRadius, kRadius, kRadius};

	// 進行方向から向きを計算して回転を設定
	Float3 dir = Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate_ = {pitch, yaw, 0.0f};

	// ---------------------------------------------------------
	// コライダー生成・登録
	// ---------------------------------------------------------
	auto sphere = std::make_unique<SphereCollider>();
	sphere->SetTag("PlayerBullet");
	sphere->SetFollowTarget(&objectBullet_->transform_.translate_);
	sphere->SetRadius(kRadius);
	sphere->SetOwner(this);

	collider_ = std::move(sphere);
	CollisionManager::GetInstance()->Register(collider_.get());

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
	Float3 currentPos = objectBullet_->transform_.translate_;
	Float3 nextPos = currentPos + velocity_;

	// 前の位置から次の位置までの移動距離
	Float3 movement = nextPos - currentPos;
	float moveDistance = Float3::Length(movement);

	// レイキャストで中間の衝突をチェック
	if (moveDistance > kRadius * kRaycastThreshold) { // 移動距離が半径の半分以上の場合のみチェック
		Float3 rayDirection = Float3::Normalize(movement);

		RayCastHit hit;
		if (CollisionManager::GetInstance()->RayCast(
		        currentPos, rayDirection,
		        moveDistance + kRadius, // 弾の半径分を追加
		        &hit, {"PlayreBullet"}  // 自身は除外する
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
	elapsedTime_ += TimeManager::GetInstance()->GetDeltaTime();
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

void PlayerBullet::OnCollision(Collider* other) {
	Float3 bulletPos = this->GetTranslate();

	// ---------------------------------------------------------
	// 血の出る敵との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "NormalEnemy") {
		// ヒット時パーティクル発生
		EmitBloodHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();
	}

	// ---------------------------------------------------------
	// 硬い敵・物との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "ImmobileEnemy" || other->GetTag() == "BossEnemy" || other->GetTag() == "Obstacle") {
		// ヒット時パーティクル発生
		EmitHardHitParticles(bulletPos, velocity_);
		// ライフサイクル終了
		FinishLifeCycle();
	}
}