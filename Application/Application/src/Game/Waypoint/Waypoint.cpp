#include "Waypoint.h"

Waypoint::Waypoint(const std::string& name, const Float3& pos, ModelManager::ModelData* model) {
	name_ = name;
	position_ = pos;

	objectSphere_ = std::make_unique<Object3D>();
	objectSphere_->transform_.translate = pos;
	objectSphere_->transform_.scale = {0.25f, 0.25f, 0.25f};
	objectSphere_->materialCB_.data_->color = {1.0f, 1.0f, 0.0f, 1.0f};
	objectSphere_->model_ = model;
}

void Waypoint::Update() { objectSphere_->UpdateMatrix(); }

void Waypoint::Draw() {
	if (isSelected_) {
		objectSphere_->materialCB_.data_->color = {0.0f, 1.0f, 1.0f, 1.0f}; // 水色
	} else {
		objectSphere_->materialCB_.data_->color = {1.0f, 1.0f, 0.0f, 1.0f}; // 黄色
	}

	objectSphere_->Draw();
}