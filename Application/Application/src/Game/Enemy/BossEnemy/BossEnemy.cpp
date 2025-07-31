#include "BossEnemy.h"

// C++
#include <numbers>

// Application
#include <src/Game/Player/Player.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void BossEnemy::Initialize(const Float3& position, ModelManager::ModelData* model)
{
	///
	///	基盤機能生成
	///

	DirectXBase* dxBase = DirectXBase::GetInstance();

	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

	///
	/// オブジェクト生成
	///

	objectEnemy_ = std::make_unique<Object3D>();
	objectEnemy_->model_ = model;
	objectEnemy_->transform_.translate = position;
	objectEnemy_->transform_.scale = { 1.0f, 1.0f, 1.0f };
	objectEnemy_->transform_.rotate = { 0.0f, std::numbers::pi_v<float>, 0.0f }; // 手前を向いた状態でスポーン（一時的に）
	objectEnemy_->materialCB_.data_->color = { 0.2f, 0.2f, 0.2f, 1.0f };

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<OBBCollider>();
	collider_->SetTag("BossEnemy");
	collider_->SetOwner(this);
	colliderSize_ = { 5.0f, 3.0f, 5.0f };

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	パラメーター設定
	///

	isDead_ = false;

	// HPの設定
	currentHP_ = 100;
	maxHP_ = currentHP_;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void BossEnemy::Update()
{
	///
	///	プレイヤー方向へ向く（デバッグで一時的に）
	///

	//// プレイヤーへの方向ベクトル
	//Float3 toPlayer = player->GetTranslate() - objectEnemy_->transform_.translate;
	//// 方向ベクトルからY軸回転角度を計算
	//float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	//// Y軸に回転を適用
	//objectEnemy_->transform_.rotate.y = targetAngle;

	///
	/// コライダー更新処理
	///

	UpdateCollider();

	///
	/// オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void BossEnemy::Draw()
{
	// オブジェクト描画処理
	objectEnemy_->Draw();
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void BossEnemy::DrawUI()
{}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void BossEnemy::Debug() {
	ImGui::Begin("BossEnemy");
	ImGui::DragFloat3("translate", &objectEnemy_->transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectEnemy_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &objectEnemy_->transform_.scale.x, 0.01f);
	ImGui::End();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void BossEnemy::OnCollision(Collider* other)
{
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		// PlayerBulletのdamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;

		// HPが0になったら死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;
		}
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void BossEnemy::UpdateCollider()
{
	//if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
	//	Float3 center = objectEnemy_->transform_.translate;
	//	Float3 size = colliderSize_;

	//	// min
	//	aabb->min_ = center - size;
	//	aabb->max_ = center + size;
	//}

	if (OBBCollider* obb = dynamic_cast<OBBCollider*>(collider_.get())) {
		Float3 center = objectEnemy_->transform_.translate;
		Float3 size = colliderSize_;

		obb->center_ = center;
		obb->size_ = colliderSize_;

		// 回転軸の更新
		Matrix rotMat = Matrix::Rotation(objectEnemy_->transform_.rotate);
		obb->xAxis_ = Float3::Normalize(Float3(rotMat.r[0][0], rotMat.r[1][0], rotMat.r[2][0]));
		obb->yAxis_ = Float3::Normalize(Float3(rotMat.r[0][1], rotMat.r[1][1], rotMat.r[2][1]));
		obb->zAxis_ = Float3::Normalize(Float3(rotMat.r[0][2], rotMat.r[1][2], rotMat.r[2][2]));
	}
}
