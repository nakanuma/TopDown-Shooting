#include "ImmobileEnemy.h"

// C++
#include <numbers>

// Engine
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Application
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/System/ResultStats.h>
#include <src/Game/Utility/Utility.h>

void ImmobileEnemy::Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) {
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
	objectEnemy_->transform_.scale = {1.0f, 1.0f, 1.0f};
	objectEnemy_->transform_.rotate = {0.0f, std::numbers::pi_v<float>, 0.0f}; // 手前を向いた状態でスポーン（一時的に）

	///
	///	コライダー生成
	///

	colliderSize_ = {1.0f, 2.0f, 1.0f};

	auto aabb = std::make_unique<AABBCollider>();
	aabb->SetTag("ImmobileEnemy");
	aabb->SetFollowTarget(&objectEnemy_->transform_.translate);
	aabb->SetSize(colliderSize_);
	aabb->SetOwner(this);

	collider_ = std::move(aabb);
	CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	///
	///	スプライト生成
	///

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("white.png");
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({0.0f, 0.0f, 0.0f, 1.0f}); // 黒

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("white.png");
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({0.0f, 1.0f, 0.5f, 1.0f}); // 緑

	// リロード表示
	uint32_t textureReload = TextureManager::Load("white.png");
	spriteReload_ = std::make_unique<Sprite>();
	spriteReload_->Initialize(spriteCommon_.get(), textureReload);
	spriteReload_->SetSize(kReloadSize);
	spriteReload_->SetColor({1.0f, 1.0f, 1.0f, 1.0f}); // 白

	///
	///	パラメーター設定
	///

	isDead_ = false;

	// HPの設定
	currentHP_ = 40;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定

	///
	///	ビヘイビアツリー構築
	///

	targetPlayer_ = player;
	BuildBehaviorTree();
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

	///
	///	ビヘイビアツリーを評価
	///

	if (behaviorTree_) {
		behaviorTree_->Tick(this, TimeManager::GetInstance()->GetDeltaTime());
	}

	///
	///	スプライト更新処理
	///

	// HPバー（後景）更新
	spriteHPBackground_->Update();
	// HPバー（前景）更新
	spriteHPForeground_->Update();

	// リロード表示更新
	spriteReload_->Update();
}

void ImmobileEnemy::Draw() {
	// オブジェクト描画
	objectEnemy_->Draw();
}

void ImmobileEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void ImmobileEnemy::DrawUI() {
	// オブジェクトのワールド座標->スクリーン座標に変換
	Float3 screenPosition = Utility::WorldToScreen(objectEnemy_->transform_.translate);
	// 上にずらす分のオフセット
	const float kOffsetHPBar = 90.0f;

	// HP割合
	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	///
	/// HPバー（後景）描画
	///

	// スクリーン座標をセット
	spriteHPBackground_->SetPosition({
	    screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定,
	    screenPosition.y - kOffsetHPBar         // オフセット分上にずらす
	});
	spriteHPBackground_->Draw();

	///
	///	HPバー（前景）描画
	///

	// 現在HPに応じてサイズ変更
	Float2 hpBarForegroundSize = {kHPBarSize.x * hpRatio, kHPBarSize.y};
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	// スクリーン座標をセット
	spriteHPForeground_->SetPosition({
	    screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定
	    screenPosition.y - kOffsetHPBar         // オフセット分上にずらす
	});
	spriteHPForeground_->Draw();

	///
	///	リロード表示
	///

	// 上にずらす分のオフセット
	const float kOffsetReload = 60.0f;

	// リロード時間割合
	float reloadRatio = reloadTimer_ / kReloadTime;

	// リロード時間に応じてサイズ変更
	spriteReload_->SetSize({kReloadSize.x - (kReloadSize.x * reloadRatio), kReloadSize.y});

	// スクリーン座標をセット
	spriteReload_->SetPosition(
	    {screenPosition.x - kReloadSize.x / 2.0f, // リロード表示が中心になるよう設定
	     screenPosition.y - kOffsetReload});

	// リロード時のみ描画
	if (isReloading_) {
		spriteReload_->Draw();
	}
}

void ImmobileEnemy::OnCollision(Collider* other) {
	///
	///	vs PlayerBullet
	///

	if (other->GetTag() == "PlayerBullet") {
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
			ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate, 3, { 0.0f, 0.0f, 0.0f }, DegToRad(45)); // クロス片側
			ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate, 3, { 0.0f, 0.0f, 0.0f }, DegToRad(135)); // クロス片側

			ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録
		}
	}
}

bool ImmobileEnemy::IsPlayerInSight() {
	if (!targetPlayer_)
		return false;

	if (targetPlayer_->IsDead())
		return false;

	///
	///	プレイヤーとの距離チェック
	///

	const Float3 playerPos = targetPlayer_->GetTranslate();
	const Float3 enemyPos = this->objectEnemy_->transform_.translate;
	const Float3 direction = Float3::Normalize(playerPos - enemyPos);
	const float distanceToPlayer = Float3::Length(playerPos - enemyPos); // プレイヤーとの距離

	// プレイヤーが索敵範囲外の場合にはfalse
	if (distanceToPlayer > searchRange_) {
		isPlayerVisible_ = false;
		return false;
	}

	///
	///	RayCastによる障害物チェック
	///

	RayCastHit hit{};
	bool rayCastHit = CollisionManager::GetInstance()->RayCast(enemyPos, direction, distanceToPlayer, &hit);
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
	searchStateTimer_ += TimeManager::GetInstance()->GetDeltaTime();

	switch (searchState_) {
	// 索敵・回転時
	case SearchState::Rotate: {
		// 回転時間・方向をランダムに設定
		if (searchRotateDuration_ == 0.0f) {
			searchRotateDuration_ = RandomGenerator::GetInstance()->RandomValue(kMinRotateTime, kMaxRotateTime);
			isRotatingRight_ = RandomGenerator::GetInstance()->RandomValueBool();
		}

		// 設定された方向への回転
		float rotateSpeed = (isRotatingRight_ ? 1.0f : -1.0f) * rotationSpeed_;
		objectEnemy_->transform_.rotate.y += rotateSpeed;

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
			waitDuration_ = RandomGenerator::GetInstance()->RandomValue(kMinWaitTime, kMaxWaitTime);
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
	Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸に回転を適用
	objectEnemy_->transform_.rotate.y = targetAngle;
}

void ImmobileEnemy::Shoot() {
	// 発射方向
	Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	// 拡散をランダムに設定
	float randSpread = RandomGenerator::GetInstance()->RandomValue(-bulletSpreadAngle_, bulletSpreadAngle_);
	direction.x += randSpread;
	direction.z += randSpread;
	direction = Float3::Normalize(direction);

	// 弾の生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(objectEnemy_->transform_.translate, direction, &ModelManager::GetInstance()->GetModel("Bullet"));
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
	auto searchMotion = std::make_unique<ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float dt) -> BehaviorStatus {
		enemy->SearchMotion();
		return BehaviorStatus::Success; // 常に成功
	});

	// 距離・遮蔽チェック
	auto canSeePlayer = std::make_unique<ConditionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy) -> bool { return enemy->IsPlayerInSight(); });

	// searchSequence構築
	auto searchSequence = std::make_unique<SequenceNode<ImmobileEnemy>>();
	searchSequence->AddChild(std::move(searchMotion)); // 索敵モーション
	searchSequence->AddChild(std::move(canSeePlayer)); // 距離・遮蔽チェック

	///
	///	攻撃シーケンス関連
	///

	// プレイヤーの方向を向く
	auto faceToPlayer = std::make_unique<ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		enemy->FaceToPlayer();
		return BehaviorStatus::Success;
	});

	// リロード必要チェック
	auto needToReload = std::make_unique<ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		if (enemy->bulletRemaining_ <= 0 && !enemy->isReloading_) {
			enemy->isReloading_ = true;
			enemy->reloadTimer_ = 0.0f;
		}
		return BehaviorStatus::Success;
	});

	// リロード処理
	auto doReload = std::make_unique<ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		if (enemy->isReloading_) {
			enemy->reloadTimer_ += TimeManager::GetInstance()->GetDeltaTime();
			if (enemy->reloadTimer_ >= enemy->kReloadTime) {
				enemy->isReloading_ = false;
				enemy->bulletRemaining_ = enemy->kMaxBullet;
				enemy->reloadTimer_ = 0.0f;
			}
			return BehaviorStatus::Running;
		}
		return BehaviorStatus::Success;
	});

	// 射撃可能チェック
	auto canShoot = std::make_unique<ConditionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy) -> bool {
		enemy->nextShotInterval_ += TimeManager::GetInstance()->GetDeltaTime();
		return enemy->nextShotInterval_ >= enemy->kShotInterval && !enemy->isReloading_;
	});

	// 射撃
	auto shoot = std::make_unique<ActionNode<ImmobileEnemy>>([](ImmobileEnemy* enemy, float) {
		enemy->Shoot();
		return BehaviorStatus::Success;
	});

	// attackSequence構築
	auto attackSequence = std::make_unique<SequenceNode<ImmobileEnemy>>();
	attackSequence->AddChild(std::move(faceToPlayer)); // プレイヤーの方向を向く
	attackSequence->AddChild(std::move(needToReload)); // リロード必要チェック
	attackSequence->AddChild(std::move(doReload));     // リロード
	attackSequence->AddChild(std::move(canShoot));     // 射撃可能チェック
	attackSequence->AddChild(std::move(shoot));        // 射撃

	///
	///	ルートシーケンス
	///

	// rootSequence構築
	auto rootSequence = std::make_unique<SequenceNode<ImmobileEnemy>>();
	rootSequence->AddChild(std::move(searchSequence)); // 索敵シーケンス
	rootSequence->AddChild(std::move(attackSequence)); // 攻撃シーケンス

	///
	///	behaviorTree
	///

	// ツリー構築
	behaviorTree_ = std::make_unique<BehaviorTree<ImmobileEnemy>>(std::move(rootSequence));
}