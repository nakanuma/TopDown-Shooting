#include "NormalEnemyBehavior.h"

// Application
#include <src/Game/Enemy/Types/NormalEnemy/NormalEnemy.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Waypoint/WaypointManager.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>

std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> NormalEnemyBehavior::CreateTree(NormalEnemy* e) {
#pragma region 攻撃時ノード（シーケンス）構築

	///
	/// LeafNode
	///

	// プレイヤーの検出判定を行うノード
	auto checkDetectNode = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>([](NormalEnemy* e) -> bool { return CheckDetect(e); });
	// プレイヤーの方を向くノード
	auto faceToPlayerNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return FaceToPlayer(e, dt); }, "FaceToPlayer");
	// 射撃を行うノード
	auto shootNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionShoot(e, dt); }, "ActionShoot");
	// リロードを行うノード
	auto reloadNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionReload(e, dt); }, "ActionReload");
	// 移動判定・準備を行うノード
	auto decideMoveNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionDecideMove(e, dt); }, "ActionDecideMove");
	// 移動を行うノード
	auto moveNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return ActionMove(e, dt); }, "ActionMove");

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

	// ツリーを作成して返す
	return std::make_unique<Cygnus::BehaviorTree<NormalEnemy>>(std::move(rootNode));

#pragma endregion
}

bool NormalEnemyBehavior::CheckDetect(NormalEnemy* e) {
	// プレイヤー発見済みなら即座にtrue
	if (e->IsDetectedPlayer())
		return true;

	// プレイヤーとの距離（2乗）を計算
	float distSq = Cygnus::Float3::LengthSq(e->targetPlayer_->GetTranslate() - e->objectEnemy_->transform_.translate_);

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
		Cygnus::Float3 forward = {std::sinf(e->objectEnemy_->transform_.rotate_.y), 0.0f, std::cosf(e->objectEnemy_->transform_.rotate_.y)};

		// 敵からプレイヤーへの方向ベクトルを計算
		Cygnus::Float3 toPlayer = e->targetPlayer_->GetTranslate() - e->objectEnemy_->transform_.translate_;
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
			bool hasHit = Cygnus::CollisionManager::GetInstance()->RayCast(e->objectEnemy_->transform_.translate_, toPlayer, std::sqrtf(distSq), &hit);

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
	Cygnus::Float3 toPlayer = e->targetPlayer_->GetTranslate() - e->objectEnemy_->transform_.translate_;
	float targetAngle = std::atan2f(toPlayer.x, toPlayer.z);

	// 現在の角度と目標角度の差分を求める
	float currentAngle = e->objectEnemy_->transform_.rotate_.y;
	float angleDiff = targetAngle - currentAngle;

	// 最短距離で回転するように角度差を補正
	while (angleDiff > Cygnus::PIf)
		angleDiff -= (Cygnus::PIf * 2.0f);
	while (angleDiff < -Cygnus::PIf)
		angleDiff += (Cygnus::PIf * 2.0f);

	// 回転速度を考慮して補間
	float maxRotation = e->kRotationSpeed * dt;
	if (std::abs(angleDiff) <= maxRotation) {
		e->objectEnemy_->transform_.rotate_.y = targetAngle;
	} else {
		// 角度差の符号に応じて回転
		e->objectEnemy_->transform_.rotate_.y += (angleDiff > 0 ? maxRotation : -maxRotation);
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
	Cygnus::Float3 direction = e->targetPlayer_->GetTranslate() - e->objectEnemy_->transform_.translate_;
	float randSpread = Cygnus::RandomGenerator::GetInstance()->RandomValue(-e->kBulletSpreadAngle, e->kBulletSpreadAngle);
	direction += {randSpread, 0.0f, randSpread}; // XとZ方向にランダムな拡散角を加算
	direction = Cygnus::Float3::Normalize(direction);

	// 弾を生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(e->objectEnemy_->transform_.translate_, direction, &Cygnus::ModelManager::GetInstance()->GetModel("Bullet"));
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
	if (e->moveTimer_ > 0.0f)
		return Cygnus::BehaviorStatus::Success;

	// 移動するかどうかを指定した確率で決定
	if (Cygnus::RandomGenerator::GetInstance()->RandomValueBool(e->kMoveProbability)) {
		// 移動先ウェイポイントの選出
		std::vector<Waypoint*> candidates;
		Cygnus::Float3 myPos = e->objectEnemy_->transform_.translate_;
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
		}
	}

	return Cygnus::BehaviorStatus::Success;
}

Cygnus::BehaviorStatus NormalEnemyBehavior::ActionMove(NormalEnemy* e, float dt) {
	// 移動時間が残っていない or 移動先ウェイポイントが未設定ならスキップ
	if (e->moveTimer_ <= 0.0f || e->combatTargetWP_ == nullptr) {
		e->combatTargetWP_ = nullptr;
		return Cygnus::BehaviorStatus::Success;
	}

	// 移動先ウェイポイントへの方向
	Cygnus::Float3 myPos = e->objectEnemy_->transform_.translate_;
	Cygnus::Float3 targetPos = e->combatTargetWP_->GetPosition();
	Cygnus::Float3 dir = Cygnus::Float3::Normalize(targetPos - myPos);
	dir.y = 0.0f; // 上下方向には移動しない

	// 移動実行
	e->objectEnemy_->transform_.translate_ += dir * e->moveSpeed_ * dt;

	// タイマー更新
	e->moveTimer_ -= dt;
	return Cygnus::BehaviorStatus::Running;
}