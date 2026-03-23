#include "Teleporter.h"

// Application
#include <src/Game/Player/Player.h>

// Engine
#include <TimeManager.h>
#include <ParticleEffect/ParticleEffectManager.h>

void Teleporter::Initialize(const Cygnus::Float3& position, Cygnus::ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	object_ = std::make_unique<Cygnus::Object3D>();
	object_->model_ = model;
	object_->transform_.translate_ = position;
	object_->materialCB_.data_->useEnvironmentMap = true;
	object_->materialCB_.data_->environmentStrength = kEnvironmentStrength;

	///
	///	コライダー生成
	///

	auto sphere = std::make_unique<Cygnus::SphereCollider>();
	sphere->SetTag("Teleporter");
	sphere->SetFollowTarget(&object_->transform_.translate_);
	sphere->SetRadius(kRadius);
	sphere->SetOwner(this);

	collider_ = std::move(sphere);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）
}

void Teleporter::Update() {
	///
	///	コライダー更新処理
	///

	collider_->Update();

	///
	///	オブジェクト更新処理
	///

	object_->UpdateMatrix();
	object_->UpdateShadowMatrix();

	///
	///	パーティクル発生処理
	/// 

	if (isActive_) {
		emitTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		if (emitTimer_ >= kParticleEmitInterval) {
			// 発生間隔分を減算
			emitTimer_ -= kParticleEmitInterval;
			Cygnus::ParticleEffectManager::GetInstance()->Emit("teleporterRing", GetTranslate() + kParticleEmitOffset, kRingParticleCount);
		}
	}
}

void Teleporter::Draw() {
	// オブジェクト描画
	object_->Draw();
}

void Teleporter::DrawShadow() {
	// シャドウマップ描画
	object_->DrawShadow();
}

void Teleporter::OnCollision(Cygnus::Collider* other) {
	// vs Player
	if (other->GetTag() == "Player") {
		Player* player = static_cast<Player*>(other->GetOwner());

		// このテレポーターがゴールの場合
		if (IsGoal() && isActive_) {
			// コールバック関数を呼び出す
			if (onGoalCallback_) {
				onGoalCallback_();
			}
			// 無効化状態にする
			isActive_ = false;
		}

		// このテレポーターが次ステージ移行の場合
		if(pairID_ == "NEXT" && isActive_) {
			// コールバック関数を呼び出す
			if(onNextFloorCallback_) {
				onNextFloorCallback_();
			}
			// 無効化状態にする
			isActive_ = false;
		}
	}
}