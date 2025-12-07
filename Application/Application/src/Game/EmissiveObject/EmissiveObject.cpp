#include "EmissiveObject.h"

// Engine
#include <ImguiWrapper.h>
#include <LightManager.h>

void EmissiveObject::Initialize(){
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// Rectangle
	objectRectangleLight_ = std::make_unique<Cygnus::Object3D>();
	objectRectangleLight_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("Cube");
	objectRectangleLight_->transform_.translate_ = {17.720f, 4.0f, 15.0f};
	objectRectangleLight_->transform_.scale_ = {3.0f, 2.0f, 0.1f};
	objectRectangleLight_->transform_.rotate_ = {0.0f, -0.99f, 0.0f};
	objectRectangleLight_->SetEmissiveAsAreaLight(Cygnus::Float3{1.0f, 1.0f, 1.0f}, 2.0f, 15.0f, Cygnus::LightManager::AreaLightType::RectAngle);
	
	// Disk
	objectDiskLight_ = std::make_unique<Cygnus::Object3D>();
	objectDiskLight_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("Sphere");
	objectDiskLight_->transform_.translate_ = {55.240f, 9.7f, 25.820f};
	objectDiskLight_->transform_.scale_ = {2.0f, 2.0f, 0.2f};
	objectDiskLight_->transform_.rotate_ = {-0.380f, 1.010f, 0.0f};
	objectDiskLight_->SetEmissiveAsAreaLight(Cygnus::Float3{1.0f, 1.0f, 1.0f}, 2.5f, 12.0f, Cygnus::LightManager::AreaLightType::Disk);

	// Tube
	objectTubeLight_ = std::make_unique<Cygnus::Object3D>();
	objectTubeLight_->model_ = &Cygnus::ModelManager::GetInstance()->GetModel("Cube");
	objectTubeLight_->transform_.translate_ = {40.220f, 6.1f, 47.0f};
	objectTubeLight_->transform_.scale_ = {4.0f, 0.2f, 0.2f};
	objectTubeLight_->SetEmissiveAsAreaLight(Cygnus::Float3{1.0f, 1.0f, 1.0f}, 2.0f, 10.0f, Cygnus::LightManager::AreaLightType::Tube);
}

void EmissiveObject::Update(){
	if (objectRectangleLight_) {
		objectRectangleLight_->UpdateMatrix();
		objectRectangleLight_->UpdateEmissiveAreaLight();
	}

	if (objectDiskLight_) {
		objectDiskLight_->UpdateMatrix();
		objectDiskLight_->UpdateEmissiveAreaLight();
	}

	if (objectTubeLight_) {
		objectTubeLight_->UpdateMatrix();
		objectTubeLight_->UpdateEmissiveAreaLight();
	}
}

void EmissiveObject::Draw(){
	if (objectRectangleLight_) {
		objectRectangleLight_->Draw();
	}

	if (objectDiskLight_) {
		objectDiskLight_->Draw();
	}

	if (objectTubeLight_) {
		objectTubeLight_->Draw();
	}
}

void EmissiveObject::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("Rectangle");

	ImGui::DragFloat3("translate", &objectRectangleLight_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectRectangleLight_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &objectRectangleLight_->transform_.scale_.x, 0.01f);
	ImGui::ColorEdit4("color", &objectRectangleLight_->materialCB_.data_->color.x);

	ImGui::Separator();

	ImGui::DragFloat3("color", &objectRectangleLight_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("intensity", &objectRectangleLight_->materialCB_.data_->emissiveIntensity, 0.01f);

	ImGui::End();


	ImGui::Begin("Disk");

	ImGui::DragFloat3("translate", &objectDiskLight_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectDiskLight_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &objectDiskLight_->transform_.scale_.x, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat3("color", &objectDiskLight_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("intensity", &objectDiskLight_->materialCB_.data_->emissiveIntensity, 0.01f);

	ImGui::End();


	ImGui::Begin("Tube");

	ImGui::DragFloat3("translate", &objectTubeLight_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectTubeLight_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &objectTubeLight_->transform_.scale_.x, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat3("color", &objectTubeLight_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("intensity", &objectTubeLight_->materialCB_.data_->emissiveIntensity, 0.01f);

	ImGui::End();
#endif
}