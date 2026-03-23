#include "PowerGenerator.h"

// Application
#include <src/Game/Bullet/Base/Bullet.h>

void PowerGenerator::Initialize(const Cygnus::Float3& position, Cygnus::ModelManager::ModelData* model) {
	// オブジェクト生成
	object_ = std::make_unique<Cygnus::Object3D>();
	object_->model_ = model;
	object_->transform_.translate_ = position;

	// コライダー生成
	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("PowerGenerator");
	aabb->SetFollowTarget(&object_->transform_.translate_);
	aabb->SetSize(kColliderSize);
	aabb->SetOwner(this);
	collider_ = std::move(aabb);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 常に更新しなくても良いので初期化時に1度のみ更新

	// UI生成
	ui_ = std::make_unique<PowerGeneratorUIManager>();
	ui_->Initialize();

	// パラメーター初期化
	currentHP_ = kMaxHP;
	isDead_ = false;
}

void PowerGenerator::Update() {
	// オブジェクト更新
	object_->UpdateMatrix();
	object_->UpdateShadowMatrix();

	// UI更新
	PowerGeneratorUIState state;
	state.worldPos = object_->transform_.translate_;
	state.hpRatio = static_cast<float>(currentHP_) / static_cast<float>(kMaxHP);
	ui_->Update(state);
}

void PowerGenerator::Draw() {
	// オブジェクト描画
	object_->Draw();
}

void PowerGenerator::DrawUI() {
	// UI描画
	ui_->Draw();
}

void PowerGenerator::DrawShadow() {
	// オブジェクトシャドウマップ描画
	object_->DrawShadow();
}

void PowerGenerator::OnCollision(Cygnus::Collider* other) {
	// vs PlayerBullet
	if(other->GetTag() == "PlayerBullet") {
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// 自身のHPを減らす
		currentHP_ -= damage;

		// 死亡判定
		if(currentHP_ <= 0) {
			isDead_ = true;
		}
	}
}

