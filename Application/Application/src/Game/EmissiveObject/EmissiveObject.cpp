#include "EmissiveObject.h"

// Engine
#include <ImguiWrapper.h>

void EmissiveObject::Initialize(){
	DirectXBase* dxBase = DirectXBase::GetInstance();

	object_ = std::make_unique<Object3D>();
	object_->model_ = &ModelManager::GetInstance()->GetModel("Cube");
	object_->transform_.translate_ = {36.0f, 1.0f, 0.0f};
	object_->transform_.scale_ = {4.0f, 1.0f, 1.0f};
	object_->SetEmissive(kEmissiveColor, kEmissiveIntensity, 10.0f, 2.0f);
}

void EmissiveObject::Update(){
	object_->UpdateMatrix();
	object_->UpdateEmissiveLight();
}

void EmissiveObject::Draw(){
	object_->Draw();
}

void EmissiveObject::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("EmissiveObject");

	ImGui::DragFloat3("translate", &object_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &object_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &object_->transform_.scale_.x, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat3("emissiveColor", &object_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("emissiveIntensity", &object_->materialCB_.data_->emissiveIntensity, 0.01f);

	ImGui::End();
#endif
}