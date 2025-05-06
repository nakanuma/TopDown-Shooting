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
	objectPlayer_->transform_.translate = {0.0f, 1.0f, 0.0f};


	// 弾モデル読み込み
	modelBullet_ = ModelManager::LoadModelFile("resources/Models", "sphere.obj", dxBase->GetDevice());
	modelBullet_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	///
	///	スプライト関連
	/// 
	
	// ターゲットスプライト初期化
	uint32_t textureTarget = TextureManager::Load("resources/Images/game/target.png", dxBase->GetDevice());
	spriteTarget_ = std::make_unique<Sprite>();
	spriteTarget_->Initialize(spriteCommon_.get(), textureTarget);
	spriteTarget_->SetAnchorPoint({ 0.5f, 0.5f });
	spriteTarget_->SetSize({ 100.0f, 100.0f });
}

// ---------------------------------------------------------
// 毎フレーム更新処理
// ---------------------------------------------------------
void Player::Update() { 
	// 移動処理
	HandleMove();
	// 弾の発射処理
	HandleShooting();
	// 弾の更新処理
	UpdateBullets();

	// プレイヤーオブジェクト更新
	objectPlayer_->UpdateMatrix();
	// ターゲットスプライト更新
	spriteTarget_->Update();
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
	// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定（あとで整理）
	Float3 screenPos = Float3::Transform(CalclateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	spriteTarget_->SetPosition({ screenX, screenY });

	// ターゲットスプライト描画
	spriteTarget_->Draw();
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