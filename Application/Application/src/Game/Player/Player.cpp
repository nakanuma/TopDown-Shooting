#define NOMINMAX
#include "Player.h"

// C++
#include <random>

// Engine
#include <Camera.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <RandomGenerator.h>

// Application
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Utility/Utility.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/System/ResultStats.h>

// externals
#include <ImguiWrapper.h>

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
	walkData_.modelData = ModelManager::LoadModelFile("Character/Player/walk.gltf");
	walkData_.modelData.material.textureHandle = TextureManager::Load("Character/Player/player.png");
	walkData_.animation = AnimationLoader::LoadAnimation("resources/Models", "Character/Player/walk.gltf");
	walkData_.skeleton.CreateSkeleton(walkData_.modelData.rootNode);

	///
	///	オブジェクト関連
	///

	// プレイヤーオブジェクト生成
	objectPlayer_ = std::make_unique<AnimatedModelInstance>();
	objectPlayer_->Initialize(walkData_);
	objectPlayer_->GetTranslate() = data.translate;
	objectPlayer_->SetPlayBackSpeed(1.5f);

	// 銃オブジェクト生成
	objectGun_ = std::make_unique<Object3D>();
	objectGun_->model_ = &ModelManager::GetInstance()->GetModel("Gun");
	objectGun_->materialCB_.data_->color = {0.0f, 0.0f, 0.0f, 1.0f};
	objectGun_->materialCB_.data_->useEnvironmentMap = true;
	objectGun_->materialCB_.data_->environmentStrength = 0.2f;


	///
	///	コライダー生成
	///

	auto aabb = std::make_unique<AABBCollider>();
	aabb->SetTag("Player");
	aabb->SetFollowTarget(&objectPlayer_->GetTranslate());
	aabb->SetSize(kColliderSize);
	aabb->SetOwner(this);

	collider_ = std::move(aabb);
	CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

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

void Player::Update(bool operable) {
	// 前フレームでの死亡フラグを保持
	bool wasDead = isDead_;

	///
	///	内部処理
	///

	// 操作可能かつ生きている間のみ入力操作を有効に
	if (operable && !isDead_) {
		// カーソル方向へ向くよう回転
		FaceCursor();
		// 射撃 & オーバーヒート処理
		HandleOverHeat();
		// 移動処理
		HandleMove();
	}

	// HPが0未満にならないよう制限
	currentHP_ = std::clamp(currentHP_, 0, kMaxHP);

	// HPが0になったら死亡
	if (currentHP_ <= 0) {
		isDead_ = true;

		// 死亡したフレームのみパーティクルを発生
		if(!wasDead){
			ParticleEffectManager::GetInstance()->Emit("bloodSplatter", this->GetTranslate(), 30);
		}
	}

	///
	///	コライダー更新処理
	///

	collider_->Update();

	///
	///	オブジェクト更新処理
	///

	// プレイヤーオブジェクト更新
	objectPlayer_->Update(TimeManager::GetInstance()->GetDeltaTime(), isMoving_);
	objectPlayer_->object_->UpdateShadowMatrix();

	// 銃オブジェクト更新
	Float3 playerPos = objectPlayer_->GetTranslate();
	Float3 playerRot = objectPlayer_->GetRotate();
	Float3 forward = {sinf(playerRot.y), 0.0f, cosf(playerRot.y)}; // 前方向ベクトル
	Float3 right = {cosf(playerRot.y), 0.0f, -sinf(playerRot.y)}; // 右方向ベクトル

	const float gunForwardOffset = 1.1f; // 前方方向へのオフセット
	const float gunRightOffset = 0.3f; // 右方向のオフセット
	objectGun_->transform_.translate = playerPos + (forward * gunForwardOffset) + (right * gunRightOffset);
	objectGun_->transform_.rotate = playerRot;

	objectGun_->UpdateMatrix();

	///
	///	UI更新処理
	///

	ui_->Update(this);
}

void Player::Draw() {
	// 死亡したら描画スキップ
	if(isDead_) return;

	objectPlayer_->Draw();
	objectGun_->Draw();
}

void Player::DrawShadow() {
	// 死亡したら描画スキップ
	if (isDead_) return;

	objectPlayer_->DrawShadow();
}

void Player::DrawGunShadow()
{
	// 死亡したら描画スキップ
	if (isDead_) return;

	objectGun_->DrawShadow();
}

void Player::DrawUI() {
	// 死亡状態ならスキップ
	if(IsDead()) return;

	ui_->Draw();
}

void Player::OnCollision(Collider* other) {
	///
	/// vs NormalEnemy
	///
	if (other->GetTag() == "NormalEnemy") {
		
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
			Float3 currentMin = myAABB->GetMin();
			Float3 currentMax = myAABB->GetMax();
			myAABB->SetMin(currentMin + pushVec);
			myAABB->SetMax(currentMax + pushVec);
		}
	}
}

void Player::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Player");

	/* Translate */
	ImGui::Text("Translate");

	ImGui::DragFloat3("translate", &objectPlayer_->GetTranslate().x, 0.01f);

	ImGui::DragFloat3("rotate", &objectPlayer_->GetRotate().x, 0.01f);

	ImGui::DragFloat3("scale", &objectPlayer_->GetScale().x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat3("Gun:Translate", &objectGun_->transform_.translate.x, 0.01f);
	ImGui::DragFloat3("Gun:Rotate", &objectGun_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Gun:Scale", &objectGun_->transform_.scale.x, 0.01f);

	/* Parameter */
	ImGui::Text("Parameter");

	ImGui::Checkbox("isDead", &isDead_);

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

void Player::FaceCursor() {
	// プレイヤーからカーソルへの方向ベクトル
	Float3 direction = Utility::CalculateCursorPosition() - objectPlayer_->GetTranslate();

	// 方向ベクトルからY軸回転角度を計算
	float angle = std::atan2(direction.x, direction.z);

	// カーソルがクライアント領域内にある場合のみ回転を適用
	if (Utility::IsInsideClientCursor()) {
		// Y軸に回転を適用
		objectPlayer_->GetRotate().y = angle;
	}
}

void Player::HandleMove() {
	velocity_ = { 0.0f, 0.0f, 0.0f };

	// キー入力で速度ベクトル加算
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

void Player::HandleShooting() {
	///
	///	左クリックで弾の生成
	///

	// 左クリックで弾を生成
	if (input_->IsPressMouse(0) && Utility::IsInsideClientCursor()) {
		// カーソル位置の取得
		Float3 cursorPos = Utility::CalculateCursorPosition();
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

		float blurDist = RandomGenerator::GetInstance()->RandomValue(-blurAmount, blurAmount);

		// Y成分以外のランダムベクトルを加算
		direction.x += blurDist;
		direction.z += blurDist;
		direction = Float3::Normalize(direction); // 再正規化

		// 弾の生成・初期化
		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(objectPlayer_->GetTranslate(), direction, &ModelManager::GetInstance()->GetModel("Bullet"));
		BulletManager::GetInstance()->AddBullet(std::move(newBullet));
		ResultStats::GetInstance()->AddShot(); // 弾を撃ったことを記録

		// パーティクル発生
		ParticleEffectManager::GetInstance()->Emit("shellEjection", objectGun_->transform_.translate, 1, { 0.0f, 0.0f, 0.0f }, objectGun_->transform_.rotate.y); // 薬莢排出
		
		Float3 forward = { sinf(objectGun_->transform_.rotate.y), 0.0f, cosf(objectGun_->transform_.rotate.y) }; // 前方向ベクトル
		const float forwardOffset = 1.4f; // 前方方向へのオフセット
		ParticleEffectManager::GetInstance()->Emit("muzzleFlash", objectGun_->transform_.translate + (forward * forwardOffset), 6); // マズルフラッシュ
	}
}

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