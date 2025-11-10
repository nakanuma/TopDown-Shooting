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
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Application
#include <src/Game/Bullet/Base/Bullet.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/System/ResultStats.h>
#include <src/Game/Utility/Utility.h>

// Externals
#include <ImguiWrapper.h>

void NormalEnemy::Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) {
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

	colliderSize_ = { 1.0f, 2.0f, 1.0f };

	auto aabb = std::make_unique<AABBCollider>();
	aabb->SetTag("NormalEnemy");
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
	spriteReload_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});

	///
	///	パラメーター設定
	///

	isDead_ = false;

	// HPの設定
	currentHP_ = 40;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定（全Enemyクラス共通）

	targetPlayer_ = player;

	spawnPosition_ = position; // スポーン地点を記録

	currentAmmo_ = kMagazineSize; // 初期マガジン設定

	///
	///	調整パラメーター登録
	///

	RegisterParam("searchRadius", &searchRadius_, 0.0f, 100.0f, 0.01f);
	RegisterParam("searchFovDeg", &searchFovDeg_, 0.0f, 360.0f, 1.00f);

	SetConfigPath("Enemy/normalEnemyConfig.json"); // ファイルパス設定
	InitConfig();                                  // 初回読み込み

	///
	///	ビヘイビアツリー構築
	///

	BuildBehaviorTree();

	btEditor_ = std::make_unique<BehaviorTreeEditor<NormalEnemy>>();
	btEditor_->SetBehaviorTree(behaviorTree_.get());
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
	// リロード表示
	spriteReload_->Update();
}

void NormalEnemy::Draw() {
	// オブジェクト描画
	objectEnemy_->Draw();
}

void NormalEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void NormalEnemy::DrawUI() {
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

	// 上にずらすオフセット
	const float kOffsetReload = 60.0f;
	// リロード進捗率
	float reloadProgress = 1.0f - (reloadTimer_ / kReloadTime);
	// リロード時間に応じてサイズ変更
	spriteReload_->SetSize({kReloadSize.x * reloadProgress, kReloadSize.y});
	// スクリーン座標をセット
	spriteReload_->SetPosition(
	    {screenPosition.x - kReloadSize.x / 2.0f, // リロード表示が中心になるよう設定
	     screenPosition.y - kOffsetReload});

	// リロード時のみ描画
	if (isReloading_ && reloadTimer_ < kReloadTime) {
		spriteReload_->Draw();
	}
}

void NormalEnemy::OnCollision(Collider* other) {
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		// デバッグでプレイヤー発見状態にする
		if (!isPlayerDetected_) {
			isPlayerDetected_ = true;
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
			ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate, 3, { 0.0f, 0.0f, 0.0f }, DegToRad(45));
			ParticleEffectManager::GetInstance()->Emit("deathCross", objectEnemy_->transform_.translate, 3, { 0.0f, 0.0f, 0.0f }, DegToRad(135));

			ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録
		}
	}

	///
	/// vs Obstacle
	///
	if (other->GetTag() == "Obstacle") {
		AABBCollider* myAABB = dynamic_cast<AABBCollider*>(collider_.get());
		AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other);

		// 押し戻し処理
		if (myAABB && otherAABB) {
			// 押し戻しベクトル取得
			Float3 pushVec = myAABB->GetPushBackVector(*otherAABB);
			// 位置を補正
			objectEnemy_->transform_.translate += pushVec;

			// コライダーも更新しておく
			Float3 currentMin = myAABB->GetMin();
			Float3 currentMax = myAABB->GetMax();
			myAABB->SetMin(currentMin + pushVec);
			myAABB->SetMax(currentMax + pushVec);
		}
	}
}

void NormalEnemy::Debug() {
	ImGui::Begin("BehaviorTree_NormalEnemy");

	btEditor_->Draw();
	if (ImGui::Button("SAVE")) {
		btEditor_->Save("normalEnemy.json");
	}
	if (ImGui::Button("LOAD")) {
		btEditor_->Load("normalEnemy.json");
	}

	ImGui::End();

	// 索敵中の視界を可視化
	DrawDebugSight();

	// 調整パラメーター
	DrawConfigWindow("NormalEnemyConfig");

	ImGui::Begin("NormalEnemy");

	ImGui::End();
}

void NormalEnemy::MoveAlongPath(const std::vector<Waypoint*>& path, float speed) {
	// 経路に移動先がなければ終了
	if (path.size() < 2)
		return;

	// [0]は敵の位置なので[1]が次に向かう目標ウェイポイントになる
	Waypoint* nextWP = path[1];
	// 2個先の位置を補間した座標を目標にする
	Float3 targetPos = nextWP->GetPosition();
	if (path.size() > 2) {
		targetPos = (path[1]->GetPosition() + path[2]->GetPosition()) * 0.5f;
	}
	Float3 dir = Float3::Normalize(targetPos - objectEnemy_->transform_.translate);
	dir.y = 0.0f;
	dir = Float3::Normalize(dir);

	// 移動
	objectEnemy_->transform_.translate += dir * speed * TimeManager::GetInstance()->GetDeltaTime();

	// 向き補間
	Float3 lookDir = dir;

	float turnSpeed = 5.0f;
	float currentYaw = objectEnemy_->transform_.rotate.y;
	float targetYaw = std::atan2(lookDir.x, lookDir.z);

	// -π ~ πに正規化
	float deltaYaw = targetYaw - currentYaw;
	while (deltaYaw > PIf)
		deltaYaw -= 2.0f * PIf;
	while (deltaYaw < -PIf)
		deltaYaw += 2.0f * PIf;

	float newYaw = currentYaw + deltaYaw * turnSpeed * TimeManager::GetInstance()->GetDeltaTime();

	objectEnemy_->transform_.rotate.y = newYaw;
}

bool NormalEnemy::IsPlayerInSight() {
	if (!targetPlayer_)
		return false;

	if(targetPlayer_->IsDead())
		return false;

	Float3 enemyPos = this->objectEnemy_->transform_.translate;
	Float3 playerPos = targetPlayer_->GetTranslate();
	Float3 toPlayer = playerPos - enemyPos;

	///
	///	距離チェック
	///

	float distance = Float3::Length(toPlayer);
	// Playerが範囲外ならfalse
	if (distance > searchRadius_) {
		return false;
	}

	///
	///	FOVチェック
	///

	toPlayer = Float3::Normalize(toPlayer);

	// 前方向ベクトル（Y軸回転のみで考慮）
	Float3 forward = {std::sinf(objectEnemy_->transform_.rotate.y), 0.0f, std::cosf(objectEnemy_->transform_.rotate.y)};

	forward = Float3::Normalize(forward);

	// 内積から角度を求める
	float dot = Float3::Dot(forward, toPlayer);

	// ラジアンに変換
	float angleRad = std::acosf(dot);
	float angleDeg = angleRad * 180.0f / PIf;

	// 扇形角度チェック
	if (angleDeg > (searchFovDeg_ * 0.5f)) {
		return false;
	}

	///
	///	RayCastによる障害物チェック
	///

	RayCastHit hit{};
	bool rayCast = CollisionManager::GetInstance()->RayCast(enemyPos, toPlayer, distance, &hit);

	if (rayCast && hit.hitCollider->GetTag() == "Obstacle") {
		return false;
	}

	// プレイヤー発見状態にする
	isPlayerDetected_ = true;
	return true;
}

void NormalEnemy::DrawDebugSight() {
	// 分割数
	const uint32_t segments = 16;

	// 視界にプレイヤーがいれば赤色に
	Float4 color;
	if (IsPlayerInSight()) {
		color = {1.0f, 0.0f, 0.0f, 1.0f};
	} else {
		color = {1.0f, 1.0f, 1.0f, 1.0f};
	}

	Float3 center = objectEnemy_->transform_.translate;

	// 前方向ベクトル（Y軸回転のみで考慮）
	Float3 forward = {std::sinf(objectEnemy_->transform_.rotate.y), 0.0f, std::cosf(objectEnemy_->transform_.rotate.y)};
	forward = Float3::Normalize(forward);

	// 左端の方向ベクトル
	float halfFovRad = (searchFovDeg_ * 0.5f) * PIf / 180.0f;
	float baseAngle = std::atan2f(forward.z, forward.x);

	float startAngle = baseAngle - halfFovRad;
	float endAngle = baseAngle + halfFovRad;

	// 扇形を分割して線を描画
	Float3 firstPoint = {center.x + std::cosf(startAngle) * searchRadius_, center.y, center.z + std::sinf(startAngle) * searchRadius_};

	Float3 prevPoint = firstPoint;

	for (uint32_t i = 1; i <= segments; i++) {
		float t = static_cast<float>(i) / segments;
		float angle = startAngle + (endAngle - startAngle) * t;

		Float3 nextPoint = {center.x + std::cosf(angle) * searchRadius_, center.y, center.z + std::sinf(angle) * searchRadius_};

		// 円弧の線分
		LineDrawer::GetInstance()->RegisterLine(prevPoint, nextPoint, color);

		prevPoint = nextPoint;
	}

	// 扇の骨組み（中心から弧の両端）
	Float3 lastPoint = prevPoint;
	LineDrawer::GetInstance()->RegisterLine(center, firstPoint, color);
	LineDrawer::GetInstance()->RegisterLine(center, lastPoint, color);
}

BehaviorStatus NormalEnemy::RandomPatrol() {
	BehaviorStatus status = BehaviorStatus::Running;

	///
	///	移動先のウェイポイントを取得
	///

	if (!currentTargetWP_) {
		// 移動先ウェイポイント候補
		std::vector<Waypoint*> candidates;
		for (auto& wp : WaypointManager::GetInstance()->GetWaypoints()) {
			float distFromSpawn = Float3::Length(wp->GetPosition() - spawnPosition_);                       // スポーン地点からウェイポイントまでの距離
			float distFromCurrent = Float3::Length(wp->GetPosition() - objectEnemy_->transform_.translate); // 現在地点からウェイポイントまでの距離

			// スポーン地点から一定範囲内にあるかつ、現在位置から一定距離離れたウェイポイントのみを収集
			if (distFromSpawn <= patrolRange_ && distFromCurrent >= minPatrolRange_) {
				candidates.push_back(wp.get());
			}
		}

		// 候補からランダムに1つ選択
		uint32_t randIndex = RandomGenerator::GetInstance()->RandomValue(0, candidates.size() - 1);
		currentTargetWP_ = candidates[randIndex];

		status = BehaviorStatus::Running;

		///
		/// ターゲットのウェイポイントまで移動
		///
	} else {
		// 経路探索
		Waypoint* startWP = WaypointManager::GetInstance()->FindClosestWaypoint(objectEnemy_->transform_.translate);
		std::vector<Waypoint*> path = WaypointManager::GetInstance()->FindPath(startWP, currentTargetWP_);
		if (!path.empty()) {
			// 移動
			MoveAlongPath(path, patrolMoveSpeed_);

			// 目標に到達したらターゲットをクリア
			Float3 targetPos = currentTargetWP_->GetPosition();
			if (Float3::Length(targetPos - objectEnemy_->transform_.translate) < 3.0f) {
				currentTargetWP_ = nullptr;

				status = BehaviorStatus::Success; // 成功
			}
		}
	}

	return status;
}

BehaviorStatus NormalEnemy::RandomRotate() {
	if (rotateTimer_ <= 0.0f) {
		// 回転方向をランダムに決める
		rotateDirection_ = RandomGenerator::GetInstance()->RandomValueBool() ? 1.0f : -1.0f;

		// 回転時間をランダムに決める
		rotateTimer_ = RandomGenerator::GetInstance()->RandomValue(1.0f, 2.0f);
	}

	// 回転処理
	objectEnemy_->transform_.rotate.y += rotateDirection_ * TimeManager::GetInstance()->GetDeltaTime();

	// タイマー減少
	rotateTimer_ -= TimeManager::GetInstance()->GetDeltaTime();

	// 時間が残っていれば実行中
	if (rotateTimer_ > 0.0f) {
		return BehaviorStatus::Running;
	}

	// 終了したら成功
	rotateTimer_ = 0.0f;
	return BehaviorStatus::Success;
}

BehaviorStatus NormalEnemy::FacePlayer() {
	// プレイヤーへの方向ベクトルからY軸回転角度の計算
	Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸回転を適用
	objectEnemy_->transform_.rotate.y = targetAngle;

	return BehaviorStatus::Success;
}

BehaviorStatus NormalEnemy::Shoot() {
	float dt = TimeManager::GetInstance()->GetDeltaTime();

	// memo : リロード開始/終了時に移動先の経路探索を挟む

	// リロード中処理
	if (isReloading_) {
		reloadTimer_ -= dt;
		// リロード終了時に弾を込める
		if (reloadTimer_ <= 0.0f) {
			isReloading_ = false;
			currentAmmo_ = kMagazineSize;
		}
		return BehaviorStatus::Running;
	}

	// バースト間のインターバル（次のバースト撃ちまで待機）
	if (fireCooldown_ > 0.0f) {
		fireCooldown_ -= dt;
		return BehaviorStatus::Running;
	}

	// バースト内のインターバル（バースト射撃中）
	if (burstCooldown_ > 0.0f) {
		burstCooldown_ -= dt;
		return BehaviorStatus::Running;
	}

	// 弾切れならリロード開始
	if (currentAmmo_ <= 0) {
		isReloading_ = true;
		reloadTimer_ = kReloadTime; // リロード時間セット

		return BehaviorStatus::Running;
	}

	// 弾の発射処理
	Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	// 拡散角をランダムに設定
	float randSpread = RandomGenerator::GetInstance()->RandomValue(-bulletSpreadAngle_, bulletSpreadAngle_);
	direction.x += randSpread;
	direction.z += randSpread;
	direction = Float3::Normalize(direction);
	// 弾の生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(objectEnemy_->transform_.translate, direction, &ModelManager::GetInstance()->GetModel("Bullet"));
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	// カウント更新
	currentAmmo_--;
	burstCount_++;

	// バースト射撃中管理
	if (burstCount_ < kBurstSize) {
		// バースト内クールタイムをセット
		burstCooldown_ = kBurstInterval;

		// バースト射撃終了
	} else {
		burstCount_ = 0;
		fireCooldown_ = kFireInterval; // バースト間クールタイムをセット
	}

	return BehaviorStatus::Success;
}

BehaviorStatus NormalEnemy::MoveToPlayer() {
	if (targetPlayer_->IsDead())
		return BehaviorStatus::Failure;

	// 経路探索でプレイヤーに移動
	Waypoint* start = WaypointManager::GetInstance()->FindClosestWaypoint(objectEnemy_->transform_.translate);
	Waypoint* goal = WaypointManager::GetInstance()->FindClosestWaypoint(targetPlayer_->GetTranslate());

	if (!start || !goal)
		return BehaviorStatus::Failure;

	std::vector<Waypoint*> path = WaypointManager::GetInstance()->FindPath(start, goal);
	MoveAlongPath(path, speed_);

	return BehaviorStatus::Running;
}

void NormalEnemy::BuildBehaviorTree() {
	///
	///	索敵シーケンス
	///

	// 移動前待機
	auto waitBeforePatrol = std::make_unique<WaitNode<NormalEnemy>>(1.0f, 1.0f, "");

	// ランダム移動
	auto randomPatrol = std::make_unique<ActionNode<NormalEnemy>>([this](NormalEnemy* enemy, float dt) { return this->RandomPatrol(); }, "randomPatrol");

	// 回転前待機
	auto waitBeforeRotate = std::make_unique<WaitNode<NormalEnemy>>(1.0f, 1.0f, "");

	// ランダム回転
	auto randomRotate = std::make_unique<ActionNode<NormalEnemy>>([this](NormalEnemy* enemy, float dt) { return this->RandomRotate(); }, "randomRotate");

	// searchSequence構築
	auto searchSequence = std::make_unique<SequenceNode<NormalEnemy>>("searchSequence");
	searchSequence->AddChild(std::move(waitBeforePatrol));
	searchSequence->AddChild(std::move(randomPatrol));
	searchSequence->AddChild(std::move(waitBeforeRotate));
	searchSequence->AddChild(std::move(randomRotate));

	///
	///	攻撃シーケンス
	///

	// 視界チェック
	auto isPlayerInSight = std::make_unique<ConditionNode<NormalEnemy>>([this](NormalEnemy* enemy) { return this->IsPlayerInSight(); }, "isPlayerInSight");

	// プレイヤー方向を向く
	auto facePlayer = std::make_unique<ActionNode<NormalEnemy>>([this](NormalEnemy* enemy, float dt) { return this->FacePlayer(); }, "facePlayer");

	// 射撃を行う
	auto shoot = std::make_unique<ActionNode<NormalEnemy>>([this](NormalEnemy* enemy, float dt) { return this->Shoot(); }, "shoot");

	// attackParallel構築
	auto attackParallel = std::make_unique<ParallelNode<NormalEnemy>>("attackParallel");
	attackParallel->AddChild(std::move(facePlayer));
	attackParallel->AddChild(std::move(shoot));

	// attackSequence構築
	auto attackSequence = std::make_unique<SequenceNode<NormalEnemy>>("attackSequence");
	attackSequence->AddChild(std::move(isPlayerInSight));
	attackSequence->AddChild(std::move(attackParallel));

	///
	/// 移動シーケンス
	///

	// 視界チェック
	auto isDetected = std::make_unique<ConditionNode<NormalEnemy>>([this](NormalEnemy* enemy) { return this->isPlayerDetected_; }, "isPlayerDetected");

	// 発見済みなら移動
	auto moveToPlayer = std::make_unique<ActionNode<NormalEnemy>>([this](NormalEnemy* enemy, float dt) { return this->MoveToPlayer(); }, "moveToPlayer");

	// moveSequence構築
	auto moveSequence = std::make_unique<SequenceNode<NormalEnemy>>("moveSequence");
	moveSequence->AddChild(std::move(isDetected));
	moveSequence->AddChild(std::move(moveToPlayer));

	///
	///	攻撃セレクタ
	///

	// attackSelector構築
	auto attackSelector = std::make_unique<SelectorNode<NormalEnemy>>("attackSelector");
	attackSelector->AddChild(std::move(attackSequence));
	attackSelector->AddChild(std::move(moveSequence));

	///
	///	ルートノード構築
	///

	auto root = std::make_unique<SelectorNode<NormalEnemy>>("root");
	root->AddChild(std::move(attackSelector));
	root->AddChild(std::move(searchSequence));

	///
	///	BehaviorTree構築
	///
	behaviorTree_ = std::make_unique<BehaviorTree<NormalEnemy>>(std::move(root));
}
