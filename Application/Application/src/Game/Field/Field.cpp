#include "Field.h"

// C++
#include <numbers>

void Field::Initialize() {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	objectFloor_ = std::make_unique<Cygnus::Object3D>();
	objectFloor_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("Plane");
	objectFloor_->transform_.rotate_ = {-std::numbers::pi_v<float> / 2.0f, 0.0f, 0.0f}; // 上を向ける
	objectFloor_->transform_.scale_ = kScale;
	objectFloor_->materialCB_.data_->color = kColor;
}

void Field::Update() { objectFloor_->UpdateMatrix(); }

void Field::Draw() { objectFloor_->Draw(); }
