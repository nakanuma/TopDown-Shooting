#include "Player.h"

// Engine
#include <Camera.h>

// externals
#include <ImguiWrapper.h>

void Player::Initialize() { 
	DirectXBase* dxBase = DirectXBase::GetInstance(); 
	input_ = Input::GetInstance();

	modelPlayer_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice());
	modelPlayer_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	objectPlayer_ = std::make_unique<Object3D>();
	objectPlayer_->model_ = &modelPlayer_;
	objectPlayer_->transform_.translate = {0.0f, 1.0f, 0.0f};
}

void Player::Update() { 
	objectPlayer_->UpdateMatrix(); 

	///
	///	移動処理
	/// 
	
	velocity_ = {0.0f, 0.0f, 0.0f};

	if (input_->PushKey(DIK_W)) velocity_.z += 1.0f;
	if (input_->PushKey(DIK_S)) velocity_.z -= 1.0f;
	if (input_->PushKey(DIK_A)) velocity_.x -= 1.0f;
	if (input_->PushKey(DIK_D)) velocity_.x += 1.0f;

	objectPlayer_->transform_.translate += velocity_ * speed_;

	///
	///	弾の発射処理
	/// 
	
	// 左クリックで弾を生成
	if (input_->IsTriggerMouse(0)) {
		Float3 cursorDirection = CaluclateCursorDirection();
		cursorDirection.y = 0.0f; // Y方向には移動させないように

		auto newBullet = std::make_unique<PlayerBullet>();
		newBullet->Initialize(objectPlayer_->transform_.translate, cursorDirection);

		bullets_.push_back(std::move(newBullet));
	}

	// 全ての弾を更新
	for (auto& bullet : bullets_) {
		bullet->Update();
	}

	// 画面外の弾を削除
	// あとで記述
}

void Player::Draw() {
	objectPlayer_->Draw();

	// 全ての弾を描画
	for (const auto& bullet : bullets_) {
		bullet->Draw();
	}

#ifdef _DEBUG
	Debug();
#endif //  _DEBUG
}

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

Float3 Player::CaluclateCursorDirection() { 
	// マウス座標の取得
	Float2 mousePos = {
		static_cast<float>(input_->GetMousePosition().x), 
		static_cast<float>(input_->GetMousePosition().y)
	};

	// スクリーンサイズ
	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	// NDC座標系 (-1 ～ 1)
	Float2 ndc;
	ndc.x = (mousePos.x / screenWidth) * 2.0f - 1.0f;
	ndc.y = 1.0f - (mousePos.y / screenHeight) * 2.0f; // 上下反転

	// ビュープロジェクション逆行列
	Matrix invViewProj = Matrix::Inverse(Camera::GetCurrent()->GetViewProjectionMatrix());

	// NDCからワールド座標への変換（Z=0.0と1.0の２点を取得して方向ベクトルを得る）
	Float4 nearPoint = {ndc.x, ndc.y, 0.0f, 1.0f};
	Float4 farPoint = {ndc.x, ndc.y, 1.0f, 1.0f};

	nearPoint = nearPoint * invViewProj;
	farPoint = farPoint * invViewProj;

	// 射影除去
	nearPoint /= nearPoint.w;
	farPoint /= farPoint.w;

	// 方向ベクトル
	Float3 direction = Float3::Normalize(Float3{farPoint.x - nearPoint.x, farPoint.y - nearPoint.y, farPoint.z - nearPoint.z});

	return direction;
}
