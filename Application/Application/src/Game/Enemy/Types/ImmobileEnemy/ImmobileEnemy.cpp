#include "ImmobileEnemy.h"

// C++
#include <numbers>

// Engine
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <SoundManager.h>

// Application
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/System/ResultStats.h>
#include <src/Game/Utility/Utility.h>

void ImmobileEnemy::Initialize(const Cygnus::Float3& position, Player* player) {
	///
	/// オブジェクト生成
	///

	objectEnemy_ = std::make_unique<Cygnus::Object3D>();
	objectEnemy_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("ImmobileEnemy");
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = {0.0f, std::numbers::pi_v<float>, 0.0f}; // 手前を向いた状態でスポーン（一時的に）

	///
	///	コライダー生成
	///

	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("ImmobileEnemy");
	aabb->SetFollowTarget(&objectEnemy_->transform_.translate_);
	aabb->SetSize(kColliderSize);
	aabb->SetOwner(this);

	collider_ = std::move(aabb);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	///
	///	パラメーター設定
	///

	isDead_ = false;

	// HPの設定
	currentHP_ = kInitialHP;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定

	///
	///	ビヘイビアツリー構築
	///

	targetPlayer_ = player;
	BuildBehaviorTree();

	///
	///	UI生成
	/// 
	
	ui_ = std::make_unique<EnemyUIManager>();
	ui_->Initialize();

	// 発光演出クラス生成・初期化
	visualEffect_ = std::make_unique<EnemyVisualEffects>();
	visualEffect_->Initialize(objectEnemy_.get());
}

void ImmobileEnemy::Update() {
	///
	///	コライダー更新処理
	///

	collider_->Update();

	///
	///	オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();
	objectEnemy_->UpdateShadowMatrix();

	// 被弾時の発光演出
	visualEffect_->Update();

	///
	///	ビヘイビアツリーを評価
	///

	if (behaviorTree_) {
		behaviorTree_->Tick(this, Cygnus::TimeManager::GetInstance()->GetDeltaTime());
	}

	///
	///	UI更新
	/// 
	
	EnemyUIState state;
	state.worldPos = objectEnemy_->transform_.translate_;
	state.hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);
	state.reloadRatio = reloadTimer_ / kReloadTime;
	state.isReloading = isReloading_;

	ui_->Update(state);
}

void ImmobileEnemy::Draw() { objectEnemy_->Draw(); }

void ImmobileEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void ImmobileEnemy::DrawUI() { ui_->Draw(); }

void ImmobileEnemy::OnCollision(Cygnus::Collider* other) { 
	// 敵共通の衝突時処理
	Enemy::OnCollision(other); 
}

bool ImmobileEnemy::IsPlayerInSight() {
	if (!targetPlayer_)
		return false;

	if (targetPlayer_->IsDead())
		return false;

	///
	///	プレイヤーとの距離チェック
	///

	const Cygnus::Float3 playerPos = targetPlayer_->GetTranslate();
	const Cygnus::Float3 enemyPos = this->objectEnemy_->transform_.translate_;
	const Cygnus::Float3 direction = Cygnus::Float3::Normalize(playerPos - enemyPos);
	const float distanceToPlayer = Cygnus::Float3::Length(playerPos - enemyPos); // プレイヤーとの距離

	// プレイヤーが索敵範囲外の場合にはfalse
	if (distanceToPlayer > kSearchRange) {
		isPlayerVisible_ = false;
		return false;
	}

	///
	///	RayCastによる障害物チェック
	///

	Cygnus::RayCastHit hit{};
	bool rayCastHit = Cygnus::CollisionManager::GetInstance()->RayCast(enemyPos, direction, distanceToPlayer, &hit);
	// 障害物が間にある場合
	if (rayCastHit && hit.hitCollider->GetTag() == "Obstacle") {
		isPlayerVisible_ = false;
		return false;
	}

	// プレイヤーまで遮蔽なしで見えている
	isPlayerVisible_ = true;
	return true;
}

void ImmobileEnemy::SearchMotion() {
	searchStateTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	switch (searchState_) {
	// 索敵・回転時
	case SearchState::Rotate: {
		// 回転時間・方向をランダムに設定
		if (searchRotateDuration_ == 0.0f) {
			searchRotateDuration_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(kMinRotateTime, kMaxRotateTime);
			isRotatingRight_ = Cygnus::RandomGenerator::GetInstance()->RandomValueBool();
		}

		// 設定された方向への回転
		float rotateSpeed = (isRotatingRight_ ? 1.0f : -1.0f) * kRotationSpeed;
		objectEnemy_->transform_.rotate_.y += rotateSpeed;

		// 回転時間完了で待機状態へ移行
		if (searchStateTimer_ >= searchRotateDuration_) {
			searchStateTimer_ = 0.0f;
			searchRotateDuration_ = 0.0f;
			searchState_ = SearchState::Wait;
		}

		break;
	}
	// 索敵・待機時
	case SearchState::Wait: {
		// 待機時間をランダムに設定
		if (waitDuration_ == 0.0f) {
			waitDuration_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(kMinWaitTime, kMaxWaitTime);
		}

		// 待機時間完了で回転状態へ移行
		if (searchStateTimer_ >= waitDuration_) {
			searchStateTimer_ = 0.0f;
			waitDuration_ = 0.0f;

			searchState_ = SearchState::Rotate;
		}

		break;
	}
	}
}

void ImmobileEnemy::FaceToPlayer() {
	// プレイヤーへの方向ベクトルからY軸回転角度を計算
	Cygnus::Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸に回転を適用
	objectEnemy_->transform_.rotate_.y = targetAngle;
}

void ImmobileEnemy::Shoot() {
	// 発射方向
	Cygnus::Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	// 拡散をランダムに設定
	float randSpread = Cygnus::RandomGenerator::GetInstance()->RandomValue(-kBulletSpreadAngle, kBulletSpreadAngle);
	direction.x += randSpread;
	direction.z += randSpread;
	direction = Cygnus::Float3::Normalize(direction);

	// 弾の生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(objectEnemy_->transform_.translate_, direction, &Cygnus::ModelManager::GetInstance()->GetModel("Bullet"));
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	// 残弾を減らす
	bulletRemaining_--;
	// 次の発射までの待ち時間をリセット
	nextShotInterval_ = 0.0f;
}

void ImmobileEnemy::BuildBehaviorTree() {
	///
	///	索敵シーケンス関連
	///

	// 索敵モーション
	auto searchMotion = std::make_unique<Cygnus::ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float dt) -> Cygnus::BehaviorStatus {
		enemy->SearchMotion();
		return Cygnus::BehaviorStatus::Success; // 常に成功
	});

	// 距離・遮蔽チェック
	auto canSeePlayer = std::make_unique<Cygnus::ConditionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy) -> bool { return enemy->IsPlayerInSight(); });

	// searchSequence構築
	auto searchSequence = std::make_unique<Cygnus::SequenceNode<ImmobileEnemy>>();
	searchSequence->AddChild(std::move(searchMotion)); // 索敵モーション
	searchSequence->AddChild(std::move(canSeePlayer)); // 距離・遮蔽チェック

	///
	///	攻撃シーケンス関連
	///

	// プレイヤーの方向を向く
	auto faceToPlayer = std::make_unique<Cygnus::ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		enemy->FaceToPlayer();
		return Cygnus::BehaviorStatus::Success;
	});

	// リロード必要チェック
	auto needToReload = std::make_unique<Cygnus::ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		if (enemy->bulletRemaining_ <= 0 && !enemy->isReloading_) {
			enemy->isReloading_ = true;
			enemy->reloadTimer_ = 0.0f;
		}
		return Cygnus::BehaviorStatus::Success;
	});

	// リロード処理
	auto doReload = std::make_unique<Cygnus::ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		if (enemy->isReloading_) {
			enemy->reloadTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
			if (enemy->reloadTimer_ >= enemy->kReloadTime) {
				enemy->isReloading_ = false;
				enemy->bulletRemaining_ = enemy->kMaxBullet;
				enemy->reloadTimer_ = 0.0f;
			}
			return Cygnus::BehaviorStatus::Running;
		}
		return Cygnus::BehaviorStatus::Success;
	});

	// 射撃可能チェック
	auto canShoot = std::make_unique<Cygnus::ConditionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy) -> bool {
		enemy->nextShotInterval_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		return enemy->nextShotInterval_ >= enemy->kShotInterval && !enemy->isReloading_;
	});

	// 射撃
	auto shoot = std::make_unique<Cygnus::ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		enemy->Shoot();
		return Cygnus::BehaviorStatus::Success;
	});

	// attackSequence構築
	auto attackSequence = std::make_unique<Cygnus::SequenceNode<ImmobileEnemy>>();
	attackSequence->AddChild(std::move(faceToPlayer)); // プレイヤーの方向を向く
	attackSequence->AddChild(std::move(needToReload)); // リロード必要チェック
	attackSequence->AddChild(std::move(doReload));     // リロード
	attackSequence->AddChild(std::move(canShoot));     // 射撃可能チェック
	attackSequence->AddChild(std::move(shoot));        // 射撃

	///
	///	ルートシーケンス
	///

	// rootSequence構築
	auto rootSequence = std::make_unique<Cygnus::SequenceNode<ImmobileEnemy>>();
	rootSequence->AddChild(std::move(searchSequence)); // 索敵シーケンス
	rootSequence->AddChild(std::move(attackSequence)); // 攻撃シーケンス

	///
	///	behaviorTree
	///

	// ツリー構築
	behaviorTree_ = std::make_unique<Cygnus::BehaviorTree<ImmobileEnemy>>(std::move(rootSequence));
}