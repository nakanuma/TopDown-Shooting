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
	objectBullet_->transform_.translate = position;
	objectBullet_->transform_.scale = {radius_, radius_, radius_};

	// 進行方向から向きを計算して回転を設定
	Float3 dir = Float3::Normalize(direciton);
	float yaw = std::atan2(dir.x, dir.z);
	float pitch = -std::asin(dir.y);
	objectBullet_->transform_.rotate = {pitch, yaw, 0.0f};

	// ---------------------------------------------------------
	// コライダー生成・登録
	// ---------------------------------------------------------
	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("PlayerBullet");
	collider_->SetOwner(this);

	CollisionManager::GetInstance()->Register(collider_.get());

	// ---------------------------------------------------------
	// パラメーター設定
	// ---------------------------------------------------------
	damage_ = 10;// 攻撃力
	speed_ = 1.8f;// 弾速
	velocity_ = direciton * speed_;// 速度ベクトル
}

void PlayerBullet::Update() {
	// ---------------------------------------------------------
	// 位置更新
	// ---------------------------------------------------------
	objectBullet_->transform_.translate += velocity_;

	// ---------------------------------------------------------
	// 前フレーム位置履歴の更新（トレイル用）
	// ---------------------------------------------------------
	trailPoints_.push_back(objectBullet_->transform_.translate);
	if (trailPoints_.size() > kMaxTrailPoints) {
		trailPoints_.pop_front();
	}

	// ---------------------------------------------------------
	// 寿命更新
	// ---------------------------------------------------------
	elapsedTime_ += 1.0f / 60.0f;
	// 経過時間が寿命に達したら削除
	if (elapsedTime_ > kMaxLifeTime) {
		isDead_ = true;
	}

	// ---------------------------------------------------------
	// コライダー・行列更新処理
	// ---------------------------------------------------------
	UpdateCollider();
	objectBullet_->UpdateMatrix();
}

void PlayerBullet::Draw() {
	// ---------------------------------------------------------
	// オブジェクト描画
	// ---------------------------------------------------------
	objectBullet_->Draw();

	// ---------------------------------------------------------
	// トレイル（弾道エフェクト）描画
	// ---------------------------------------------------------
	DrawTrail();
}

void PlayerBullet::OnCollision(Collider* other) {
	Float3 bulletPos = this->objectBullet_->transform_.translate;
	auto rand = RandomGenerator::GetInstance();

	// ---------------------------------------------------------
	// 通常敵との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "NormalEnemy") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}

	// ---------------------------------------------------------
	// 固定敵との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "ImmobileEnemy") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}

	// ---------------------------------------------------------
	// ボスとの衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "BossEnemy") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}

	// ---------------------------------------------------------
	// 障害物との衝突
	// ---------------------------------------------------------
	if (other->GetTag() == "Obstacle") {
		// ヒットエフェクト
		ParticleEffectManager::GetInstance()->Emit("backscatter", bulletPos, rand->RandomValue(3, 4), velocity_);

		// 死亡させる
		isDead_ = true;
	}
}

void PlayerBullet::UpdateCollider() {
	if (SphereCollider* sphere = dynamic_cast<SphereCollider*>(collider_.get())) {
		// 位置と半径をオブジェクトに追従させる
		sphere->center_ = objectBullet_->transform_.translate;
		sphere->radius_ = radius_;
	}
}

void PlayerBullet::DrawTrail() {
	Float4 headColor = {1.0f, 1.0f, 1.0f, 1.0f};
	Float4 tailColor = {1.0f, 1.0f, 1.0f, 0.0f};

	// [0]と[1], [1]と[2]... といったように全てのポイントを繋ぐ線を作る
	for (size_t i = 1; i < trailPoints_.size(); ++i) {
		// 線分の位置に応じた割合を計算
		float t0 = static_cast<float>(i - 1) / (trailPoints_.size());
		float t1 = static_cast<float>(i) / (trailPoints_.size() - 1);

		// 線の両端の色を補間
		Float4 c0 = Float4::Lerp(tailColor, headColor, t0); // この線分での末尾の色
		Float4 c1 = Float4::Lerp(tailColor, headColor, t1); // この線分での先頭の色

		// トレイル線の登録
		LineDrawer::GetInstance()->RegisterTracer(trailPoints_[i - 1], trailPoints_[i], 0.5f, c1, c0);
	}
}