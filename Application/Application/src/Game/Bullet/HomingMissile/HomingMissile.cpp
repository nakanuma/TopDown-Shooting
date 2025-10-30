#include "HomingMissile.h"

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/CollisionManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Player/Player.h>

void HomingMissile::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) {
	// ---------------------------------------------------------
	// オブジェクト生成・初期設定
	// ---------------------------------------------------------
	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate = position;
	objectBullet_->transform_.scale = {0.5f, 0.5f, 0.5f};
	objectBullet_->materialCB_.data_->color = {0.5f, 0.5f, 0.5f, 1.0f};

	// 進行方向から向きを計算して回転を設定
	Float3 dir = Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate = {pitch, yaw, 0.0f};

	// ---------------------------------------------------------
	// コライダー生成・登録
	// ---------------------------------------------------------
	collider_ = std::make_unique<OBBCollider>();
	collider_->SetTag("HomingMissile");
	collider_->SetOwner(this);

	CollisionManager::GetInstance()->Register(collider_.get());

	// ---------------------------------------------------------
	// パラメーター設定
	// ---------------------------------------------------------
	damage_ = 20;                   // 攻撃力
	speed_ = 0.3f;                  // 弾速
	velocity_ = direciton * speed_; // 速度ベクトル
}

void HomingMissile::Update() {
	// ---------------------------------------------------------
	// 位置更新
	// ---------------------------------------------------------

	// プレイヤー方向
	Float3 toTarget = targetPlayer_->GetTranslate() - objectBullet_->transform_.translate;
	toTarget = Float3::Normalize(toTarget);

	// 現在の移動方向ベクトル
	Float3 currentDir = Float3::Normalize(velocity_);

	// 方向の補間
	Float3 newDir = Float3::Lerp(currentDir, toTarget, kTurnSpeed);
	newDir = Float3::Normalize(newDir);
	newDir.y = 0.0f;

	// 補間した方向で速度更新
	velocity_ = newDir * speed_;

	// 移動
	objectBullet_->transform_.translate += velocity_;

	// ---------------------------------------------------------
	// 弾の向きを進行方向へ向ける処理
	// ---------------------------------------------------------

	float yaw = std::atan2(newDir.x, newDir.z);
	float pitch = -std::asin(newDir.y);
	objectBullet_->transform_.rotate = {pitch, yaw, 0.0f};

	// パーティクル発生（後方から出るよう調整）
	float offsetDistance = -3.0f;
	Float3 offset = newDir * offsetDistance;
	ParticleEffectManager::GetInstance()->Emit("missileSmoke", objectBullet_->transform_.translate + offset, 1);

	// ---------------------------------------------------------
	// 寿命更新
	// ---------------------------------------------------------
	elapsedTime_ += 1.0f / 60.0f;
	// 経過時間が寿命に達したら削除
	if (elapsedTime_ > kMaxLifeTime) {
		// 煙パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("explodeSmoke", objectBullet_->transform_.translate + offset, 15);
		// 飛散パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("explodeScatter", objectBullet_->transform_.translate + offset, 25);

		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}

	// ---------------------------------------------------------
	// コライダー・行列更新処理
	// ---------------------------------------------------------
	UpdateCollider();
	objectBullet_->UpdateMatrix();
}

void HomingMissile::Draw() {
	// ---------------------------------------------------------
	// オブジェクト描画
	// ---------------------------------------------------------
	objectBullet_->Draw();
}

void HomingMissile::OnCollision(Collider* other) {
	Float3 bulletPos = this->objectBullet_->transform_.translate;

	// ---------------------------------------------------------
	// プレイヤーとの衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Player") {
		// 煙パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("explodeSmoke", objectBullet_->transform_.translate, 15);
		// 飛散パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("explodeScatter", objectBullet_->transform_.translate, 25);

		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}

	// ---------------------------------------------------------
	// 障害物との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Obstacle") {
		// 煙パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("explodeSmoke", objectBullet_->transform_.translate, 15);
		// 飛散パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("explodeScatter", objectBullet_->transform_.translate, 25);

		// 死亡させる
		isDead_ = true;
		// コライダー破棄
		OnDestroy();
	}
}

void HomingMissile::UpdateCollider() {
	if (OBBCollider* obb = dynamic_cast<OBBCollider*>(collider_.get())) {
		Float3 center = objectBullet_->transform_.translate;
		Float3 size = kColliderSize;

		// コライダーの位置をオブジェクトに追従させ、常にサイズを最新状態にする
		obb->center_ = center;
		obb->size_ = kColliderSize;

		// 回転行列を作成して、コライダーの回転軸の更新
		Matrix rotMat = Matrix::Rotation(objectBullet_->transform_.rotate);
		obb->xAxis_ = Float3::Normalize(Float3(rotMat.r[0][0], rotMat.r[1][0], rotMat.r[2][0]));
		obb->yAxis_ = Float3::Normalize(Float3(rotMat.r[0][1], rotMat.r[1][1], rotMat.r[2][1]));
		obb->zAxis_ = Float3::Normalize(Float3(rotMat.r[0][2], rotMat.r[1][2], rotMat.r[2][2]));
	}
}