#include "Player.h"

// Engine
#include <Camera.h>

// externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void Player::Initialize() {
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
	objectPlayer_->transform_.translate = { 0.0f, 1.0f, 0.0f };

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
	///	スプライト関連
	/// 

	/* レティクル */

	// ターゲットスプライト初期化
	uint32_t textureTarget = TextureManager::Load("resources/Images/game/target.png", dxBase->GetDevice());
	spriteTarget_ = std::make_unique<Sprite>();
	spriteTarget_->Initialize(spriteCommon_.get(), textureTarget);
	spriteTarget_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteTarget_->SetSize({ 100.0f, 100.0f });

	/* HPバー */

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({ 1.0f, 0.2f, 0.2f, 1.0f });

	///
	///	パラメーター設定
	/// 
	
	currentHP_ = 100;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定しておく
}

// ---------------------------------------------------------
// 毎フレーム更新処理
// ---------------------------------------------------------
void Player::Update() {
	///
	///	内部処理
	/// 

	// 移動処理
	HandleMove();
	// 弾の発射処理
	HandleShooting();
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
	///	スプライト更新処理
	/// 

	/* レティクル */

	// ターゲットスプライト更新
	spriteTarget_->Update();

	/* HPバー */

	// HPバー（後景）
	spriteHPBackground_->Update();
	// HPバー（前景）
	spriteHPForeground_->Update();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void Player::Draw() {

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
void Player::DrawUI()
{
	/*--------------*/
	/* レティクル関連 */
	/*--------------*/

	///
	///	レティクル描画
	/// 

	// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定（あとで整理）
	Float3 screenPos = Float3::Transform(CalclateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	spriteTarget_->SetPosition({ screenX, screenY });

	// ターゲットスプライト描画
	spriteTarget_->Draw();

	/*--------------*/
	/*   HPバー関連   */
	/*--------------*/

	///
	/// HPバー（後景）描画
	/// 
	
	spriteHPBackground_->SetPosition({
		Window::GetWidth() / 2.0f - kHPBarSize.x / 2.0f, // 画面の中央 - サイズ半分で中央揃え
		(Window::GetHeight() / 8.0f) * 7.0f // 画面縦サイズの 7/8 の位置へ設定
		});
	spriteHPBackground_->Draw();

	///
	/// HPバー（前景）描画
	/// 

	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_); // HP割合

	Float2 hpBarForegroundSize = { kHPBarSize.x * hpRatio, kHPBarSize.y };
	spriteHPForeground_->SetSize(hpBarForegroundSize); // 現在HPに応じてサイズ変更

	spriteHPForeground_->SetPosition({
		Window::GetWidth() / 2.0f - kHPBarSize.x / 2.0f, // 画面の中央 - サイズ半分で中央揃え
		(Window::GetHeight() / 8.0f) * 7.0f // 画面縦サイズの 7/8 の位置へ設定
		});
	spriteHPForeground_->Draw();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void Player::OnCollision(Collider* other)
{
	if (other->GetTag() == "NormalEnemy") {
		objectPlayer_->transform_.translate.x += 1.0f;
	}
}

// ---------------------------------------------------------
// デバッグ表示
// ---------------------------------------------------------
void Player::Debug() {
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

	/*  */

	ImGui::End();
#endif //  _DEBUG
}

// ---------------------------------------------------------
// カーソル位置のワールド座標を取得
// ---------------------------------------------------------
Float3 Player::CalclateCursorPosition()
{
	// マウス位置の取得
	Float2 mousePos = Float2(input_->GetMousePosition().x, input_->GetMousePosition().y);

	// 画面サイズ取得
	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	// スクリーン座標 -> 正規化デバイス座標（NDC）へ変換
	float ndcX = (2.0f * mousePos.x / screenWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * mousePos.y / screenHeight);

	// NDC -> ワールド空間へ変換
	Float4 clipNear = Float4(ndcX, ndcY, 0.0f, 1.0f);
	Float4 clipFar = Float4(ndcX, ndcY, 1.0f, 1.0f);

	Matrix matVPInv = Matrix::Inverse(Camera::GetCurrent()->GetViewProjectionMatrix());

	// ワールド座標に変換
	Float4 worldNear = Float4::Transform(clipNear, matVPInv);
	Float4 worldFar = Float4::Transform(clipFar, matVPInv);

	// w除算で正しい座標に変換
	worldNear /= worldNear.w;
	worldFar /= worldFar.w;

	// レイの始点と方向
	Float3 rayOrigin = { worldNear.x, worldNear.y, worldNear.z };
	Float3 rayDir = Float3::Normalize({
		worldFar.x - worldNear.x,
		worldFar.y - worldNear.y,
		worldFar.z - worldNear.z
		});

	if (std::abs(rayDir.y) > 0.0001f) {
		float t = -rayOrigin.y / rayDir.y;

		Float3 hitPos = {
			rayOrigin.x + rayDir.x * t,
			1.0f,
			rayOrigin.z + rayDir.z * t
		};

		return hitPos;
	}

	// 計算できない場合には無効値を返す
	return Float3(0.0f, 0.0f, 0.0f);
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
	// 左クリックで弾を生成
	if (input_->IsTriggerMouse(0)) {
		// カーソル位置の取得
		Float3 cursorPos = CalclateCursorPosition();
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
