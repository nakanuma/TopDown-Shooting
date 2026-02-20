#include "NormalEnemy.h"

// Engine
#include <LineDrawer.h>
#include <TimeManager.h>

// Application
#include <src/Game/Enemy/Types/NormalEnemy/NormalEnemyBehavior.h>

// Externals
#include <ImguiWrapper.h>

void NormalEnemy::Initialize(const Cygnus::Float3& position, Player* player) {
	// オブジェクト生成
	objectEnemy_ = std::make_unique<Cygnus::Object3D>();
	objectEnemy_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("NormalEnemy");
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = {0.0f, Cygnus::PIf, 0.0f}; // 手前を向いた状態でスポーン

	// アニメーションデータ読み込み
	walkData_.modelData = Cygnus::ModelManager::LoadModelFile("Character/Enemy/NormalEnemy/walk.gltf");
	walkData_.modelData.material.textureHandle = Cygnus::TextureManager::Load("Character/Enemy/NormalEnemy/normalEnemy.png");
	walkData_.animation = Cygnus::AnimationLoader::LoadAnimation("resources/Models", "Character/Enemy/NormalEnemy/walk.gltf");
	walkData_.skeleton.CreateSkeleton(walkData_.modelData.rootNode);

	// 敵オブジェクト生成
	objectEnemyAnim_ = std::make_unique<Cygnus::AnimatedModelInstance>();
	objectEnemyAnim_->Initialize(walkData_);
	objectEnemyAnim_->GetTranslate() = position;
	objectEnemyAnim_->GetRotate() = {0.0f, Cygnus::PIf, 0.0f}; // 手前を向いた状態でスポーン
	objectEnemyAnim_->SetPlayBackSpeed(kAnimationPlaybackSpeed);

	// 銃オブジェクト生成
	objectGun_ = std::make_unique<Cygnus::Object3D>();
	objectGun_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("Pistol");
	objectGun_->materialCB_.data_->color = kGunColor;

	// コライダー生成・登録
	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("NormalEnemy");
	aabb->SetFollowTarget(&objectEnemyAnim_->GetTranslate());
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
	behaviorTree_ = NormalEnemyBehavior::CreateTree(this);
}

void NormalEnemy::Update() {
	float dt = Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// 敵オブジェクト更新
	objectEnemyAnim_->Update(dt, isWalking_);
	objectEnemyAnim_->object_->UpdateShadowMatrix();

	// 銃オブジェクト更新
	Cygnus::Float3 myPos = objectEnemyAnim_->GetTranslate();
	Cygnus::Float3 myRot = objectEnemyAnim_->GetRotate();
	Cygnus::Float3 forward = {sinf(myRot.y), 0.0f, cosf(myRot.y)};
	Cygnus::Float3 up = {0.0f, 1.0f, 0.0f};

	objectGun_->transform_.translate_ = myPos + (forward * kGunForwardOffset) + (up * kGunUpOffset); // 位置を補正
	objectGun_->transform_.rotate_ = myRot;

	objectGun_->UpdateMatrix();
	objectGun_->UpdateShadowMatrix();

	// コライダー更新
	collider_->Update();

	// UI更新
	EnemyUIState state;
	state.worldPos = objectEnemyAnim_->GetTranslate();
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
	objectEnemyAnim_->Draw();
	objectGun_->Draw();

#ifdef _DEBUG
	DebugDrawLine();
#endif
}

void NormalEnemy::DrawShadow() { objectGun_->DrawShadow(); }

void NormalEnemy::DrawShadowSkinning() { objectEnemyAnim_->DrawShadow(); }

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

void NormalEnemy::OnDetected() {
	Enemy::OnDetected(); // 基底クラスの共通処理を呼び出す

	shootTimer_ = kFirstShootDelay; // 発見時のみ、最初の射撃まで遅延時間を設定する
}

void NormalEnemy::DebugDrawLine() {
#ifdef _DEBUG

	Cygnus::Float4 color = IsDetectedPlayer() ? Cygnus::Float4(1.0f, 0.0f, 0.0f, 1.0f) : Cygnus::Float4(1.0f, 1.0f, 1.0f, 1.0f);
	Cygnus::Float3 center = objectEnemyAnim_->GetTranslate();

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
	float currentRotY = objectEnemyAnim_->GetRotate().y;
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
