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
	objectEnemy_->transform_.scale = { 1.0f, 1.0f, 1.0f };
	objectEnemy_->transform_.rotate = { 0.0f, std::numbers::pi_v<float>, 0.0f }; // 手前を向いた状態でスポーン（一時的に）
	objectEnemy_->materialCB_.data_->color = { 1.0f, 0.5f, 0.0f, 1.0f };

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);
	colliderSize_ = { 1.0f, 2.0f, 1.0f };

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
	spriteHPBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f }); // 黒

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({ 0.0f, 1.0f, 0.5f, 1.0f }); // 緑


	// リロード表示
	uint32_t textureReload = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteReload_ = std::make_unique<Sprite>();
	spriteReload_->Initialize(spriteCommon_.get(), textureReload);
	spriteReload_->SetSize(kReloadSize);
	spriteReload_->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }); // 白

	///
	///	パラメーター設定
	///

	isDead_ = false;

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

	// リロード表示更新
	spriteReload_->Update();
}

// ---------------------------------------------------------
// 弾の更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateBullets()
{
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

	bullets_.erase(std::remove_if(bullets_.begin(), bullets_.end(),
		[](const std::unique_ptr<Bullet>& bullet) { return bullet->IsDead(); }),
		bullets_.end());
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


	///
	///	リロード表示
	/// 

	// 上にずらす分のオフセット
	const float kOffsetReload = 60.0f;

	// リロード時間割合
	float reloadRatio = reloadTimer_ / kReloadTime;

	// リロード時間に応じてサイズ変更
	spriteReload_->SetSize({
		kReloadSize.x - (kReloadSize.x * reloadRatio),
		kReloadSize.y
		});

	// スクリーン座標をセット
	spriteReload_->SetPosition({
		screenPosition.x - kReloadSize.x / 2.0f, // リロード表示が中心になるよう設定
		screenPosition.y - kOffsetReload
		});

	// リロード時のみ描画
	if (isReloading_) {
		spriteReload_->Draw();
	}
}


// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void NormalEnemy::OnCollision(Collider* other) {
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		///
		///	現在が警戒ステートなら移動ステートへ移行
		/// 
		if (state_ == EnemyState::Alert) {
			state_ = EnemyState::Move;
		}

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
		Float3 size = colliderSize_;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
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

	default:
		break;
	}
}

// ---------------------------------------------------------
// 警戒ステート更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateAlertState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer) {
	///
	///	プレイヤーを発見したら移動ステートへ
	/// 

	// プレイヤー方向へのレイキャスト
	RayCastHit hit{};
	bool rayCastHit = CollisionManager::GetInstance()->RayCast(enemyPos, Float3::Normalize(playerPos - enemyPos), distanceToPlayer, &hit);

	bool isInDetectionRange = distanceToPlayer < detectionRange_; // プレイヤーが索敵範囲内かどうか
	bool hasLineOfSight = rayCastHit && hit.hitCollider->GetTag() != "NormalObstacle"; // プレイヤーとの間に障害物がない場合（視線が通っている場合）

	if (isInDetectionRange && hasLineOfSight) {
		state_ = EnemyState::Move;
	}

	///
	///	警戒モーション処理（待機 -> 回転 -> 待機 -> 移動 -> 待機 -> 回転...）
	/// 

	alertStateTimer_ += TimeManager::GetInstance()->GetDeltaTime();

	switch (alertSubState_) {
	case AlertSubState::Rotate: {
		if (alertRotateDuration_ == 0.0f) {
			alertRotateDuration_ = RandomGenerator::GetInstance()->RandomValue(kMinRotateTime, kMaxRotateTime); // ランダムな回転時間を設定
			isRotatingRight_ = RandomGenerator::GetInstance()->RandomValueBool();
		}

		// 回転方向への回転適用
		float rotateSpeed = (isRotatingRight_ ? 1.0f : -1.0f) * rotationSpeed_;
		objectEnemy_->transform_.rotate.y += rotateSpeed;

		// 回転時間を過ぎたら回転後ステートへ
		if (alertStateTimer_ >= alertRotateDuration_) {
			alertStateTimer_ = 0.0f; // タイマーリセット
			alertRotateDuration_ = 0.0f; // 回転時間リセット
			alertSubState_ = AlertSubState::WaitAfterRotate;
		}

		break;
	}
	case AlertSubState::WaitAfterRotate: {
		if (waitDuration_ == 0.0f) {
			waitDuration_ = RandomGenerator::GetInstance()->RandomValue(kMinWaitTime, kMaxWaitTime); // 待機時間をランダムに設定
		}

		// 待機時間が過ぎたら次ステートへ
		if (alertStateTimer_ >= waitDuration_) {
			alertStateTimer_ = 0.0f; // タイマーリセット
			waitDuration_ = 0.0f; // 待機時間リセット
			
			// ランダムで直進ステートor回転ステートへ
			bool goForward = RandomGenerator::GetInstance()->RandomValueBool(kMoveForwardProbability);
			if (goForward) {
				alertSubState_ = AlertSubState::MoveForward;
			} else {
				alertSubState_ = AlertSubState::Rotate;
			}
		}

		break;
	}
	case AlertSubState::MoveForward: {
		if (moveForwardDuration_ == 0.0f) {
			moveForwardDuration_ = RandomGenerator::GetInstance()->RandomValue(kMinMoveTime, kMaxMoveTime); // 直進時間をランダムに設定
		}

		// 向いている方向へ直進
		Float3 forward = { std::sinf(objectEnemy_->transform_.rotate.y), 0.0f, std::cosf(objectEnemy_->transform_.rotate.y) };
		objectEnemy_->transform_.translate += forward * (moveSpeed_ * 0.5f); // ゆっくりめに移動

		// 直進時間を過ぎたら直進後ステートへ
		if (alertStateTimer_ >= moveForwardDuration_) {
			alertStateTimer_ = 0.0f; // タイマーリセット
			moveForwardDuration_ = 0.0f; // 直進時間リセット
			alertSubState_ = AlertSubState::WaitAfterMove;
		}

		break;
	}
	case AlertSubState::WaitAfterMove: {
		if (waitDuration_ == 0.0f) {
			waitDuration_ = RandomGenerator::GetInstance()->RandomValue(kMinWaitTime, kMaxWaitTime); // 待機時間をランダムに設定
		}

		// 待機時間が過ぎたら回転ステートへ
		if (alertStateTimer_ >= waitDuration_) {
			alertStateTimer_ = 0.0f; // タイマーリセット
			waitDuration_ = 0.0f; // 待機時間リセット
			alertSubState_ = AlertSubState::Rotate;
	
		}

		break;
	}
	default:

		break;

	}
}

// ---------------------------------------------------------
// 移動ステート更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateMoveState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer) {
	moveStateTimer_ += TimeManager::GetInstance()->GetDeltaTime();

	///
	/// プレイヤーとの視線チェック（障害物判定）
	/// 

	// プレイヤー方向へのレイキャスト
	RayCastHit hit{};
	bool rayCastHit = CollisionManager::GetInstance()->RayCast(enemyPos, Float3::Normalize(playerPos - enemyPos), distanceToPlayer, &hit);

	// 障害物に遮られている時間を計測
	if (rayCastHit && hit.hitCollider->GetTag() == "NormalObstacle") {
		obstacleLostTimer_ += TimeManager::GetInstance()->GetDeltaTime();
		// 上限に達したら警戒ステートへ
		if (obstacleLostTimer_ >= kLostTime) {
			state_ = EnemyState::Alert;
			moveStateTimer_ = 0.0f;
			obstacleLostTimer_ = 0.0f;
			return;
		}
	// 遮られていなければタイマーリセット
	} else {
		obstacleLostTimer_ = 0.0f;
	}

	///
	///	プレイヤー方向へ向く
	/// 

	// プレイヤーへの方向ベクトル
	Float3 toPlayer = playerPos - enemyPos;
	// 方向ベクトルからY軸回転角度を計算
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸に回転を適用
	objectEnemy_->transform_.rotate.y = targetAngle;

	///
	/// プレイヤーとの距離に応じた動き
	///

	Float3 moveDir;
	// 適正戦闘範囲外なら接近
	if (distanceToPlayer > kOptimalCombatRange) {
		moveDir = Float3::Normalize(toPlayer);
	}
	// 適正戦闘範囲内なら離脱
	else if (distanceToPlayer < kOptimalCombatRange * 0.7f) {
		moveDir = Float3::Normalize(toPlayer * -1.0f);
	}
	// それ以外は斜め移動
	else {
		moveDir = Float3::Normalize({ -toPlayer.z, 0.0f, toPlayer.x });
	}

	// 移動を適用
	objectEnemy_->transform_.translate += moveDir * moveSpeed_;

	///
	///	攻撃ステートへ
	/// 

	// 一時的に2秒移動したら攻撃ステートへ
	if (moveStateTimer_ >= 2.0f) {
		state_ = EnemyState::Attack;
		attackStateTimer_ = 0.0f;
	}

}

// ---------------------------------------------------------
// 攻撃ステート更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateAttackState(const Float3& playerPos, const Float3& enemyPos, float distanceToPlayer) {
	///
	///	今回発射する予定の弾数をランダムに設定
	/// 

	if (attackStateTimer_ == 0.0f && bulletsShotInThisAttack_ == 0) {
		bulletsToShot_ = RandomGenerator::GetInstance()->RandomValue(kMinShotThisTime, kMaxShotThisTime);
	}

	///
	///	タイマー加算
	/// 

	attackStateTimer_ += TimeManager::GetInstance()->GetDeltaTime();

	///
	/// プレイヤーとの視線チェック（障害物判定）
	/// 

	// プレイヤー方向へのレイキャスト
	RayCastHit hit{};
	bool rayCastHit = CollisionManager::GetInstance()->RayCast(enemyPos, Float3::Normalize(playerPos - enemyPos), distanceToPlayer, &hit);

	// 障害物に遮られている時間を計測
	if (rayCastHit && hit.hitCollider->GetTag() == "NormalObstacle") {
		obstacleLostTimer_ += TimeManager::GetInstance()->GetDeltaTime();
		// 上限に達したら警戒ステートへ
		if (obstacleLostTimer_ >= kLostTime) {
			state_ = EnemyState::Alert;
			moveStateTimer_ = 0.0f;
			obstacleLostTimer_ = 0.0f;
			return;
		}
		// 遮られていなければタイマーリセット
	} else {
		obstacleLostTimer_ = 0.0f;
	}

	///
	///	プレイヤー方向へ向く
	/// 

	// プレイヤーへの方向ベクトル
	Float3 toPlayer = playerPos - enemyPos;
	// 方向ベクトルからY軸回転角度を計算
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸に回転を適用
	objectEnemy_->transform_.rotate.y = targetAngle;

	///
	///	リロード処理
	/// 
	
	if (isReloading_) {
		reloadTimer_ += TimeManager::GetInstance()->GetDeltaTime();
		// リロード時間に達したらリロード完了
		if (reloadTimer_ >= kReloadTime) {
			bulletRemaining_ = kMaxBullet; // 最大弾数を込める
			reloadTimer_ = 0.0f; // リロードタイマーリセット
			isReloading_ = false; // リロード終了

			// 移動ステートへ
			state_ = EnemyState::Move;
			moveStateTimer_ = 0.0f;
		}
		return;
	}

	///
	///	発射間隔の調整
	/// 

	if (attackStateTimer_ < nextShotInterval_) {
		return;
	}

	///
	///	弾数制限処理
	/// 

	// 残弾が0になったらリロード開始
	if (bulletRemaining_ <= 0) {
		isReloading_ = true;
		reloadTimer_ = 0.0f;
		return;
	}

	///
	///	弾生成処理
	/// 

	// 発射方向
	Float3 direction = playerPos - enemyPos;
	// 拡散をランダムに設定
	float randSpread = RandomGenerator::GetInstance()->RandomValue(-bulletSpreadAngle_, bulletSpreadAngle_);
	direction.x += randSpread;
	direction.z += randSpread;
	direction = Float3::Normalize(direction);

	// 弾の生成
	auto newBullet = std::make_unique<EnemyBullet>();
	newBullet->Initialize(enemyPos, direction, modelEnemyBullet_);
	bullets_.push_back(std::move(newBullet));

	bulletRemaining_--; // 残弾を減らす
	attackStateTimer_ = 0.0f; // 攻撃ステートタイマーをリセット
	nextShotInterval_ = RandomGenerator::GetInstance()->RandomValue(kMinShotInterval, kMaxShotInterval); // 次までの発射間隔をランダムに設定

	///
	///	今回撃つ予定の弾数を撃ち終えたら移動ステートへ
	/// 
	
	bulletsShotInThisAttack_++;
	if (bulletsShotInThisAttack_ >= bulletsToShot_) {
		state_ = EnemyState::Move;
		moveStateTimer_ = 0.0f;
		bulletsShotInThisAttack_ = 0; // 今回撃った弾数カウントをリセット
	}
}
