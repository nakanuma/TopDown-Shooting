#include "NormalEnemy.h"

// C++
#include <Windows.h>
#include <algorithm>
#include <cstdio>
#include <numbers>

// Engine
#include <LineDrawer.h>
#include <TimeManager.h>

// Application
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Waypoint/WaypointManager.h>

// Externals
#include <ImguiWrapper.h>

void NormalEnemy::Initialize(const Cygnus::Float3& position, Player* player) {
	// オブジェクト生成
	objectEnemy_ = std::make_unique<Cygnus::Object3D>();
	objectEnemy_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("NormalEnemy");
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = {0.0f, std::numbers::pi_v<float>, 0.0f}; // 手前を向いた状態でスポーン（一時的に）

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

	// ビヘイビアツリー構築
	BuildBehaviorTree();
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
	state.isReloading = isReloading_;
	state.reloadRatio = reloadTimer_ / kReloadTime;
	ui_->Update(state);

	// 発光演出更新
	visualEffect_->Update();

	// ビヘイビアツリー更新
	behaviorTree_->Tick(this, Cygnus::TimeManager::GetInstance()->GetDeltaTime());
}

void NormalEnemy::Draw() {
	objectEnemy_->Draw();

#ifdef _DEBUG

	Cygnus::Float4 color = IsDetectedPlayer() ? Cygnus::Float4(1.0f, 0.0f, 0.0f, 1.0f) : Cygnus::Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Cygnus::Float3 center = objectEnemy_->transform_.translate_;

	const uint32_t kCircleSegments = 32;

#pragma region 射撃音半径（円形）描画
	for (uint32_t i = 0; i < kCircleSegments; i++) {
		float angle1 = (static_cast<float>(i) / kCircleSegments) * (Cygnus::PIf * 2.0f);
		float angle2 = (static_cast<float>(i + 1) / kCircleSegments) * (Cygnus::PIf * 2.0f);

		Cygnus::Float3 p1 = {center.x + std::cosf(angle1) * kShootDetectionRadius, center.y, center.z + std::sinf(angle1) * kShootDetectionRadius};
		Cygnus::Float3 p2 = {center.x + std::cosf(angle2) * kShootDetectionRadius, center.y, center.z + std::sinf(angle2) * kShootDetectionRadius};

		Cygnus::LineDrawer::GetInstance()->RegisterLine(p1, p2, color);
	}
#pragma endregion

#pragma region 近接半径（円形）描画
	for (uint32_t i = 0; i < kCircleSegments; i++) {
		float angle1 = (static_cast<float>(i) / kCircleSegments) * (Cygnus::PIf * 2.0f);
		float angle2 = (static_cast<float>(i + 1) / kCircleSegments) * (Cygnus::PIf * 2.0f);

		Cygnus::Float3 p1 = {center.x + std::cosf(angle1) * kProximityRadius, center.y, center.z + std::sinf(angle1) * kProximityRadius};
		Cygnus::Float3 p2 = {center.x + std::cosf(angle2) * kProximityRadius, center.y, center.z + std::sinf(angle2) * kProximityRadius};

		Cygnus::LineDrawer::GetInstance()->RegisterLine(p1, p2, color);
	}
#pragma endregion

#pragma region 視界（扇形）描画
	const uint32_t kSectorSegments = 32;
	// 敵の向いている方向を基準にする
	float currentRotY = objectEnemy_->transform_.rotate_.y;
	float halfFovRad = Cygnus::DegToRad(kSearchFovDeg * 0.5f);

	// 扇形の両端の角度を算出
	float baseAngle = std::atan2f(std::sinf(currentRotY), std::cosf(currentRotY));

	// 扇の開始地点と終了地点（左右対称に広げる）
	float startAngle = baseAngle - halfFovRad;
	float endAngle = baseAngle + halfFovRad;

	Cygnus::Float3 prevPoint;
	for (uint32_t i = 0; i <= kSectorSegments; i++) {
		float t = static_cast<float>(i) / kSectorSegments;
		float angle = startAngle + (endAngle - startAngle) * t;

		Cygnus::Float3 nextPoint = {center.x + std::sinf(angle) * kVisionRange, center.y, center.z + std::cosf(angle) * kVisionRange};

		if (i > 0) {
			// 弧を描画
			Cygnus::LineDrawer::GetInstance()->RegisterLine(prevPoint, nextPoint, color);
		} else {
			// 扇の左側の線
			Cygnus::LineDrawer::GetInstance()->RegisterLine(center, nextPoint, color);
		}
		if (i == kSectorSegments) {
			// 扇の右側の線
			Cygnus::LineDrawer::GetInstance()->RegisterLine(nextPoint, center, color);
		}
		prevPoint = nextPoint;
	}
#pragma endregion

	ImGui::Begin("NormalEnemy");
	ImGui::DragFloat("moveTimer", &moveTimer_);
	ImGui::DragFloat3("moveDir", &moveDir_.x);
	ImGui::End();

#endif
}

void NormalEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void NormalEnemy::DrawUI() { ui_->Draw(); }

void NormalEnemy::OnCollision(Cygnus::Collider* other) {
	// 敵共通の衝突時処理
	Enemy::OnCollision(other);

	if (other->GetTag() == "PlayerBullet") {
		OnDetected();
	}

	// vs Obstacle
	if (other->GetTag() == "Obstacle") {
		// 障害物との押し戻し処理
		ResolveObstacleCollision(other);
	}
}

void NormalEnemy::Debug() {
#ifdef USE_IMGUI

#endif
}

void NormalEnemy::BuildBehaviorTree() {
#pragma region 攻撃時ノード（シーケンス）構築

	///
	/// LeafNode
	/// 

	// プレイヤーの検出判定を行うノード
	auto checkDetectNode = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>([](NormalEnemy* e) -> bool { return e->CheckDetect(); });
	// プレイヤーの方を向くノード
	auto faceToPlayerNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return e->FaceToPlayer(dt); }, "FaceToPlayer");
	// 射撃を行うノード
	auto shootNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return e->ActionShoot(dt); }, "ActionShoot");
	// リロードを行うノード
	auto reloadNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return e->ActionReload(dt); }, "ActionReload");
	// 移動判定・準備を行うノード
	auto decideMoveNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return e->ActionDecideMove(); }, "ActionDecideMove");
	// 移動を行うノード
	auto moveNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return e->ActionMove(dt); }, "ActionMove");

	///
	///	CompositeNode
	/// 

	// 射撃 / リロードの選択ノード
	auto shootOrReloadNode = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>();
	shootOrReloadNode->AddChild(std::move(shootNode));
	shootOrReloadNode->AddChild(std::move(reloadNode));

	// 移動判定->射撃を行うノード
	auto moveAndAttackSequence = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	moveAndAttackSequence->AddChild(std::move(decideMoveNode));
	moveAndAttackSequence->AddChild(std::move(shootOrReloadNode));

	// 移動と攻撃の並列ノード
	auto combatParallelNode = std::make_unique<Cygnus::ParallelNode<NormalEnemy>>();
	combatParallelNode->AddChild(std::move(faceToPlayerNode));
	combatParallelNode->AddChild(std::move(moveNode));
	combatParallelNode->AddChild(std::move(moveAndAttackSequence));

	// 常にプレイヤー発見状態を確認し、発見していれば射撃行動を行うセレクタノード
	auto attackNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	attackNode->AddChild(std::move(checkDetectNode));
	attackNode->AddChild(std::move(combatParallelNode));

#pragma endregion

#pragma region 索敵時ノード（シーケンス）構築

	auto searchNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	// TODO : 巡回・待機など索敵時の行動追加

#pragma endregion

#pragma region ルートノード（セレクター）構築

	auto rootNode = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>();
	rootNode->AddChild(std::move(attackNode)); // 攻撃時ノード追加
	rootNode->AddChild(std::move(searchNode)); // 索敵時ノード追加

	// ツリーの作成
	behaviorTree_ = std::make_unique<Cygnus::BehaviorTree<NormalEnemy>>(std::move(rootNode));

#pragma endregion
}

bool NormalEnemy::CheckDetect() {
	// プレイヤー発見済みなら即座にtrue
	if (IsDetectedPlayer()) return true;

	// プレイヤーとの距離（2乗）を計算
	float distSq = Cygnus::Float3::LengthSq(targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_);

#pragma region 射撃音（半径）チェック
	// プレイヤーが半径内で射撃していれば発見状態にする
	if (targetPlayer_->IsShootedThisFrame()) {
		if (distSq <= (kShootDetectionRadius * kShootDetectionRadius)) {
			OnDetected();
			return true;
		}
	}

#pragma region 至近距離（半径）チェック
	// プレイヤーが半径内にいれば発見状態にする
	if (distSq <= (kProximityRadius * kProximityRadius)) {
		OnDetected();
		return true;
	}
#pragma endregion

#pragma region 視界チェック（扇形 + レイキャスト）
	// 索敵半径内かどうかの判定
	if (distSq <= kVisionRange * kVisionRange) {
		// 前方向ベクトルを計算
		Cygnus::Float3 forward = {std::sinf(objectEnemy_->transform_.rotate_.y), 0.0f, std::cosf(objectEnemy_->transform_.rotate_.y)};

		// 敵からプレイヤーへの方向ベクトルを計算
		Cygnus::Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
		toPlayer = Cygnus::Float3::Normalize(toPlayer);

		// 内積から角度を求める
		float dot = Cygnus::Float3::Dot(forward, toPlayer);

		// ラジアンに変換
		float halfFovRad = Cygnus::DegToRad(kSearchFovDeg * 0.5f);
		float cosThresold = std::cosf(halfFovRad);

		// 内積結果が閾値より大きければ視界内
		if (dot >= cosThresold) {
			// レイキャスト判定
			Cygnus::RayCastHit hit{};
			bool hasHit = Cygnus::CollisionManager::GetInstance()->RayCast(objectEnemy_->transform_.translate_, toPlayer, std::sqrtf(distSq), &hit);

			// 障害物に遮られたらスキップ
			if (hasHit && hit.hitCollider->GetTag() == "Obstacle") {
				return false;
			}

			// ここまできたら発見状態にする
			OnDetected();
			return true;
		}
	}

	// 全ての判定をすり抜けたのでfalse（未発見）
	return false;

#pragma endregion
}

void NormalEnemy::OnDetected() { 
	Enemy::OnDetected(); // 基底クラスの共通処理を呼び出す

	shootTimer_ = kFirstShootDelay; // 発見時のみ、最初の射撃まで遅延時間を設定する
}

Cygnus::BehaviorStatus NormalEnemy::FaceToPlayer(float dt) {
	// プレイヤーへの方向ベクトルからY軸回転角度を計算
	Cygnus::Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	float targetAngle = std::atan2f(toPlayer.x, toPlayer.z);

	// 現在の角度と目標角度の差分を求める
	float currentAngle = objectEnemy_->transform_.rotate_.y;
	float angleDiff = targetAngle - currentAngle;

	// 最短距離で回転するように角度差を補正
	while (angleDiff > Cygnus::PIf) angleDiff -= (Cygnus::PIf * 2.0f);
	while (angleDiff < -Cygnus::PIf) angleDiff += (Cygnus::PIf * 2.0f);

	// 回転速度を考慮して補間
	float maxRotation = kRotationSpeed * dt;
	if (std::abs(angleDiff) <= maxRotation) {
		objectEnemy_->transform_.rotate_.y = targetAngle;
	} else {
		// 角度差の符号に応じて回転
		objectEnemy_->transform_.rotate_.y += (angleDiff > 0 ? maxRotation : -maxRotation);
	}

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemy::ActionShoot(float dt) {
	// リロード中なら失敗を返す
	if (isReloading_)
		return Cygnus::BehaviorStatus::Failure;
	// マガジンに弾が無ければ失敗を返す
	if (magazine_ <= 0)
		return Cygnus::BehaviorStatus::Failure;

	// 射撃間隔の待機
	shootTimer_ -= dt;
	if (shootTimer_ > 0.0f)
		return Cygnus::BehaviorStatus::Running;

	// 発射方向を決定
	Cygnus::Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	float randSpread = Cygnus::RandomGenerator::GetInstance()->RandomValue(-kBulletSpreadAngle, kBulletSpreadAngle);
	direction += {randSpread, 0.0f, randSpread}; // XとZ方向にランダムな拡散角を加算
	direction = Cygnus::Float3::Normalize(direction);

	// 弾を生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(objectEnemy_->transform_.translate_, direction, &Cygnus::ModelManager::GetInstance()->GetModel("Bullet"));
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	magazine_--;

	// 次までの射撃間隔時間を設定
	shootTimer_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(kShootMinInterval, kShootMaxInterval);
	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemy::ActionReload(float dt) {
	// リロード開始
	if (!isReloading_) {
		isReloading_ = true;
		reloadTimer_ = kReloadTime;
	}

	// タイマーを更新してリロードが完了したら成功を返す
	reloadTimer_ -= dt;
	if (reloadTimer_ <= 0.0f) {
		magazine_ = kMaxMagazine;
		isReloading_ = false;
		return Cygnus::BehaviorStatus::Success;
	}

	return Cygnus::BehaviorStatus::Running;
}

Cygnus::BehaviorStatus NormalEnemy::ActionDecideMove() {
	// 既に移動中ならスキップ
	if (moveTimer_ > 0.0f) 
		return Cygnus::BehaviorStatus::Success;

	// 移動するかどうかを指定した確率で決定
	if (Cygnus::RandomGenerator::GetInstance()->RandomValueBool(kMoveProbability)) {
		// 移動先ウェイポイントの選出
		std::vector<Waypoint*> candidates;
		Cygnus::Float3 myPos = objectEnemy_->transform_.translate_;
		Cygnus::Float3 playerPos = targetPlayer_->GetTranslate();
		float distToPlayer = Cygnus::Float3::Length(playerPos - myPos);

		// プレイヤーへの方向ベクトル
		Cygnus::Float3 toPlayerVec = Cygnus::Float3::Normalize(playerPos - myPos);

		for (auto& wp : WaypointManager::GetInstance()->GetWaypoints()) {
			Cygnus::Float3 wpPos = wp->GetPosition();
			float distFromMe = Cygnus::Float3::Length(wpPos - myPos);

			// 自分の一定範囲内のウェイポイントを対象にする
			if (distFromMe > 2.0f && distFromMe < 20.0f) {
				Cygnus::Float3 toWPVec = Cygnus::Float3::Normalize(wpPos - myPos);
				// プレイヤー方向 or 逆方向かを判定
				float dot = Cygnus::Float3::Dot(toPlayerVec, toWPVec);

				// プレイヤーとの距離判定
				if (distToPlayer > kKeepDistance) {
					// プレイヤーが理想距離よりも遠いなら前方にあるウェイポイントを候補に
					if (dot > 0.2f) candidates.push_back(wp.get());
				} else {
					// プレイヤーが理想距離よりも近いなら後方にあるウェイポイントを候補に
					if (dot < -0.2f) candidates.push_back(wp.get());
				}
			}
		}

		// 候補があればターゲットを設定
		if (!candidates.empty()) {
			uint32_t idx = Cygnus::RandomGenerator::GetInstance()->RandomValue(0, (uint32_t)candidates.size() - 1);
			combatTargetWP_ = candidates[idx];

			// 移動時間と速度をランダムに設定
			moveTimer_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(kMoveMinDuration, kMoveMaxDuration);
			moveSpeed_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(kMoveMinSpeed, kMoveMaxSpeed);
		}
	}

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemy::ActionMove(float dt) { 
	// 移動時間が残っていない or 移動先ウェイポイントが未設定ならスキップ
	if (moveTimer_ <= 0.0f || combatTargetWP_ == nullptr) {
		combatTargetWP_ = nullptr;
		return Cygnus::BehaviorStatus::Success;
	}

	// 移動先ウェイポイントへの方向
	Cygnus::Float3 myPos = objectEnemy_->transform_.translate_;
	Cygnus::Float3 targetPos = combatTargetWP_->GetPosition();
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(targetPos - myPos);
	dir.y = 0.0f; // 上下方向には移動しない

	// 移動実行
	objectEnemy_->transform_.translate_ += dir * moveSpeed_ * dt;

	// タイマー更新
	moveTimer_ -= dt;
	return Cygnus::BehaviorStatus::Running;
}
