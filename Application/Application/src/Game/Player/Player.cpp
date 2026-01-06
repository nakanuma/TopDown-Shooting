#define NOMINMAX
#include "Player.h"

// C++
#include <random>

// Engine
#include <Camera.h>
#include <Engine/Util/TimeManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <RandomGenerator.h>
#include <Easing.h>
#include <SoundManager.h>

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

	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// 入力
	input_ = Cygnus::Input::GetInstance();

	// スプライト基盤
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(Cygnus::DirectXBase::GetInstance());

	///
	///	アニメーションデータ読み込み
	/// 

	// 前移動
	walkData_.modelData = Cygnus::ModelManager::LoadModelFile("Character/Player/walk.gltf");
	walkData_.modelData.material.textureHandle = Cygnus::TextureManager::Load("Character/Player/player.png");
	walkData_.animation = Cygnus::AnimationLoader::LoadAnimation("resources/Models", "Character/Player/walk.gltf");
	walkData_.skeleton.CreateSkeleton(walkData_.modelData.rootNode);

	///
	///	オブジェクト関連
	///

	// プレイヤーオブジェクト生成
	objectPlayer_ = std::make_unique<Cygnus::AnimatedModelInstance>();
	objectPlayer_->Initialize(walkData_);
	objectPlayer_->GetTranslate() = data.translate;
	objectPlayer_->SetPlayBackSpeed(kAnimationPlaybackSpeed);
	objectPlayer_->object_->materialCB_.data_->emissiveColor = kHitBlinkColor;

	// 銃オブジェクト生成
	objectGun_ = std::make_unique<Cygnus::Object3D>();
	objectGun_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("Gun");
	objectGun_->materialCB_.data_->color = kGunColor;
	objectGun_->materialCB_.data_->useEnvironmentMap = true;
	objectGun_->materialCB_.data_->environmentStrength = kGunEnvironmentStrength;

	///
	///	コライダー生成
	///

	auto aabb = std::make_unique<Cygnus::AABBCollider>();
	aabb->SetTag("Player");
	aabb->SetFollowTarget(&objectPlayer_->GetTranslate());
	aabb->SetSize(kColliderSize);
	aabb->SetOwner(this);

	collider_ = std::move(aabb);
	Cygnus::CollisionManager::GetInstance()->Register(collider_.get());

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
	invincible_ = false;     // 開始時は非無敵状態

	///
	///	調整パラメーター登録
	///

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

	// 被弾時の発光演出
	HandleHitBlink();
	// 被弾時のダメージ演出
	HandleDamageEffect();

	// HPが0になったら死亡
	if (currentHP_ <= 0) {
		isDead_ = true;

		// 死亡したフレームのみパーティクルを発生
		if (!wasDead) {
			// 死亡時パーティクル発生
			Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", GetTranslate(), kDeathCrossCount, { 0.0f, 0.0f, 0.0f }, Cygnus::DegToRad(kDeathCrossAngle1));
			Cygnus::ParticleEffectManager::GetInstance()->Emit("deathCross", GetTranslate(), kDeathCrossCount, { 0.0f, 0.0f, 0.0f }, Cygnus::DegToRad(kDeathCrossAngle2));

			Cygnus::ParticleEffectManager::GetInstance()->Emit("bloodSplatter", this->GetTranslate(), kBloodSplatterCount);
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
	objectPlayer_->Update(Cygnus::TimeManager::GetInstance()->GetDeltaTime(), isMoving_);
	objectPlayer_->object_->UpdateShadowMatrix();

	// 銃オブジェクト更新
	Cygnus::Float3 playerPos = objectPlayer_->GetTranslate();
	Cygnus::Float3 playerRot = objectPlayer_->GetRotate();
	Cygnus::Float3 forward = { sinf(playerRot.y), 0.0f, cosf(playerRot.y) }; // 前方向ベクトル
	Cygnus::Float3 right = { cosf(playerRot.y), 0.0f, -sinf(playerRot.y) }; // 右方向ベクトル

	objectGun_->transform_.translate_ = playerPos + (forward * kGunForwardOffset) + (right * kGunRightOffset);
	objectGun_->transform_.rotate_ = playerRot;

	objectGun_->UpdateMatrix();
	objectGun_->UpdateShadowMatrix();

	///
	///	UI更新処理
	///

	ui_->Update(this);
}

void Player::Draw() {
	// 死亡したら描画スキップ
	if (isDead_) return;

	objectPlayer_->Draw();
	objectGun_->Draw();
}

void Player::DrawShadow() {
	objectGun_->DrawShadow();
}

void Player::DrawShadowSkinning()
{
	objectPlayer_->DrawShadow();
}

void Player::DrawUI() {
	ui_->Draw();
}

void Player::OnCollision(Cygnus::Collider* other) {
	///
	/// vs NormalEnemy
	///
	if (other->GetTag() == "NormalEnemy") {

	}

	///
	///	vs Bullet
	///

	if (other->GetTag() == "EnemyBullet" || other->GetTag() == "HomingMissile" || other->GetTag() == "GroundWarning") {
		// 無敵状態であればスキップ
		if (invincible_) return;
		
		// 弾のダメージを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;

		// 被弾時の演出を開始
		if (!isDead_) {
			// 発光演出
			isHitBlink_ = true;
			hitBlinkPhase_ = HitBlinkPhase::BlinkIn;
			hitBlinkTimer_ = 0.0f;

			// ダメージエフェクト演出
			if (postEffectManager_->damageVignetteCB_.data_->intensity < kDamageEffectIntensityThreshold) { // 閾値を超えていたら演出を行わない
				isReceiveDamage_ = true;
				damageEffectPhase_ = DamageEffectPhase::In;
				damageEffectTimer_ = 0.0f;
			}
		}
	}

	///
	/// vs Obstacle
	///
	if (other->GetTag() == "Obstacle") {
		Cygnus::AABBCollider* myAABB = dynamic_cast<Cygnus::AABBCollider*>(collider_.get());
		Cygnus::AABBCollider* otherAABB = dynamic_cast<Cygnus::AABBCollider*>(other);

		// 押し戻し処理
		if (myAABB && otherAABB) {
			// 押し戻しベクトル取得
			Cygnus::Float3 pushVec = myAABB->GetPushBackVector(*otherAABB);
			// プレイヤー位置を補正
			objectPlayer_->GetTranslate() += pushVec;

			// コライダーも更新しておく
			Cygnus::Float3 currentMin = myAABB->GetMin();
			Cygnus::Float3 currentMax = myAABB->GetMax();
			myAABB->SetMin(currentMin + pushVec);
			myAABB->SetMax(currentMax + pushVec);
		}
	}
}

void Player::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("Player");

	ImGui::DragFloat3("emissiveColor", &objectPlayer_->object_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("emissiveIntensity", &objectPlayer_->object_->materialCB_.data_->emissiveIntensity, 0.01f);

	/* Translate */
	ImGui::Text("Translate");

	ImGui::DragFloat3("translate", &objectPlayer_->GetTranslate().x, 0.01f);

	ImGui::DragFloat3("rotate", &objectPlayer_->GetRotate().x, 0.01f);

	ImGui::DragFloat3("scale", &objectPlayer_->GetScale().x, 0.01f);

	ImGui::Separator();
	ImGui::DragFloat3("Gun:Translate", &objectGun_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("Gun:Rotate", &objectGun_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("Gun:Scale", &objectGun_->transform_.scale_.x, 0.01f);

	/* Parameter */
	ImGui::Text("Parameter");

	ImGui::Checkbox("isDead", &isDead_);
	ImGui::Checkbox("invincible", &invincible_);

	ImGui::BeginDisabled(true); // 操作不可
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);
	ImGui::EndDisabled();

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
#endif
}

void Player::FaceCursor() {
	// プレイヤーからカーソルへの方向ベクトル
	Cygnus::Float3 direction = Utility::CalculateCursorPosition() - objectPlayer_->GetTranslate();

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
		velocity_.z += kVelocityNormalizeAdditive;
	if (input_->PushKey(DIK_S))
		velocity_.z -= kVelocityNormalizeAdditive;
	if (input_->PushKey(DIK_A))
		velocity_.x -= kVelocityNormalizeAdditive;
	if (input_->PushKey(DIK_D))
		velocity_.x += kVelocityNormalizeAdditive;

	// 移動しているか
	isMoving_ = (velocity_.x != 0.0f || velocity_.z != 0.0f);

	// 正規化
	if (isMoving_) {
		velocity_ = Cygnus::Float3::Normalize(velocity_);
	}

	///
	///	ダッシュ処理
	///

	// クールタイム更新
	if (dashCooldownTimer_ > 0.0f) {
		dashCooldownTimer_ -= Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	}

	// ダッシュ中処理
	if (isDashing_) {
		dashTimer_ -= Cygnus::TimeManager::GetInstance()->GetDeltaTime();
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
	float currentSpeed = kMoveSpeed;
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
		Cygnus::SoundManager::GetInstance()->Play("shot");

		// カーソル位置の取得
		Cygnus::Float3 cursorPos = Utility::CalculateCursorPosition();
		// プレイヤー位置の取得
		Cygnus::Float3 playerPos = objectPlayer_->GetTranslate();

		// 発射方向
		Cygnus::Float3 direction = cursorPos - playerPos;
		direction.y = 0.0f;
		direction = Cygnus::Float3::Normalize(direction);

		// 少しだけ方向をブレさせる
		float blurAmount = kMaxRandomAngle;

		if (Cygnus::Float3::Length(velocity_) > kVelocityThreshold) {
			blurAmount *= kShootingBlurMultiplier; // プレイヤーが動いていたらブレの幅を増やす
		}

		float blurDist = Cygnus::RandomGenerator::GetInstance()->RandomValue(-blurAmount, blurAmount);

		// Y成分以外のランダムベクトルを加算
		direction.x += blurDist;
		direction.z += blurDist;
		direction = Cygnus::Float3::Normalize(direction); // 再正規化

		// 弾の生成・初期化
		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(objectPlayer_->GetTranslate(), direction, &Cygnus::ModelManager::GetInstance()->GetModel("Bullet"));
		BulletManager::GetInstance()->AddBullet(std::move(newBullet));
		ResultStats::GetInstance()->AddShot(); // 弾を撃ったことを記録

		// パーティクル発生
		Cygnus::ParticleEffectManager::GetInstance()->Emit("shellEjection", objectGun_->transform_.translate_, kShellEjectionCount, { 0.0f, 0.0f, 0.0f }, objectGun_->transform_.rotate_.y); // 薬莢排出

		Cygnus::Float3 forward = { sinf(objectGun_->transform_.rotate_.y), 0.0f, cosf(objectGun_->transform_.rotate_.y) }; // 前方向ベクトル
		Cygnus::ParticleEffectManager::GetInstance()->Emit("muzzleFlash", objectGun_->transform_.translate_ + (forward * kMuzzleFlashForwardOffset), kMuzzleFlashCount); // マズルフラッシュ
	}
}

void Player::HandleOverHeat()
{
	// 左クリック取得
	isFiring_ = Cygnus::Input::GetInstance()->IsPressMouse(0);

	// 発射タイマーを進める
	fireTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();

	// オーバーヒートしていない場合の処理
	if (!isOverheated_ && isFiring_) {
		// オーバーヒート処理
		overheatTime_ += kOverheatGainPerSecond * Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		if (overheatTime_ >= kOverheatLimit) {
			overheatTime_ = kOverheatLimit;
			isOverheated_ = true;
		}

		// 射撃処理
		if (fireTimer_ >= kFireCooldown) {
			HandleShooting();
			fireTimer_ = 0.0f;
		}

		// オーバーヒート中の処理
	} else {
		// 冷却処理（撃っていない間 or オーバーヒート中）
		overheatTime_ -= kOverheatRecoverySpeed * Cygnus::TimeManager::GetInstance()->GetDeltaTime();
		overheatTime_ = std::max(overheatTime_, 0.0f);

		// 冷却時間完了でオーバーヒート解除
		if (isOverheated_ && overheatTime_ <= 0.0f) {
			isOverheated_ = false;
		}
	}
}

void Player::HandleHitBlink()
{
	// 発光演出中でなければスキップ
	if (!isHitBlink_) return;

	hitBlinkTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float t;

	switch (hitBlinkPhase_)
	{
	case HitBlinkPhase::BlinkIn:
		if (hitBlinkTimer_ < kHitBlinkDuration) {
			t = std::clamp(hitBlinkTimer_ / kHitBlinkDuration, 0.0f, 1.0f);
			// プレイヤーを発光させる
			objectPlayer_->object_->materialCB_.data_->emissiveIntensity = Cygnus::Easing::EaseOutQuad(t);
		} else {
			// 終了したら次のフェーズへ
			hitBlinkPhase_ = HitBlinkPhase::BlinkOut;
			hitBlinkTimer_ = 0.0f;
		}
		break;
	case HitBlinkPhase::BlinkOut:
		if (hitBlinkTimer_ < kHitBlinkDuration) {
			t = std::clamp(hitBlinkTimer_ / kHitBlinkDuration, 0.0f, 1.0f);
			// プレイヤーを減光させる
			objectPlayer_->object_->materialCB_.data_->emissiveIntensity = 1.0f - Cygnus::Easing::EaseInQuad(t);
		} else {
			// 終了したら待機フェーズへ
			hitBlinkPhase_ = HitBlinkPhase::Wait;
			hitBlinkTimer_ = 0.0f;
			objectPlayer_->object_->materialCB_.data_->emissiveIntensity = 0.0f;
			isHitBlink_ = false;
		}
		break;
	}
}

void Player::HandleDamageEffect()
{
	// ダメージ演出中でなければスキップ
	if (!isReceiveDamage_) return;

	damageEffectTimer_ += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float t;

	switch (damageEffectPhase_)
	{
	case DamageEffectPhase::In:
		if (damageEffectTimer_ < kDamageEffectDurationIn) {
			t = std::clamp(damageEffectTimer_ / kDamageEffectDurationIn, 0.0f, 1.0f);
			// ダメージビネットの強度を増加
			postEffectManager_->damageVignetteCB_.data_->intensity = Cygnus::Easing::EaseInQuad(t);
		} else {
			// 終了したら維持フェーズへ
			damageEffectPhase_ = DamageEffectPhase::Hold;
			damageEffectTimer_ = 0.0f;
		}
		break;
	case DamageEffectPhase::Hold:
		// 最大強度で維持しておく
		postEffectManager_->damageVignetteCB_.data_->intensity = 1.0f;

		// 維持時間に達したら減少フェーズへ
		if (damageEffectTimer_ > kDamageEffectDurationHold) {
			damageEffectPhase_ = DamageEffectPhase::Out;
			damageEffectTimer_ = 0.0f;
		}

	case DamageEffectPhase::Out:
		if (damageEffectTimer_ < kDamageEffectDurationOut) {
			t = std::clamp(damageEffectTimer_ / kDamageEffectDurationOut, 0.0f, 1.0f);
			// ダメージビネットの強度を減少
			postEffectManager_->damageVignetteCB_.data_->intensity = 1.0f - Cygnus::Easing::EaseInQuad(t);
		} else {
			// 終了したら待機フェーズへ
			damageEffectPhase_ = DamageEffectPhase::Wait;
			damageEffectTimer_ = 0.0f;
			isReceiveDamage_ = false;
		}
		break;
	}
}
