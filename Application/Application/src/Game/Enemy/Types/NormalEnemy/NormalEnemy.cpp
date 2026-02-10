#include "NormalEnemy.h"

// C++
#include <Windows.h>
#include <algorithm>
#include <cstdio>
#include <numbers>

// Engine
#include <TimeManager.h>

// Application
#include <src/Game/Player/Player.h>

// Externals
#include <ImguiWrapper.h>

void NormalEnemy::Initialize(const Cygnus::Float3& position, Player* player) {
	// オブジェクト生成
	objectEnemy_ = std::make_unique<Cygnus::Object3D>();
	objectEnemy_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("NormalEnemy");
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = { 0.0f, std::numbers::pi_v<float>, 0.0f }; // 手前を向いた状態でスポーン（一時的に）

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
	ui_->Update(state);

	// 発光演出更新
	visualEffect_->Update();

	// ビヘイビアツリー更新
	behaviorTree_->Tick(this, Cygnus::TimeManager::GetInstance()->GetDeltaTime());
}

void NormalEnemy::Draw() { objectEnemy_->Draw(); }

void NormalEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void NormalEnemy::DrawUI() { ui_->Draw(); }

void NormalEnemy::OnCollision(Cygnus::Collider* other) {
	// 敵共通の衝突時処理
	Enemy::OnCollision(other);

	// このクラス特有の衝突時処理
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

	///
	///	攻撃時ノード（シーケンス）
	/// 

	auto attackNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();

	// プレイヤー方向を向くノード
	auto faceToPlayer = std::make_unique<Cygnus::ActionNode<NormalEnemy>>(
		[](NormalEnemy* enemy, float dt){ return enemy->FaceToPlayer(); }, "FaceToPlayer");
	attackNode->AddChild(std::move(faceToPlayer));

	///
	///	ルートノード（シーケンス）
	/// 

	auto rootNode = std::make_unique<Cygnus::SequenceNode<NormalEnemy>>();
	rootNode->AddChild(std::move(attackNode)); // 攻撃時ノード追加

	///
	///	ツリー構築
	/// 
	
	behaviorTree_ = std::make_unique<Cygnus::BehaviorTree<NormalEnemy>>(std::move(rootNode));
}

Cygnus::BehaviorStatus NormalEnemy::FaceToPlayer() {
	// プレイヤーへの方向ベクトルからY軸回転角度を計算
	Cygnus::Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	float targetAngle = std::atan2f(toPlayer.x, toPlayer.z);
	// Y軸回転を適用
	objectEnemy_->transform_.rotate_.y = targetAngle;

	return Cygnus::BehaviorStatus::Success;
}