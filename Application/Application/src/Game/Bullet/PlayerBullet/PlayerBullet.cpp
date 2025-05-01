#include "PlayerBullet.h"

void PlayerBullet::Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	objectBullet_ = std::make_unique<Object3D>();
	objectBullet_->model_ = model;
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