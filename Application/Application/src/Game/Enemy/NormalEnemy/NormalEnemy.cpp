#include "NormalEnemy.h"

// C++
#include <algorithm>
#include <Windows.h>
#include <cstdio>
#include <numbers>

// Engine
#include <Camera.h>
#include <Collider/CollisionManager.h>
#include <DirectXBase.h>
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/3D/LineDrawer.h>

// Application
#include <src/Game/Bullet/Base/Bullet.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/System/ResultStats.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
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
	objectEnemy_->transform_.scale = { 1.0f, 1.0f, 1.0f };
	objectEnemy_->transform_.rotate = { 0.0f, std::numbers::pi_v<float>, 0.0f }; // 手前を向いた状態でスポーン（一時的に）

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);
	colliderSize_ = { 1.0f, 2.0f, 1.0f };

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());
	UpdateCollider(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	///
	///	スプライト生成
	///

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 黒

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({ 0.0f, 1.0f, 0.5f, 1.0f }); // 緑

	///
	///	パラメーター設定
	///

	isDead_ = false;

	// HPの設定
	currentHP_ = 100;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定（全Enemyクラス共通）

	targetPlayer_ = player;

	///
	///	調整パラメーター登録
	///

	RegisterParam("speed", &speed_, 0.0f, 10.0f, 0.01f);
	RegisterParam("searchRadius", &searchRadius_, 0.0f, 100.0f, 0.01f);
	RegisterParam("searchFovDeg", &searchFovDeg_, 0.0f, 180.0f, 1.00f);

	SetConfigPath("Player/playerConfig.json"); // ファイルパス設定
	InitConfig(); // 初回読み込み

	///
	///	ビヘイビアツリー構築
	///

	BuildBehaviorTree();

	btEditor_ = std::make_unique<BehaviorTreeEditor<NormalEnemy>>();
	btEditor_->SetBehaviorTree(behaviorTree_.get());
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void NormalEnemy::Update() {
	///
	/// コライダー更新処理
	///

	UpdateCollider();

	///
	/// オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();

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
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void NormalEnemy::Draw() {
	// オブジェクト描画
	objectEnemy_->Draw();
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
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
		screenPosition.y - kOffsetHPBar               // オフセット分上にずらす
		});
	spriteHPBackground_->Draw();

	///
	///	HPバー（前景）描画
	///

	// 現在HPに応じてサイズ変更
	Float2 hpBarForegroundSize = { kHPBarSize.x * hpRatio, kHPBarSize.y };
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	// スクリーン座標をセット
	spriteHPForeground_->SetPosition({
		screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定
		screenPosition.y - kOffsetHPBar               // オフセット分上にずらす
		});
	spriteHPForeground_->Draw();
}


// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void NormalEnemy::OnCollision(Collider* other) {
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		// PlayerBulletのdamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
		ResultStats::GetInstance()->AddHit(); // 弾が命中したことを記録
		ResultStats::GetInstance()->AddDamage(damage); // 与えたダメージを記録

		// HPが0になった敵を死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;
			ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録
		}
	}

	///
	/// vs NormalObstacle
	///
	if (other->GetTag() == "NormalObstacle") {
		AABBCollider* myAABB = dynamic_cast<AABBCollider*>(collider_.get());
		AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other);

		// 押し戻し処理
		if (myAABB && otherAABB) {
			// 押し戻しベクトル取得
			Float3 pushVec = myAABB->GetPushBackVector(*otherAABB);
			// 位置を補正
			objectEnemy_->transform_.translate += pushVec;

			// コライダーも更新しておく
			myAABB->min_ += pushVec;
			myAABB->max_ += pushVec;
		}
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void NormalEnemy::Debug()
{
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
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateCollider() {
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = objectEnemy_->transform_.translate;
		Float3 size = colliderSize_;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}

// ---------------------------------------------------------
// 経路探索で得たウェイポイント列に沿って移動
// ---------------------------------------------------------
void NormalEnemy::MoveAlongPath(const std::vector<Waypoint*>& path)
{
	// 経路に移動先がなければ終了
	if (path.size() < 2) return;

	// [0]は敵の位置なので[1]が次に向かう目標ウェイポイントになる
	Waypoint* nextWP = path[1];
	// 目標ウェイポイントへのベクトル計算
	Float3 dir = Float3::Normalize(nextWP->GetPosition() - objectEnemy_->transform_.translate);
	dir.y = 0.0f;
	dir = Float3::Normalize(dir);

	// 移動
	objectEnemy_->transform_.translate += dir * speed_ * TimeManager::GetInstance()->GetDeltaTime();


	// 向き補間
	Float3 lookDir = dir;

	float turnSpeed = 5.0f;
	float currentYaw = objectEnemy_->transform_.rotate.y;
	float targetYaw = std::atan2(lookDir.x, lookDir.z);
	float newYaw = currentYaw + (targetYaw - currentYaw) * turnSpeed * TimeManager::GetInstance()->GetDeltaTime();

	objectEnemy_->transform_.rotate.y = newYaw;
}

// ---------------------------------------------------------
// プレイヤーの視界チェック
// ---------------------------------------------------------
bool NormalEnemy::IsPlayerInSight()
{ 
	if (!targetPlayer_) return false;

	Float3 enemyPos = this->objectEnemy_->transform_.translate;
	Float3 playerPos = targetPlayer_->GetTranslate();
	Float3 toPlayer = playerPos - enemyPos;

	// 距離チェック
	float distance = Float3::Length(toPlayer);
	// Playerが範囲外ならfalse
	if (distance > searchRadius_) {
		return false;
	}

	toPlayer = Float3::Normalize(toPlayer);

	// 前方向ベクトル（Y軸回転のみで考慮）
	Float3 forward = {
		std::sinf(objectEnemy_->transform_.rotate.y),
		0.0f,
		std::cosf(objectEnemy_->transform_.rotate.y)
	};

	forward = Float3::Normalize(forward);

	// 内積から角度を求める
	float dot = Float3::Dot(forward, toPlayer);

	// ラジアンに変換
	float angleRad = std::acosf(dot);
	float angleDeg = angleRad * 180.0f / PIf;

	// 扇形角度チェック
	return angleDeg <= (searchFovDeg_ * 0.5f);
}

void NormalEnemy::DrawDebugSight()
{
	// 分割数
	const uint32_t segments = 16;

	// 視界にプレイヤーがいれば赤色に
	Float4 color;
	if (IsPlayerInSight()) {
		color = { 1.0f, 0.0f, 0.0f, 1.0f };
	} else {
		color = { 1.0f, 1.0f, 1.0f, 1.0f };
	}
	
	Float3 center = objectEnemy_->transform_.translate;

	// 前方向ベクトル（Y軸回転のみで考慮）
	Float3 forward = {
		std::sinf(objectEnemy_->transform_.rotate.y),
		0.0f,
		std::cosf(objectEnemy_->transform_.rotate.y)
	};
	forward = Float3::Normalize(forward);

	// 左端の方向ベクトル
	float halfFovRad = (searchFovDeg_ * 0.5f) * PIf / 180.0f;
	float baseAngle = std::atan2f(forward.z, forward.x);

	float startAngle = baseAngle - halfFovRad;
	float endAngle = baseAngle + halfFovRad;

	// 扇形を分割して線を描画
	Float3 firstPoint = {
		center.x + std::cosf(startAngle) * searchRadius_,
		center.y,
		center.z + std::sinf(startAngle) * searchRadius_
	};

	Float3 prevPoint = firstPoint;

	for (uint32_t i = 1; i <= segments; i++) {
		float t = static_cast<float>(i) / segments;
		float angle = startAngle + (endAngle - startAngle) * t;

		Float3 nextPoint = {
			center.x + std::cosf(angle) * searchRadius_,
			center.y,
			center.z + std::sinf(angle) * searchRadius_
		};

		// 円弧の線分
		LineDrawer::GetInstance()->RegisterLine(prevPoint, nextPoint, color);

		prevPoint = nextPoint;
	}

	// 扇の骨組み（中心から弧の両端）
	Float3 lastPoint = prevPoint;
	LineDrawer::GetInstance()->RegisterLine(center, firstPoint, color);
	LineDrawer::GetInstance()->RegisterLine(center, lastPoint, color);
}

// ---------------------------------------------------------
// ビヘイビアツリーの構築
// ---------------------------------------------------------
void NormalEnemy::BuildBehaviorTree()
{


	///
	///	ルートノード
	/// 
	
	auto root = std::make_unique<SequenceNode<NormalEnemy>>("root");
	behaviorTree_ = std::make_unique<BehaviorTree<NormalEnemy>>(std::move(root));
}
