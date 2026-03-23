#include "EventTrigger.h"

void EventTrigger::Initialize(const Cygnus::Float3& translate, const Cygnus::Float3& colliderSize) {
	// コライダー生成
	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("EventTrigger");
	aabb->SetSize(colliderSize);
	aabb->SetMin(translate - colliderSize);
	aabb->SetMax(translate + colliderSize);
	aabb->SetOwner(this);

	collider_ = std::move(aabb);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 初期フレーム衝突回避用更新
}

void EventTrigger::Update() {
	// コライダー更新処理
	collider_->Update();
}

void EventTrigger::OnCollision(Cygnus::Collider* other) {
	// アクティブ時のみ判定（再発火防止）
	if (isActive_) {
		// 自身がボス登場演出用トリガーの場合
		if (eventID_ == "BOSS_INTRO" && other->GetTag() == "Player") {
			isTriggered_ = true; // トリガーを引く
			isActive_ = false; // 再発火防止のため非アクティブ状態へ
		}
	}
}