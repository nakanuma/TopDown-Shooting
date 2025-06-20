#include "NormalEnemy.h"

// C++
#include <algorithm>

// Engine
#include <Camera.h>
#include <Collider/CollisionManager.h>
#include <DirectXBase.h>
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>

// Application
#include <src/Game/Bullet/Base/Bullet.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void NormalEnemy::Initialize(const Float3& position, ModelManager::ModelData* model) {
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
	objectEnemy_->materialCB_.data_->color = {1.0f, 0.5f, 0.0f, 1.0f};

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	スプライト生成
	///

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({0.0f, 1.0f, 0.5f, 1.0f});

	///
	///	パラメーター設定
	///

	// HPの設定
	currentHP_ = 10;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定（全Enemyクラス共通）
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void NormalEnemy::Update(Player* player) {
	///
	/// コライダー更新処理
	///

	UpdateCollider();

	///
	///	ステート管理
	///

	UpdateState(player);

	///
	///	弾の更新処理
	///

	UpdateBullets();

	///
	/// オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();

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

	// 全ての弾を描画
	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void NormalEnemy::DrawUI() {
	// オブジェクトのワールド座標->スクリーン座標に変換
	Float3 screenPosition = Utility::WorldToScreen(objectEnemy_->transform_.translate);
	// 上にずらす分のオフセット
	float offset = 60.0f;

	// HP割合
	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	///
	/// HPバー（後景）描画
	///

	// スクリーン座標をセット
	spriteHPBackground_->SetPosition({
	    screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定,
	    screenPosition.y - offset               // オフセット分上にずらす
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
	    screenPosition.y - offset               // オフセット分上にずらす
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

		// HPが0になった敵を死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;
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
// コライダー更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateCollider() {
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = objectEnemy_->transform_.translate;
		Float3 size = objectEnemy_->transform_.scale;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}

// ---------------------------------------------------------
// 弾の更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateBullets() {
	// 全ての弾を更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// 弾の削除処理
	for (auto& bullet : bullets_) {
		if (bullet->IsDead()) {
			bullet->OnDestroy();
		}
	}
	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(), [](const std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); }), bullets_.end());
}

// ---------------------------------------------------------
// ステート管理
// ---------------------------------------------------------
void NormalEnemy::UpdateState(Player* player) {
	Float3 playerPos = player->GetTranslate();                     // プレイヤー位置
	Float3 enemyPos = this->objectEnemy_->transform_.translate;    // 敵位置
	float distanceToPlayer = Float3::Length(playerPos - enemyPos); // プレイヤーとの距離

	switch (state_) {
	// 警戒ステート更新処理
	case EnemyState::Alert:
		UpdateAlertState(playerPos, enemyPos, distanceToPlayer);
		break;

	// 移動ステート更新処理
	case EnemyState::Move:
		UpdateMoveState(playerPos, enemyPos, distanceToPlayer);
		break;

	// 攻撃ステート更新処理
	case EnemyState::Attack:
		UpdateAttackState(playerPos, enemyPos, distanceToPlayer);
		break;
	}
}

// ---------------------------------------------------------
// 警戒ステート更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateAlertState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer) {
	// プレイヤーが一定距離に入ったら移動ステートへ
	if (distanceToPlayer < detectionRange_) {
		state_ = EnemyState::Move;
	}
}

// ---------------------------------------------------------
// 移動ステート更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateMoveState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer) {
	// プレイヤーの方向へ移動
	Float3 direction = playerPos - enemyPos;
	direction = Float3::Normalize(direction);

	objectEnemy_->transform_.translate += direction * moveSpeed_;

	// 攻撃距離に入ったら攻撃ステートへ
	if (distanceToPlayer < attackRange_) {
		state_ = EnemyState::Attack;
		attackTimer_ = 0.0f;
	}
}

// ---------------------------------------------------------
// 攻撃ステート更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateAttackState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer) {
	// プレイヤーとの間に障害物があるか判定
	RayCastHit hit{};
	bool isBlocked = CollisionManager::GetInstance()->RayCast(enemyPos, Float3::Normalize(playerPos - enemyPos), distanceToPlayer, &hit);

	// 間に障害物があれば警戒ステートへ
	if (isBlocked && hit.hitCollider->GetTag() == "NormalObstacle") {
		state_ = EnemyState::Alert;
		return;
	}

	// 攻撃のクールタイム更新
	attackTimer_ += TimeManager::GetInstance()->GetDeltaTime();
	if (attackTimer_ < attackCooldown_)
		return; // クールダウン中は早期リターン

	attackTimer_ = 0.0f;

	// 発射方向
	Float3 direction = playerPos - enemyPos;
	direction.y = 0.0f;
	direction = Float3::Normalize(direction);

	// 拡散をランダムに設定
	float randSpread = RandomGenerator::GetInstance()->RandomValue(-bulletSpreadAngle_, bulletSpreadAngle_);
	direction.x += randSpread;
	direction.y += randSpread;
	direction = Float3::Normalize(direction);

	// 弾の生成・初期化
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(enemyPos, direction, modelEnemyBullet_);

	bullets_.push_back(std::move(newBullet));
}
