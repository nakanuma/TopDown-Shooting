#include "HomingMissile.h"

// Engine
#include <Collider/CollisionManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Application
#include <src/Game/Player/Player.h>

void HomingMissile::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
	objectBullet_->transform_.translate = position;
	objectBullet_->transform_.scale = {0.5f, 0.5f, 0.5f};
	objectBullet_->materialCB_.data_->color = {0.5f, 0.5f, 0.5f, 1.0f};

	Float3 dir = Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate = {pitch, yaw, 0.0f};

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<OBBCollider>();
	collider_->SetTag("HomingMissile");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	パラメーター設定
	///

	// 攻撃力
	damage_ = 20;

	// 速さ
	speed_ = 0.3f;

	// 速度
	velocity_ = direciton * speed_;
}

void HomingMissile::Update() {
	///
	///	移動処理
	///

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

	///
	///	弾の向きを進行方向へ向ける
	///

	float yaw = std::atan2(newDir.x, newDir.z);
	float pitch = -std::asin(newDir.y);
	objectBullet_->transform_.rotate = {pitch, yaw, 0.0f};

	// パーティクル発生（後方から出るよう調整）
	float offsetDistance = -3.0f;
	Float3 offset = newDir * offsetDistance;
	ParticleEffectManager::GetInstance()->Emit("missileSmoke", objectBullet_->transform_.translate + offset, 1);

	// 時間経過による削除
	elapsedTime_ += 1.0f / 60.0f;
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

	// コライダー更新処理
	UpdateCollider();
	// オブジェクト更新
	objectBullet_->UpdateMatrix();
}

void HomingMissile::Draw() {
	// オブジェクト描画
	objectBullet_->Draw();
}

void HomingMissile::OnCollision(Collider* other) {
	Float3 bulletPos = this->objectBullet_->transform_.translate;

	// vs Player
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

	// vs Obstacle
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

		obb->center_ = center;
		obb->size_ = kColliderSize;

		// 回転軸の更新
		Matrix rotMat = Matrix::Rotation(objectBullet_->transform_.rotate);
		obb->xAxis_ = Float3::Normalize(Float3(rotMat.r[0][0], rotMat.r[1][0], rotMat.r[2][0]));
		obb->yAxis_ = Float3::Normalize(Float3(rotMat.r[0][1], rotMat.r[1][1], rotMat.r[2][1]));
		obb->zAxis_ = Float3::Normalize(Float3(rotMat.r[0][2], rotMat.r[1][2], rotMat.r[2][2]));
	}
}