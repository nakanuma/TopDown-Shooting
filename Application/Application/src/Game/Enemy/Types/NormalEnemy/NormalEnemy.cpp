#include "NormalEnemy.h"

// C++
#include <Windows.h>
#include <algorithm>
#include <cstdio>
#include <numbers>

// Engine
#include <Camera.h>
#include <Collider/CollisionManager.h>
#include <DirectXBase.h>
#include <Engine/3D/LineDrawer.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>
#include <SoundManager.h>

// Application
#include <src/Game/Bullet/Base/Bullet.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/System/ResultStats.h>
#include <src/Game/Utility/Utility.h>

// Externals
#include <ImguiWrapper.h>

void NormalEnemy::Initialize(const Cygnus::Float3& position, Player* player) {
	// オブジェクト生成
	objectEnemy_ = std::make_unique<Cygnus::Object3D>();
	objectEnemy_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("NormalEnemy");
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = { 0.0f, std::numbers::pi_v<float>, 0.0f }; // 手前を向いた状態でスポーン（一時的に）

	// コライダー生成・登録
	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("NormalEnemy");
	aabb->SetFollowTarget(&objectEnemy_->transform_.translate_);
	aabb->SetSize(kColliderSize);
	aabb->SetOwner(this);
	collider_ = std::move(aabb);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	// UI生成・初期化
	ui_ = std::make_unique<EnemyUIManager>();
	ui_->Initialize();

	// 発光演出クラス生成・初期化
	visualEffect_ = std::make_unique<EnemyVisualEffects>();
	visualEffect_->Initialize(objectEnemy_.get());

	// パラメーター設定
	currentHP_ = kInitialHP;
	maxHP_ = kInitialHP;
	targetPlayer_ = player;
}

void NormalEnemy::Update() {
	// オブジェクト更新
	objectEnemy_->UpdateMatrix();
	objectEnemy_->UpdateShadowMatrix();

	// コライダー更新
	collider_->Update();

	// UI更新
	EnemyUIState state;
	state.worldPos = objectEnemy_->transform_.translate_;
	state.hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);
	ui_->Update(state);

	// 発光演出更新
	visualEffect_->Update();
}

void NormalEnemy::Draw() { objectEnemy_->Draw(); }

void NormalEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void NormalEnemy::DrawUI() { ui_->Draw(); }

void NormalEnemy::OnCollision(Cygnus::Collider* other) {
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		// 被弾時の発光演出を開始
		visualEffect_->TriggerHitBlink();

		// PlayerBulletのdamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
		ResultStats::GetInstance()->AddHit();          // 弾が命中したことを記録
		ResultStats::GetInstance()->AddDamage(damage); // 与えたダメージを記録

		// HPが0になったら自身を死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;

			// 死亡時パーティクル発生
			Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate_, kDeathCrossCount, { 0.0f, 0.0f, 0.0f }, Cygnus::DegToRad(kDeathCrossAngle1)); // クロス片側
			Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate_, kDeathCrossCount, { 0.0f, 0.0f, 0.0f }, Cygnus::DegToRad(kDeathCrossAngle2)); // クロス片側

			ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録

			// 効果音発生
			Cygnus::SoundManager::GetInstance()->Play("enemy_dead", false, 0.25f);
		}
	}

	///
	/// vs Obstacle
	///
	if (other->GetTag() == "Obstacle") {
		Cygnus::AABBCollider* myAABB = dynamic_cast<Cygnus::AABBCollider*>(collider_.get());
		Cygnus::AABBCollider* otherAABB = dynamic_cast<Cygnus::AABBCollider*>(other);

		// 押し戻し処理
		if (myAABB && otherAABB) {
			// 押し戻しベクトル取得
			Cygnus::Float3 pushVec = myAABB->GetPushBackVector(*otherAABB);
			// 位置を補正
			objectEnemy_->transform_.translate_ += pushVec;

			// コライダーも更新しておく
			Cygnus::Float3 currentMin = myAABB->GetMin();
			Cygnus::Float3 currentMax = myAABB->GetMax();
			myAABB->SetMin(currentMin + pushVec);
			myAABB->SetMax(currentMax + pushVec);
		}
	}
}

void NormalEnemy::Debug() {
#ifdef USE_IMGUI
	
#endif
}