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

void NormalEnemy::Initialize(const Cygnus::Float3& position, Player* player) {}

void NormalEnemy::Initialize(const Cygnus::Float3& position, Cygnus::ModelManager::ModelData* model, Player* player, Cygnus::BehaviorTree<NormalEnemy>* masterTree)
{
	///
	/// オブジェクト生成
	///

	objectEnemy_ = std::make_unique<Cygnus::Object3D>();
	objectEnemy_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("NormalEnemy");
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = { 0.0f, std::numbers::pi_v<float>, 0.0f }; // 手前を向いた状態でスポーン（一時的に）
	objectEnemy_->materialCB_.data_->emissiveColor = kHitBlinkColor;

	///
	///	コライダー生成
	///

	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("NormalEnemy");
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
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定（全Enemyクラス共通）

	targetPlayer_ = player;

	spawnPosition_ = position; // スポーン地点を記録

	bb_.currentAmmo = kMagazineSize; // 初期マガジン設定

	///
	///	調整パラメーター登録
	///

	SetConfigPath("Enemy/normalEnemyConfig.json"); // ファイルパス設定
	InitConfig();                                  // 初回読み込み

	///
	///	ビヘイビアツリー構築
	///

	// マスターツリーを複製して自分専用のインスタンスを作成
	if(masterTree) {
		behaviorTree_ = masterTree;
	}

	///
	///	UI生成
	/// 

	ui_ = std::make_unique<EnemyUIManager>();
	ui_->Initialize();
}

void NormalEnemy::Update() {
	///
	/// コライダー更新処理
	///

	collider_->Update();

	///
	/// オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();
	objectEnemy_->UpdateShadowMatrix();

	// 被弾時の発光演出
	HandleHitBlink();

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
	state.reloadRatio = bb_.reloadTimer / kReloadTime;
	state.isReloading = bb_.isReloading;

	ui_->Update(state);
}

void NormalEnemy::Draw() { objectEnemy_->Draw(); }

void NormalEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void NormalEnemy::DrawUI() { ui_->Draw(); }

void NormalEnemy::OnCollision(Cygnus::Collider* other) {
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		// デバッグでプレイヤー発見状態にする
		if (!bb_.isPlayerDetected) {
			bb_.isPlayerDetected = true;
		}

		// 被弾時の発光演出を開始
		if (!isDead_) {
			isHitBlink_ = true;
			hitBlinkPhase_ = HitBlinkPhase::BlinkIn;
			hitBlinkTimer_ = 0.0f;
		}

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
			Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate_, kDeathCrossCount, {0.0f, 0.0f, 0.0f}, Cygnus::DegToRad(kDeathCrossAngle1));
			Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate_, kDeathCrossCount, {0.0f, 0.0f, 0.0f}, Cygnus::DegToRad(kDeathCrossAngle2));

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
	// 索敵中の視界を可視化
	DrawDebugSight();

	// 調整パラメーター
	DrawConfigWindow("NormalEnemyConfig");
#endif
}

void NormalEnemy::MoveAlongPath(const std::vector<Waypoint*>& path, float speed) {
	// 経路に移動先がなければ終了
	if (path.size() < 2)
		return;

	// [0]は敵の位置なので[1]が次に向かう目標ウェイポイントになる
	Waypoint* nextWP = path[1];
	// 2個先の位置を補間した座標を目標にする
	Cygnus::Float3 targetPos = nextWP->GetPosition();
	if (path.size() > 2) {
		targetPos = (path[1]->GetPosition() + path[2]->GetPosition()) * kPathInterpolation;
	}
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(targetPos - objectEnemy_->transform_.translate_);
	dir.y = 0.0f;
	dir = Cygnus::Float3::Normalize(dir);

	// 移動
	objectEnemy_->transform_.translate_ += dir * speed * Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 向き補間
	Cygnus::Float3 lookDir = dir;

	float currentYaw = objectEnemy_->transform_.rotate_.y;
	float targetYaw = std::atan2(lookDir.x, lookDir.z);

	// -π ~ πに正規化
	float deltaYaw = targetYaw - currentYaw;
	while (deltaYaw > Cygnus::PIf)
		deltaYaw -= 2.0f * Cygnus::PIf;
	while (deltaYaw < -Cygnus::PIf)
		deltaYaw += 2.0f * Cygnus::PIf;

	float newYaw = currentYaw + deltaYaw * kTurnSpeed * Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	objectEnemy_->transform_.rotate_.y = newYaw;
}

bool NormalEnemy::IsPlayerInSight() {
	if (!targetPlayer_)
		return false;

	if (targetPlayer_->IsDead())
		return false;

	Cygnus::Float3 enemyPos = this->objectEnemy_->transform_.translate_;
	Cygnus::Float3 playerPos = targetPlayer_->GetTranslate();
	Cygnus::Float3 toPlayer = playerPos - enemyPos;

	///
	///	距離チェック
	///

	float distance = Cygnus::Float3::Length(toPlayer);
	// Playerが範囲外ならfalse
	if (distance > kSearchRadius) {
		return false;
	}

	///
	///	FOVチェック
	///

	toPlayer = Cygnus::Float3::Normalize(toPlayer);

	// 前方向ベクトル（Y軸回転のみで考慮）
	Cygnus::Float3 forward = {std::sinf(objectEnemy_->transform_.rotate_.y), 0.0f, std::cosf(objectEnemy_->transform_.rotate_.y)};

	forward = Cygnus::Float3::Normalize(forward);

	// 内積から角度を求める
	float dot = Cygnus::Float3::Dot(forward, toPlayer);

	// ラジアンに変換
	float angleRad = std::acosf(dot);
	float angleDeg = angleRad * 180.0f / Cygnus::PIf;

	// 扇形角度チェック
	if (angleDeg > (kSearchFovDeg * 0.5f)) {
		return false;
	}

	///
	///	RayCastによる障害物チェック
	///

	Cygnus::RayCastHit hit{};
	bool rayCast = Cygnus::CollisionManager::GetInstance()->RayCast(enemyPos, toPlayer, distance, &hit);

	if (rayCast && hit.hitCollider->GetTag() == "Obstacle") {
		return false;
	}

	// プレイヤー発見状態にする
	bb_.isPlayerDetected = true;
	return true;
}

void NormalEnemy::DrawDebugSight() {
	// 視界にプレイヤーがいれば赤色に
	Cygnus::Float4 color;
	if (IsPlayerInSight()) {
		color = kDebugSightColorDetect;
	} else {
		color = kDebugSightColorNormal;
	}

	Cygnus::Float3 center = objectEnemy_->transform_.translate_;

	// 前方向ベクトル（Y軸回転のみで考慮）
	Cygnus::Float3 forward = {std::sinf(objectEnemy_->transform_.rotate_.y), 0.0f, std::cosf(objectEnemy_->transform_.rotate_.y)};
	forward = Cygnus::Float3::Normalize(forward);

	// 左端の方向ベクトル
	float halfFovRad = (kSearchFovDeg * 0.5f) * Cygnus::PIf / 180.0f;
	float baseAngle = std::atan2f(forward.z, forward.x);

	float startAngle = baseAngle - halfFovRad;
	float endAngle = baseAngle + halfFovRad;

	// 扇形を分割して線を描画
	Cygnus::Float3 firstPoint = {center.x + std::cosf(startAngle) * kSearchRadius, center.y, center.z + std::sinf(startAngle) * kSearchRadius};

	Cygnus::Float3 prevPoint = firstPoint;

	for (uint32_t i = 1; i <= kDebugSightSegments; i++) {
		float t = static_cast<float>(i) / kDebugSightSegments;
		float angle = startAngle + (endAngle - startAngle) * t;

		Cygnus::Float3 nextPoint = {center.x + std::cosf(angle) * kSearchRadius, center.y, center.z + std::sinf(angle) * kSearchRadius};

		// 円弧の線分
		Cygnus::LineDrawer::GetInstance()->RegisterLine(prevPoint, nextPoint, color);

		prevPoint = nextPoint;
	}

	// 扇の骨組み（中心から弧の両端）
	Cygnus::Float3 lastPoint = prevPoint;
	Cygnus::LineDrawer::GetInstance()->RegisterLine(center, firstPoint, color);
	Cygnus::LineDrawer::GetInstance()->RegisterLine(center, lastPoint, color);
}

Cygnus::BehaviorStatus NormalEnemy::RandomPatrol() {
	Cygnus::BehaviorStatus status = Cygnus::BehaviorStatus::Running;

	///
	///	移動先のウェイポイントを取得
	///

	if (!bb_.currentTargetWP) {
		// 移動先ウェイポイント候補
		std::vector<Waypoint*> candidates;
		for (auto& wp : WaypointManager::GetInstance()->GetWaypoints()) {
			float distFromSpawn = Cygnus::Float3::Length(wp->GetPosition() - spawnPosition_);                        // スポーン地点からウェイポイントまでの距離
			float distFromCurrent = Cygnus::Float3::Length(wp->GetPosition() - objectEnemy_->transform_.translate_); // 現在地点からウェイポイントまでの距離

			// スポーン地点から一定範囲内にあるかつ、現在位置から一定距離離れたウェイポイントのみを収集
			if (distFromSpawn <= kPatrolRange && distFromCurrent >= kMinPatrolRange) {
				candidates.push_back(wp.get());
			}
		}

		// 候補からランダムに1つ選択
		uint32_t randIndex = Cygnus::RandomGenerator::GetInstance()->RandomValue(0, static_cast<int>(candidates.size()) - 1);
		bb_.currentTargetWP = candidates[randIndex];

		status = Cygnus::BehaviorStatus::Running;

		///
		/// ターゲットのウェイポイントまで移動
		///
	} else {
		// 経路探索
		Waypoint* startWP = WaypointManager::GetInstance()->FindClosestWaypoint(objectEnemy_->transform_.translate_);
		std::vector<Waypoint*> path = WaypointManager::GetInstance()->FindPath(startWP, bb_.currentTargetWP);
		if (!path.empty()) {
			// 移動
			MoveAlongPath(path, kPatrolMoveSpeed);

			// 目標に到達したらターゲットをクリア
			Cygnus::Float3 targetPos = bb_.currentTargetWP->GetPosition();
			if (Cygnus::Float3::Length(targetPos - objectEnemy_->transform_.translate_) < kWaypointReachDistance) {
				bb_.currentTargetWP = nullptr;

				status = Cygnus::BehaviorStatus::Success; // 成功
			}
		}
	}

	return status;
}

Cygnus::BehaviorStatus NormalEnemy::RandomRotate() {
	if (bb_.rotateTimer <= 0.0f) {
		// 回転方向をランダムに決める
		bb_.rotateDirection = Cygnus::RandomGenerator::GetInstance()->RandomValueBool() ? 1.0f : -1.0f;

		// 回転時間をランダムに決める
		bb_.rotateTimer = Cygnus::RandomGenerator::GetInstance()->RandomValue(kRotateTimeMin, kRotateTimeMax);
	}

	// 回転処理
	objectEnemy_->transform_.rotate_.y += bb_.rotateDirection * Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// タイマー減少
	bb_.rotateTimer -= Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 時間が残っていれば実行中
	if (bb_.rotateTimer > 0.0f) {
		return Cygnus::BehaviorStatus::Running;
	}

	// 終了したら成功
	bb_.rotateTimer = 0.0f;
	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemy::FacePlayer() {
	// プレイヤーへの方向ベクトルからY軸回転角度の計算
	Cygnus::Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸回転を適用
	objectEnemy_->transform_.rotate_.y = targetAngle;

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemy::Shoot() {
	float dt = Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// memo : リロード開始/終了時に移動先の経路探索を挟む

	// リロード中処理
	if (bb_.isReloading) {
		bb_.reloadTimer -= dt;
		// リロード終了時に弾を込める
		if (bb_.reloadTimer <= 0.0f) {
			bb_.isReloading = false;
			bb_.currentAmmo = kMagazineSize;
		}
		return Cygnus::BehaviorStatus::Running;
	}

	// バースト間のインターバル（次のバースト撃ちまで待機）
	if (bb_.fireCooldown > 0.0f) {
		bb_.fireCooldown -= dt;
		return Cygnus::BehaviorStatus::Running;
	}

	// バースト内のインターバル（バースト射撃中）
	if (bb_.burstCooldown > 0.0f) {
		bb_.burstCooldown -= dt;
		return Cygnus::BehaviorStatus::Running;
	}

	// 弾切れならリロード開始
	if (bb_.currentAmmo <= 0) {
		bb_.isReloading = true;
		bb_.reloadTimer = kReloadTime; // リロード時間セット

		return Cygnus::BehaviorStatus::Running;
	}

	// 弾の発射処理
	Cygnus::Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	// 拡散角をランダムに設定
	float randSpread = Cygnus::RandomGenerator::GetInstance()->RandomValue(-kBulletSpreadAngle, kBulletSpreadAngle);
	direction.x += randSpread;
	direction.z += randSpread;
	direction = Cygnus::Float3::Normalize(direction);
	// 弾の生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(objectEnemy_->transform_.translate_, direction, &Cygnus::ModelManager::GetInstance()->GetModel("Bullet"));
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	// カウント更新
	bb_.currentAmmo--;
	bb_.burstCount++;

	// バースト射撃中管理
	if (bb_.burstCount < kBurstSize) {
		// バースト内クールタイムをセット
		bb_.burstCooldown = kBurstInterval;

		// バースト射撃終了
	} else {
		bb_.burstCount = 0;
		bb_.fireCooldown = kFireInterval; // バースト間クールタイムをセット
	}

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemy::MoveToPlayer() {
	if (targetPlayer_->IsDead())
		return Cygnus::BehaviorStatus::Failure;

	// 経路探索でプレイヤーに移動
	Waypoint* start = WaypointManager::GetInstance()->FindClosestWaypoint(objectEnemy_->transform_.translate_);
	Waypoint* goal = WaypointManager::GetInstance()->FindClosestWaypoint(targetPlayer_->GetTranslate());

	if (!start || !goal)
		return Cygnus::BehaviorStatus::Failure;

	std::vector<Waypoint*> path = WaypointManager::GetInstance()->FindPath(start, goal);
	MoveAlongPath(path, kMoveSpeed);

	return Cygnus::BehaviorStatus::Running;
}