#include "Waypoint.h"

Waypoint::Waypoint(const std::string& name, const Float3& pos, ModelManager::ModelData* model) {
	// 名前と初期位置を受け取る
	name_ = name;
	position_ = pos;

	// 球オブジェクト生成
	objectSphere_ = std::make_unique<Object3D>();
	objectSphere_->transform_.translate_ = pos;
	objectSphere_->transform_.scale_ = kInitialScale;
	objectSphere_->materialCB_.data_->color = kColorYellow;
	objectSphere_->model_ = model;
}

void Waypoint::Update() { objectSphere_->UpdateMatrix(); }

void Waypoint::Draw() {
	if (isSelected_) {
		objectSphere_->materialCB_.data_->color = kColorLightBlue; // 水色
	} else {
		objectSphere_->materialCB_.data_->color = kColorYellow; // 黄色
	}

	objectSphere_->Draw();
}