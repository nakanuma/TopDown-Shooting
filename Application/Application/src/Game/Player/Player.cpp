#include "Player.h"

// Engine
#include <Camera.h>
#include <Engine/Util/TimeManager.h>

// Application
#include <src/Game/Utility/Utility.h>

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
	///	オブジェクト関連
	/// 

	// プレイヤーモデル読み込み
	modelPlayer_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelPlayer_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	// プレイヤーオブジェクト生成
	objectPlayer_ = std::make_unique<Object3D>();
	objectPlayer_->model_ = &modelPlayer_;
	objectPlayer_->transform_.translate = data.translate;
	objectPlayer_->materialCB_.data_->color = {0.0f, 0.5f, 1.0f, 1.0f};

	// 弾モデル読み込み
	modelBullet_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	modelBullet_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	///
	///	コライダー生成
	/// 

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("Player");
	collider_->SetOwner(this);

	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	UI
	/// 

	ui_ = std::make_unique<PlayerUIManager>();
	ui_->Initialize();

	///
	///	パラメーター設定
	/// 
	
	currentHP_ = kMaxHP; // 現在HPには最大HPをセット
	currentAmmo_ = kMaxAmmo; // マガジンには最大弾数をセット
}

// ---------------------------------------------------------
// 毎フレーム更新処理
// ---------------------------------------------------------
void Player::Update() 
{
	///
	///	内部処理
	/// 

	// 移動処理
	HandleMove();
	// 弾の発射処理
	HandleShooting();
	// 弾のリロード処理
	HandleReloading();
	// 弾の更新処理
	UpdateBullets();

	///
	///	コライダー更新処理
	/// 

	UpdateCollider();

	///
	///	オブジェクト更新処理
	/// 

	// プレイヤーオブジェクト更新
	objectPlayer_->UpdateMatrix();

	///
	///	UI更新処理
	/// 
	
	ui_->Update(this);
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void Player::Draw() 
{

	// 全ての弾を描画
	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}

	// プレイヤーオブジェクト描画
	objectPlayer_->Draw();

#ifdef _DEBUG
	// デバッグ表示
	Debug();
#endif //  _DEBUG
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
void Player::OnCollision(Collider* other)
{
	// vs NormalEnemy
	if (other->GetTag() == "NormalEnemy") {
		currentHP_--;
	}

	// vs NormalObstacle
	if (other->GetTag() == "NormalObstacle") {
		AABBCollider* myAABB = dynamic_cast<AABBCollider*>(collider_.get());
		AABBCollider* otherAABB = dynamic_cast<AABBCollider*>(other);

		// 押し戻し処理
		if (myAABB && otherAABB) {
			// 押し戻しベクトル取得
			Float3 pushVec = myAABB->GetPushBackVector(*otherAABB);
			// プレイヤー位置を補正
			objectPlayer_->transform_.translate += pushVec;

			// コライダーも更新しておく
			myAABB->min_ += pushVec;
			myAABB->max_ += pushVec;
		}
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void Player::Debug() 
{
#ifdef  _DEBUG
	ImGui::Begin("Player");

	/* Translate */
	ImGui::Text("Translate");

	ImGui::DragFloat3("translate", &objectPlayer_->transform_.translate.x, 0.01f);

	ImGui::DragFloat3("rotate", &objectPlayer_->transform_.rotate.x, 0.01f);

	ImGui::DragFloat3("scale", &objectPlayer_->transform_.scale.x, 0.01f);

	/* Parameter */
	ImGui::Text("Parameter");

	ImGui::BeginDisabled(true); // 操作不可
	ImGui::DragFloat3("Velocity", &velocity_.x, 0.01f);
	ImGui::EndDisabled();

	ImGui::DragFloat("Speed", &speed_, 0.01f);

	ImGui::DragInt("HP", &currentHP_);

	ImGui::Text("Ammo : %d", currentAmmo_);

	ImGui::Text("reloadTimer : %f", reloadTimer_);

	/*  */

	ImGui::End();
#endif //  _DEBUG
}

// ---------------------------------------------------------
// 移動処理
// ---------------------------------------------------------
void Player::HandleMove()
{
	velocity_ = { 0.0f, 0.0f, 0.0f };

	if (input_->PushKey(DIK_W)) velocity_.z += 1.0f;
	if (input_->PushKey(DIK_S)) velocity_.z -= 1.0f;
	if (input_->PushKey(DIK_A)) velocity_.x -= 1.0f;
	if (input_->PushKey(DIK_D)) velocity_.x += 1.0f;

	if (velocity_.x != 0.0f || velocity_.z != 0.0f) {
		velocity_ = Float3::Normalize(velocity_);
	}

	objectPlayer_->transform_.translate += velocity_ * speed_;
}

// ---------------------------------------------------------
// 弾の発射処理
// ---------------------------------------------------------
void Player::HandleShooting()
{
	///
	///	弾が撃てない場合には早期リターン
	/// 

	// リロード中は撃てないように
	if (isReloading_) return;
	// 弾数が0なら撃てないように
	if (currentAmmo_ <= 0) return;

	///
	///	左クリックで弾の生成
	/// 

	// 左クリックで弾を生成
	if (input_->IsTriggerMouse(0)) {
		// カーソル位置の取得
		Float3 cursorPos = Utility::CalclateCursorPosition();
		// プレイヤー位置の取得
		Float3 playerPos = objectPlayer_->transform_.translate;

		// 発射方向
		Float3 direction = cursorPos - playerPos;
		direction.y = 0.0f;
		direction = Float3::Normalize(direction);

		// 弾の生成・初期化
		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(objectPlayer_->transform_.translate, direction, &modelBullet_);

		bullets_.push_back(std::move(newBullet));

		// 残弾を減らす
		currentAmmo_--;
	}
}

// ---------------------------------------------------------
// 弾のリロード処理
// ---------------------------------------------------------
void Player::HandleReloading() 
{ 
	///
	///	リロード中更新処理
	/// 
	if (isReloading_) {
		// 必要リロード時間まで加算
		reloadTimer_ += TimeManager::GetInstance()->GetDeltaTime();
		if (reloadTimer_ >= kMaxReloadTime) {
			currentAmmo_ = kMaxAmmo; // マガジンに最大弾数をセット
			isReloading_ = false; // リロード状態解除
		}
	///
	///	Rキー押下でリロード開始
	/// 
	} else {
		if (input_->TriggerKey(DIK_R)) {
			isReloading_ = true; // リロード中にする
			reloadTimer_ = 0.0f; // タイマー初期化
		}
	}
}

// ---------------------------------------------------------
// 弾の更新処理
// ---------------------------------------------------------
void Player::UpdateBullets()
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
	bullets_.erase(
		std::remove_if(bullets_.begin(), bullets_.end(),
			[](const std::unique_ptr<Bullet>& bullet) {
				return bullet->IsDead();
			}),
		bullets_.end()
	);
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void Player::UpdateCollider()
{
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = objectPlayer_->transform_.translate;
		Float3 size = objectPlayer_->transform_.scale;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}
