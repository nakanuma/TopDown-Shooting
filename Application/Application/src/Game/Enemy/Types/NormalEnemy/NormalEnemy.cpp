#include "NormalEnemy.h"

// C++
#include <Windows.h>
#include <algorithm>
#include <cstdio>
#include <numbers>

// Engine
#include <TimeManager.h>
#include <LineDrawer.h>

// Application
#include <src/Game/Player/Player.h>

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
	// プレイヤー検出処理
	CheckDetect();

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

#pragma region 索敵時ノード（シーケンス）構築

	auto searchNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();

	// プレイヤーの検出判定を行うノード
	auto checkDetectNode = std::make_unique<Cygnus::ConditionNode<NormalEnemy>>([](NormalEnemy* e) -> bool { return e->IsDetectedPlayer(); });
	searchNode->AddChild(std::move(checkDetectNode));

#pragma endregion

#pragma region 攻撃時ノード（シーケンス）構築

	auto attackNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();

	// プレイヤー方向を向くノード
	auto faceToPlayerNode = std::make_unique<Cygnus::ActionNode<NormalEnemy>>([](NormalEnemy* e, float dt) { return e->FaceToPlayer(); }, "FaceToPlayer");
	attackNode->AddChild(std::move(faceToPlayerNode));

#pragma endregion

#pragma region ルートノード（シーケンス）構築

	auto rootNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	rootNode->AddChild(std::move(searchNode)); // 索敵時ノード追加
	rootNode->AddChild(std::move(attackNode)); // 攻撃時ノード追加

	// ツリーの作成
	behaviorTree_ = std::make_unique<Cygnus::BehaviorTree<NormalEnemy>>(std::move(rootNode));

#pragma endregion
}

void NormalEnemy::CheckDetect() {
	// 既にプレイヤーを発見しているならスキップ
	if (IsDetectedPlayer() || !targetPlayer_) return;

	// プレイヤーとの距離（2乗）を計算
	float distSq = Cygnus::Float3::LengthSq(targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_);

#pragma region 射撃音（半径）チェック
	// プレイヤーが半径内で射撃していれば発見状態にする
	if (targetPlayer_->IsShootedThisFrame()) {
		if (distSq <= (kShootDetectionRadius * kShootDetectionRadius)) {
			OnDetected();
			return;
		}
	}

#pragma region 至近距離（半径）チェック
	// プレイヤーが半径内にいれば発見状態にする
	if (distSq <= (kProximityRadius * kProximityRadius)) {
		OnDetected();
		return;
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
				return;
			}

			// ここまできたら発見状態にする
			OnDetected();
			return;
		}
	}
#pragma endregion
}

Cygnus::BehaviorStatus NormalEnemy::FaceToPlayer() {
	// プレイヤーへの方向ベクトルからY軸回転角度を計算
	Cygnus::Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	float targetAngle = std::atan2f(toPlayer.x, toPlayer.z);
	// Y軸回転を適用
	objectEnemy_->transform_.rotate_.y = targetAngle;

	return Cygnus::BehaviorStatus::Success;
}