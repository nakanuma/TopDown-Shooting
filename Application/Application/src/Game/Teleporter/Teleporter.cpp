#include "Teleporter.h"

// Application
#include <src/Game/Player/Player.h>

// Engine
#include <TimeManager.h>
#include <ParticleEffect/ParticleEffectManager.h>

void Teleporter::Initialize(const Float3& position, ModelManager::ModelData* model) {
	///
	///	オブジェクト生成
	///

	object_ = std::make_unique<Object3D>();
	object_->model_ = model;
	object_->transform_.translate_ = position;
	object_->materialCB_.data_->useEnvironmentMap = true;
	object_->materialCB_.data_->environmentStrength = kEnvironmentStrength;

	///
	///	コライダー生成
	///

	auto sphere = std::make_unique<SphereCollider>();
	sphere->SetTag("Teleporter");
	sphere->SetFollowTarget(&object_->transform_.translate_);
	sphere->SetRadius(kRadius);
	sphere->SetOwner(this);

	collider_ = std::move(sphere);
	CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	// 使用可能にしておく
	isActive_ = true;
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
		emitTimer_ += TimeManager::GetInstance()->GetDeltaTime();
		if (emitTimer_ >= kParticleEmitInterval) {
			// 発生間隔分を減算
			emitTimer_ -= kParticleEmitInterval;
			ParticleEffectManager::GetInstance()->Emit("teleporterRing", GetTranslate() + kParticleEmitOffset, kRingParticleCount);
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

void Teleporter::OnCollision(Collider* other) {
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

			// 通常テレポーターの場合
		} else {
			// プレイヤーをペアのテレポーター位置へ送る（todo : 今は直接移動なので、ここで数秒待ってテレポートする演出を入れる）
			if (pair_ && pair_->isActive_) {
				player->SetTranslate({
					pair_->GetTranslate().x,
					player->GetTranslate().y,
					pair_->GetTranslate().z,
					});

				// 使用したテレポーターは無効化する
				this->isActive_ = false;
				pair_->isActive_ = false;
			}
		}
	}
}