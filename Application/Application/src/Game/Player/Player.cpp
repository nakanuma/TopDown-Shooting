#define NOMINMAX
#include "Player.h"

// C++
#include <random>

// Engine
#include <Camera.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Application
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Utility/Utility.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/System/ResultStats.h>

// externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void Player::Initialize(const Loader::TransformData& data) {
	///
	///	基盤機能
	///

	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 入力
	input_ = Input::GetInstance();

	// スプライト基盤
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(DirectXBase::GetInstance());

	///
	///	アニメーションデータ読み込み
	/// 

	// 前移動
	walkData_.modelData = ModelManager::LoadModelFile("resources/Models/Character/Player", "walk.gltf", dxBase->GetDevice());
	walkData_.modelData.material.textureHandle = TextureManager::Load("resources/Images/Character/Player/player.png", dxBase->GetDevice());
	walkData_.animation = AnimationLoader::LoadAnimation("resources/Models/Character/Player", "walk.gltf");
	walkData_.skeleton.CreateSkeleton(walkData_.modelData.rootNode);

	///
	///	オブジェクト関連
	///

	// プレイヤーオブジェクト生成
	objectPlayer_ = std::make_unique<AnimatedModelInstance>();
	objectPlayer_->Initialize(walkData_);
	objectPlayer_->GetTranslate() = data.translate;
	objectPlayer_->SetPlayBackSpeed(1.5f);

	// 弾モデル読み込み
	modelBullet_ = ModelManager::LoadModelFile("resources/Models", "Bullet/TestBullet/testBullet.obj", dxBase->GetDevice());
	modelBullet_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("Player");
	collider_->SetOwner(this);

	CollisionManager::GetInstance()->Register(collider_.get());
	UpdateCollider(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	///
	///	UI
	///

	ui_ = std::make_unique<PlayerUIManager>();
	ui_->Initialize();

	///
	///	パラメーター設定
	///

	currentHP_ = kMaxHP;     // 現在HPには最大HPをセット

	///
	///	調整パラメーター登録
	///

	RegisterParam("speed", &speed_, 0.0f, 10.0f, 0.01f);
	RegisterParam("fireCooldown", &fireCooldown_, 0.0f, 5.0f, 0.01f);

	SetConfigPath("Player/playerConfig.json"); // ファイルパス設定
	InitConfig(); // 初回読み込み
}

// ---------------------------------------------------------
// 毎フレーム更新処理
// ---------------------------------------------------------
void Player::Update() {
	///
	///	内部処理
	///

	// カーソル方向へ向くよう回転
	FaceCursor();
	// 移動処理
	HandleMove();
	// 射撃 & オーバーヒート処理
	/*HandleOverHeat();*/
	// HPが0未満にならないよう制限
	currentHP_ = std::clamp(currentHP_, 0, kMaxHP);

	///
	///	コライダー更新処理
	///

	UpdateCollider();

	///
	///	オブジェクト更新処理
	///

	// プレイヤーオブジェクト更新
	objectPlayer_->Update(TimeManager::GetInstance()->GetDeltaTime(), isMoving_);
	objectPlayer_->object_->UpdateShadowMatrix();

	///
	///	UI更新処理
	///
	
	ui_->Update(this);
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void Player::Draw() {
	// プレイヤーオブジェクト描画
	objectPlayer_->Draw();
}

// ---------------------------------------------------------
// シャドウマップ描画処理
// ---------------------------------------------------------
void Player::DrawShadow() {
	objectPlayer_->object_->DrawShadow();
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void Player::DrawUI() { 
	ui_->Draw(); 
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void Player::OnCollision(Collider* other) {
	///
	/// vs NormalEnemy
	///
	if (other->GetTag() == "NormalEnemy") {
		/*currentHP_--;*/
	}

	///
	///	vs EnemyBullet
	///

	if (other->GetTag() == "EnemyBullet") {
		// EnemyBulletのDamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
	}

	///
	///	vs HomingMissile
	///

	if (other->GetTag() == "HomingMissile") {
		// HomingMissileのDamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
	}

	///
	///	vs GroundWarning
	///

	if (other->GetTag() == "GroundWarning") {
		// HomingMissileのDamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
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
			// プレイヤー位置を補正
			objectPlayer_->GetTranslate() += pushVec;

			// コライダーも更新しておく
			myAABB->min_ += pushVec;
			myAABB->max_ += pushVec;
		}
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void Player::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Player");

	/* Translate */
	ImGui::Text("Translate");

	ImGui::DragFloat3("translate", &objectPlayer_->GetTranslate().x, 0.01f);

	ImGui::DragFloat3("rotate", &objectPlayer_->GetRotate().x, 0.01f);

	ImGui::DragFloat3("scale", &objectPlayer_->GetScale().x, 0.01f);

	/* Parameter */
	ImGui::Text("Parameter");

	ImGui::BeginDisabled(true); // 操作不可
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);
	ImGui::EndDisabled();

	ImGui::DragFloat("Speed", &speed_, 0.01f);

	ImGui::DragInt("HP", &currentHP_);

	ImGui::Text("dashCooldown : %.2f", dashCooldownTimer_);

	// オーバーヒート
	ImGui::Text("overheatTime : %.2f", overheatTime_);
	ImGui::Checkbox("isOverheated", &isOverheated_);

	ImGui::Checkbox("isMoving", &isMoving_);

	/*  */

	ImGui::End();


	// コンフィグウインドウ
	DrawConfigWindow("playerConfig");
#endif //  _DEBUG
}

// ---------------------------------------------------------
// カーソル方向へ向くよう回転させる
// ---------------------------------------------------------
void Player::FaceCursor() {
	// プレイヤーからカーソルへの方向ベクトル
	Float3 direction = Utility::CalclateCursorPosition() - objectPlayer_->GetTranslate();

	// 方向ベクトルからY軸回転角度を計算
	float angle = std::atan2(direction.x, direction.z);

	// 回転方向の分割
	/*const int32_t division = 32;
	float step = (PIf * 2.0f) / division;
	angle = std::round(angle / step) * step;*/

	// Y軸に回転を適用
	objectPlayer_->GetRotate().y = angle;
}

// ---------------------------------------------------------
// 移動処理
// ---------------------------------------------------------
void Player::HandleMove() {
	velocity_ = { 0.0f, 0.0f, 0.0f };

	if (input_->PushKey(DIK_W))
		velocity_.z += 1.0f;
	if (input_->PushKey(DIK_S))
		velocity_.z -= 1.0f;
	if (input_->PushKey(DIK_A))
		velocity_.x -= 1.0f;
	if (input_->PushKey(DIK_D))
		velocity_.x += 1.0f;

	// 移動しているか
	isMoving_ = (velocity_.x != 0.0f || velocity_.z != 0.0f);

	// 正規化
	if (isMoving_) {
		velocity_ = Float3::Normalize(velocity_);
	}

	///
	///	ダッシュ処理
	///

	// クールタイム更新
	if (dashCooldownTimer_ > 0.0f) {
		dashCooldownTimer_ -= TimeManager::GetInstance()->GetDeltaTime();
	}

	// ダッシュ中処理
	if (isDashing_) {
		dashTimer_ -= TimeManager::GetInstance()->GetDeltaTime();
		if (dashTimer_ <= 0.0f) {
			isDashing_ = false;                 // ダッシュ終了
			dashCooldownTimer_ = kDashCoolDown; // クールタイムをセット
		}
	}

	// ダッシュ入力
	bool dashInput = input_->TriggerKey(DIK_LSHIFT) || input_->IsTriggerMouse(2); // 左SHIFT or 中央クリック
	if (!isDashing_ && dashCooldownTimer_ <= 0.0f && dashInput) {
		isDashing_ = true;          // ダッシュ中へ
		dashTimer_ = kDashDuration; // ダッシュ時間をセット
	}

	// 速度を更新
	float currentSpeed = speed_;
	if (isDashing_) {
		currentSpeed *= kDashSpeedMultiplier; // ダッシュ中は速度に倍率をかける
	}
	velocity_ = velocity_ * currentSpeed;

	// プレイヤー位置更新
	objectPlayer_->GetTranslate() += velocity_;
}

// ---------------------------------------------------------
// 弾の発射処理
// ---------------------------------------------------------
void Player::HandleShooting() {
	///
	///	左クリックで弾の生成
	///

	// 左クリックで弾を生成
	if (input_->IsPressMouse(0)) {
		// カーソル位置の取得
		Float3 cursorPos = Utility::CalclateCursorPosition();
		// プレイヤー位置の取得
		Float3 playerPos = objectPlayer_->GetTranslate();

		// 発射方向
		Float3 direction = cursorPos - playerPos;
		direction.y = 0.0f;
		direction = Float3::Normalize(direction);

		// 少しだけ方向をブレさせる
		float blurAmount = kMaxRandomAngle;

		if (Float3::Length(velocity_) > 0.01f) {
			blurAmount *= 3.0f; // プレイヤーが動いていたらブレの幅を増やす
		}

		std::mt19937 rng(std::random_device{}());
		std::uniform_real_distribution<float> blurDist(-blurAmount, blurAmount);

		// Y成分以外のランダムベクトルを加算
		direction.x += blurDist(rng);
		direction.z += blurDist(rng);
		direction = Float3::Normalize(direction); // 再正規化

		// 弾の生成・初期化
		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(objectPlayer_->GetTranslate(), direction, &modelBullet_);
		BulletManager::GetInstance()->AddBullet(std::move(newBullet));
		ResultStats::GetInstance()->AddShot(); // 弾を撃ったことを記録
	}
}

// ---------------------------------------------------------
// オーバーヒートの管理処理
// ---------------------------------------------------------
void Player::HandleOverHeat()
{
	// 左クリック取得
	isFiring_ = Input::GetInstance()->IsPressMouse(0);

	// 発射タイマーを進める
	fireTimer_ += TimeManager::GetInstance()->GetDeltaTime();

	// オーバーヒートしていない場合の処理
	if (!isOverheated_ && isFiring_) {
		// オーバーヒート処理
		overheatTime_ += overheatGainPerSecond_ * TimeManager::GetInstance()->GetDeltaTime();
		if (overheatTime_ >= kOverheatLimit) {
			overheatTime_ = kOverheatLimit;
			isOverheated_ = true;
		}

		// 射撃処理
		if (fireTimer_ >= fireCooldown_) {
			HandleShooting();
			fireTimer_ = 0.0f;
		}

		// オーバーヒート中の処理
	} else {
		// 冷却処理（撃っていない間 or オーバーヒート中）
		overheatTime_ -= overheatRecoverySpeed_ * TimeManager::GetInstance()->GetDeltaTime();
		overheatTime_ = std::max(overheatTime_, 0.0f);

		// 冷却時間完了でオーバーヒート解除
		if (isOverheated_ && overheatTime_ <= 0.0f) {
			isOverheated_ = false;
		}
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void Player::UpdateCollider() {
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = objectPlayer_->GetTranslate();
		Float3 size = kColliderSize;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}