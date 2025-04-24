#include "PlayerBullet.h"

void PlayerBullet::Initialize(const Float3& position, const Float3& direciton) { 
	DirectXBase* dxBase = DirectXBase::GetInstance();

	modelBullet_ = ModelManager::LoadModelFile("resources/Models", "cube.obj", dxBase->GetDevice()); 
	modelBullet_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = &modelBullet_;
	objectBullet_->transform_.translate = position;

	// パラメーター設定
	speed_ = 1.0f;
	velocity_ = direciton * speed_;
}

void PlayerBullet::Update() { 
	objectBullet_->UpdateMatrix();

	// 移動処理
	objectBullet_->transform_.translate += velocity_; 
}

void PlayerBullet::Draw() { 
	objectBullet_->Draw();
}