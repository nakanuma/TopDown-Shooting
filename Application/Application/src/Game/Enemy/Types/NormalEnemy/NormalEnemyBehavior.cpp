#include "NormalEnemyBehavior.h"

// Application
#include <src/Game/Enemy/Types/NormalEnemy/NormalEnemy.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Waypoint/WaypointManager.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>

std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> NormalEnemyBehavior::CreateTree(NormalEnemy* e) {
#pragma region LeafNodeの生成
	// プレイヤーの検出判定を行う条件ノード
	auto checkDetectNode = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>([](NormalEnemy* e) -> bool { return CheckDetect(e); });
	
	// プレイヤーの方を向く行動ノード
	auto faceToPlayerNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return FaceToPlayer(e, dt); }, "FaceToPlayer");
	
	// 射撃を行う行動ノード
	auto shootNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionShoot(e, dt); }, "ActionShoot");
	
	// リロードを行う行動ノード
	auto reloadNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionReload(e, dt); }, "ActionReload");
	
	// 移動判定・準備を行う行動ノード
	auto decideMoveNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionDecideMove(e, dt); }, "ActionDecideMove");
	
	// 戦闘中微移動を行う行動ノード
	auto moveNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionMove(e, dt); }, "ActionMove");
	
	// 距離チェックを行う条件ノード
	auto isNearPlayerNode = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>([](NormalEnemy* e){
		float dist = Cygnus::Float3::Length(e->targetPlayer_->GetTranslate() - e->GetTranslate());
		return dist <= e->kAttackRange;
		});
	
	// 接近（経路探索）を行う行動ノード
	auto approachNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionApproachPlayer(e, dt); }, "ActionApproachPlayer");

	// 索敵動作を行う行動ノード
	auto searchLookNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionSearchLookAround(e, dt); });
#pragma endregion

#pragma region 戦闘・射撃フェーズ（近距離用）

	// 射撃/リロードの選択ノード
	auto shootOrReloadNode = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>();
	shootOrReloadNode->AddChild(std::move(shootNode));
	shootOrReloadNode->AddChild(std::move(reloadNode));

	// 移動判定をしてから射撃/リロードを行う連続ノード
	auto moveAndAttackSequence = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	moveAndAttackSequence->AddChild(std::move(decideMoveNode));
	moveAndAttackSequence->AddChild(std::move(shootOrReloadNode));

	// 向き合わせ + 移動 + 攻撃判定の並列ノード
	auto combatParallelNode = std::make_unique<Cygnus::ParallelNode<NormalEnemy>>();
	combatParallelNode->AddChild(std::move(faceToPlayerNode));
	combatParallelNode->AddChild(std::move(moveNode));
	combatParallelNode->AddChild(std::move(moveAndAttackSequence));

	// プレイヤーと距離が近いなら戦闘挙動を行う連続ノード
	auto nearAttackSequence = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	nearAttackSequence->AddChild(std::move(isNearPlayerNode));
	nearAttackSequence->AddChild(std::move(combatParallelNode));

#pragma endregion

#pragma region 接近フェーズ（遠距離用）

	// プレイヤー方向を向く行動ノード
	auto faceToPlayerNodeForApproach = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return FaceToPlayer(e, dt); }, "FaceToPlayer");

	// 向き合わせ + 移動の並列ノード
	auto approachParallel = std::make_unique<Cygnus::ParallelNode<NormalEnemy>>();
	approachParallel->AddChild(std::move(faceToPlayerNodeForApproach));
	approachParallel->AddChild(std::move(approachNode));

#pragma endregion

#pragma region 発見時セレクター（射撃 or 接近）

	// プレイヤーを発見している場合のメイン分岐
	auto combatSelector = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>();
	combatSelector->AddChild(std::move(nearAttackSequence));	// 近ければ攻撃
	combatSelector->AddChild(std::move(approachParallel));		// 遠ければ接近

	// 発見状態チェックを先頭に置く
	auto attackNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	attackNode->AddChild(std::move(checkDetectNode));
	attackNode->AddChild(std::move(combatSelector));

#pragma endregion

#pragma region 索敵・ルート構築

	// 索敵フェーズ
	auto searchNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	searchNode->AddChild(std::move(searchLookNode));

	// ルート選択ノード
	auto rootNode = std::make_unique<Cygnus::SelectorNode<NormalEnemy>>();
	rootNode->AddChild(std::move(attackNode)); // 発見中
	rootNode->AddChild(std::move(searchNode)); // 未発見

	return std::make_unique<Cygnus::BehaviorTree<NormalEnemy>>(std::move(rootNode));

#pragma endregion
}

bool NormalEnemyBehavior::CheckDetect(NormalEnemy* e) {
	// プレイヤー発見済みなら即座にtrue
	if (e->IsDetectedPlayer())
		return true;

	// プレイヤーとの距離（2乗）を計算
	float distSq = Cygnus::Float3::LengthSq(e->targetPlayer_->GetTranslate() - e->objectEnemyAnim_->GetTranslate());

#pragma region 射撃音（半径）チェック
	// プレイヤーが半径内で射撃していれば発見状態にする
	if (e->targetPlayer_->IsShootedThisFrame()) {
		if (distSq <= (e->kShootDetectionRadius * e->kShootDetectionRadius)) {
			e->OnDetected();
			return true;
		}
	}

#pragma region 至近距離（半径）チェック
	// プレイヤーが半径内にいれば発見状態にする
	if (distSq <= (e->kProximityRadius * e->kProximityRadius)) {
		e->OnDetected();
		return true;
	}
#pragma endregion

#pragma region 視界チェック（扇形 + レイキャスト）
	// 索敵半径内かどうかの判定
	if (distSq <= e->kVisionRange * e->kVisionRange) {
		// 前方向ベクトルを計算
		Cygnus::Float3 forward = {std::sinf(e->objectEnemyAnim_->GetRotate().y), 0.0f, std::cosf(e->objectEnemyAnim_->GetRotate().y)};

		// 敵からプレイヤーへの方向ベクトルを計算
		Cygnus::Float3 toPlayer = e->targetPlayer_->GetTranslate() - e->objectEnemyAnim_->GetTranslate();
		toPlayer = Cygnus::Float3::Normalize(toPlayer);

		// 内積から角度を求める
		float dot = Cygnus::Float3::Dot(forward, toPlayer);

		// ラジアンに変換
		float halfFovRad = Cygnus::DegToRad(e->kSearchFovDeg * 0.5f);
		float cosThresold = std::cosf(halfFovRad);

		// 内積結果が閾値より大きければ視界内
		if (dot >= cosThresold) {
			// レイキャスト判定
			Cygnus::RayCastHit hit{};
			bool hasHit = Cygnus::CollisionManager::GetInstance()->RayCast(e->objectEnemyAnim_->GetTranslate(), toPlayer, std::sqrtf(distSq), &hit);

			// 障害物に遮られたらスキップ
			if (hasHit && hit.hitCollider->GetTag() == "Obstacle") {
				return false;
			}

			// ここまできたら発見状態にする
			e->OnDetected();
			return true;
		}
	}

	// 全ての判定をすり抜けたのでfalse（未発見）
	return false;

#pragma endregion
}

Cygnus::BehaviorStatus NormalEnemyBehavior::FaceToPlayer(NormalEnemy* e, float dt) {
	// プレイヤーへの方向ベクトルからY軸回転角度を計算
	Cygnus::Float3 toPlayer = e->targetPlayer_->GetTranslate() - e->objectEnemyAnim_->GetTranslate();
	float targetAngle = std::atan2f(toPlayer.x, toPlayer.z);

	// 現在の角度と目標角度の差分を求める
	float currentAngle = e->objectEnemyAnim_->GetRotate().y;
	float angleDiff = targetAngle - currentAngle;

	// 最短距離で回転するように角度差を補正
	while (angleDiff > Cygnus::PIf)
		angleDiff -= (Cygnus::PIf * 2.0f);
	while (angleDiff < -Cygnus::PIf)
		angleDiff += (Cygnus::PIf * 2.0f);

	// 回転速度を考慮して補間
	float maxRotation = e->kRotationSpeed * dt;
	if (std::abs(angleDiff) <= maxRotation) {
		e->objectEnemyAnim_->GetRotate().y = targetAngle;
	} else {
		// 角度差の符号に応じて回転
		e->objectEnemyAnim_->GetRotate().y += (angleDiff > 0 ? maxRotation : -maxRotation);
	}

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionShoot(NormalEnemy* e, float dt) {
	// リロード中なら失敗を返す
	if (e->isReloading_)
		return Cygnus::BehaviorStatus::Failure;
	// マガジンに弾が無ければ失敗を返す
	if (e->magazine_ <= 0)
		return Cygnus::BehaviorStatus::Failure;

	// 射撃間隔の待機
	e->shootTimer_ -= dt;
	if (e->shootTimer_ > 0.0f)
		return Cygnus::BehaviorStatus::Running;

	// 発射方向を決定
	Cygnus::Float3 direction = e->targetPlayer_->GetTranslate() - e->objectEnemyAnim_->GetTranslate();
	float randSpread = Cygnus::RandomGenerator::GetInstance()->RandomValue(-e->kBulletSpreadAngle, e->kBulletSpreadAngle);
	direction += {randSpread, 0.0f, randSpread}; // XとZ方向にランダムな拡散角を加算
	direction = Cygnus::Float3::Normalize(direction);

	// 弾を生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(e->objectGun_->transform_.translate_, direction, &Cygnus::ModelManager::GetInstance()->GetModel("Bullet"));
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	e->magazine_--;

	// 次までの射撃間隔時間を設定
	e->shootTimer_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(e->kShootMinInterval, e->kShootMaxInterval);
	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionReload(NormalEnemy* e, float dt) {
	// リロード開始
	if (!e->isReloading_) {
		e->isReloading_ = true;
		e->reloadTimer_ = e->kReloadTime;
	}

	// タイマーを更新してリロードが完了したら成功を返す
	e->reloadTimer_ -= dt;
	if (e->reloadTimer_ <= 0.0f) {
		e->magazine_ = e->kMaxMagazine;
		e->isReloading_ = false;
		return Cygnus::BehaviorStatus::Success;
	}

	return Cygnus::BehaviorStatus::Running;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionDecideMove(NormalEnemy* e, float dt) {
	// 既に移動中ならスキップ
	if (e->moveTimer_ > 0.0f) {
		return Cygnus::BehaviorStatus::Success;
	}

	// 移動するかどうかを指定した確率で決定
	if (Cygnus::RandomGenerator::GetInstance()->RandomValueBool(e->kMoveProbability)) {
		// 移動先ウェイポイントの選出
		std::vector<Waypoint*> candidates;
		Cygnus::Float3 myPos = e->objectEnemyAnim_->GetTranslate();
		Cygnus::Float3 playerPos = e->targetPlayer_->GetTranslate();
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
				if (distToPlayer > e->kKeepDistance) {
					// プレイヤーが理想距離よりも遠いなら前方にあるウェイポイントを候補に
					if (dot > 0.2f)
						candidates.push_back(wp.get());
				} else {
					// プレイヤーが理想距離よりも近いなら後方にあるウェイポイントを候補に
					if (dot < -0.2f)
						candidates.push_back(wp.get());
				}
			}
		}

		// 候補があればターゲットを設定
		if (!candidates.empty()) {
			uint32_t idx = Cygnus::RandomGenerator::GetInstance()->RandomValue(0, (uint32_t)candidates.size() - 1);
			e->combatTargetWP_ = candidates[idx];

			// 移動時間と速度をランダムに設定
			e->moveTimer_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(e->kMoveMinDuration, e->kMoveMaxDuration);
			e->moveSpeed_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(e->kMoveMinSpeed, e->kMoveMaxSpeed);
		
			// 歩いている状態であることを示す
			e->isWalking_ = true;
		}
	}

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionMove(NormalEnemy* e, float dt) {
	// 移動時間が残っていない or 移動先ウェイポイントが未設定ならスキップ
	if (e->moveTimer_ <= 0.0f || e->combatTargetWP_ == nullptr) {
		e->isWalking_ = false; // 移動が終了したこと示す
		e->combatTargetWP_ = nullptr;
		return Cygnus::BehaviorStatus::Success;
	}

	// 移動先ウェイポイントへの方向
	Cygnus::Float3 myPos = e->objectEnemyAnim_->GetTranslate();
	Cygnus::Float3 targetPos = e->combatTargetWP_->GetPosition();
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(targetPos - myPos);
	dir.y = 0.0f; // 上下方向には移動しない

	// 移動実行
	e->objectEnemyAnim_->GetTranslate() += dir * e->moveSpeed_ * dt;

	// タイマー更新
	e->moveTimer_ -= dt;
	return Cygnus::BehaviorStatus::Running;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionSearchLookAround(NormalEnemy* e, float dt)
{
	e->searchTimer_ -= dt;

	if(e->searchTimer_ <= 0.0f) {
		// 一定時間ごとに、現在の向きから左右ランダムな方向を向く
		float baseRot = e->objectEnemyAnim_->GetRotate().y;
		float offset = Cygnus::RandomGenerator::GetInstance()->RandomValue(-0.8f, 0.8f);
		e->searchTargetAngle_ = baseRot + offset;

		// 次の回転までのインターバルを設定
		e->searchTimer_ = Cygnus::RandomGenerator::GetInstance()->RandomValue(e->kSearchWaitMinTime, e->kSearchWaitMaxTime);
	}

	// 目標角度に向けてゆっくり回転させる
	float current = e->objectEnemyAnim_->GetRotate().y;
	float diff = e->searchTargetAngle_ - current;

	// 最短回転補正
	while(diff > Cygnus::PIf) diff -= (Cygnus::PIf * 2.0f);
	while(diff < -Cygnus::PIf) diff += (Cygnus::PIf * 2.0f);

	float step = 1.5f * dt;
	if(std::abs(diff) <= step) {
		e->objectEnemyAnim_->GetRotate().y = e->searchTargetAngle_;
	} else {
		e->objectEnemyAnim_->GetRotate().y += (diff > 0 ? step : -step);
	}

	// 索敵時は歩きアニメーションはオフ
	e->isWalking_ = false;

	return Cygnus::BehaviorStatus::Running;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionApproachPlayer(NormalEnemy* e, float dt)
{
	// 自身とプレイヤーに最も近いウェイポイントを取得
	Waypoint* start = WaypointManager::GetInstance()->FindClosestWaypoint(e->objectEnemyAnim_->GetTranslate());
	Waypoint* goal = WaypointManager::GetInstance()->FindClosestWaypoint(e->targetPlayer_->GetTranslate());

	if(!start || !goal) {
		return Cygnus::BehaviorStatus::Failure;
	}

	// 経路の計算
	std::vector<Waypoint*> path = WaypointManager::GetInstance()->FindPath(start, goal);
	if(path.size() < 2) { // 経路が短すぎる場合には終了
		return Cygnus::BehaviorStatus::Success;
	}

	// 移動処理
	Waypoint* nextWP = path[1]; // [0]は敵の位置なので[1]が次に向かう目標ウェイポイントになる
	Cygnus::Float3 targetPos = nextWP->GetPosition();

	if(path.size() > 2) { // 2個先のウェイポイントがある場合、角を滑らかにするために補間を行う
		targetPos = (path[1]->GetPosition() + path[2]->GetPosition()) * 0.5f;
	}

	Cygnus::Float3 diff = targetPos - e->objectEnemyAnim_->GetTranslate();
	diff.y = 0.0f; // 水平移動

	float distSq = Cygnus::Float3::LengthSq(diff);
	if(distSq > 0.001f) { // 極端に近い場合には移動スキップ
		Cygnus::Float3 dir = Cygnus::Float3::Normalize(diff);

		e->objectEnemyAnim_->GetTranslate() += dir * e->kApproachSpeed * dt;
		e->isWalking_ = true;
	}

	return Cygnus::BehaviorStatus::Running;
}
