#include "EmissiveObject.h"

// Engine
#include <ImguiWrapper.h>
#include <LightManager.h>

void EmissiveObject::Initialize(){
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// Rectangle
	objectRectangleLight_ = std::make_unique<Object3D>();
	objectRectangleLight_->model_ = &ModelManager::GetInstance()->GetModel("Cube");
	objectRectangleLight_->transform_.translate_ = {28.0f, 1.0f, -40.0f};
	objectRectangleLight_->transform_.scale_ = {5.0f, 0.5f, 0.1f};
	objectRectangleLight_->SetEmissiveAsAreaLight(Float3{1.0f, 0.0f, 1.0f}, 2.0f, 15.0f, LightManager::AreaLightType::RectAngle);
	
	// Disk
	objectDiskLight_ = std::make_unique<Object3D>();
	objectDiskLight_->model_ = &ModelManager::GetInstance()->GetModel("Sphere");
	objectDiskLight_->transform_.translate_ = {32.0f, 1.0f, -40.0f};
	objectDiskLight_->transform_.scale_ = {2.0f, 2.0f, 0.2f};
	objectDiskLight_->SetEmissiveAsAreaLight(Float3{0.0f, 1.0f, 0.0f}, 2.5f, 12.0f, LightManager::AreaLightType::Disk);

	// Tube
	objectTubeLight_ = std::make_unique<Object3D>();
	objectTubeLight_->model_ = &ModelManager::GetInstance()->GetModel("Cube");
	objectTubeLight_->transform_.translate_ = {36.0f, 2.0f, -40.0f};
	objectTubeLight_->transform_.scale_ = {3.0f, 0.2f, 0.2f};
	objectTubeLight_->SetEmissiveAsAreaLight(Float3{0.0f, 0.0f, 1.0f}, 2.0f, 10.0f, LightManager::AreaLightType::Tube);

	// Sphere
	objectSphereLight_ = std::make_unique<Object3D>();
	objectSphereLight_->model_ = &ModelManager::GetInstance()->GetModel("Sphere");
	objectSphereLight_->transform_.translate_ = {40.0f, 1.0f, -40.0f};
	objectSphereLight_->transform_.scale_ = {1.5f, 1.5f, 1.5f};
	objectSphereLight_->SetEmissiveAsAreaLight(Float3{1.0f, 1.0f, 0.0f}, 2.0f, 10.0f, LightManager::AreaLightType::Sphere);
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

	if (objectSphereLight_) {
		objectSphereLight_->UpdateMatrix();
		objectSphereLight_->UpdateEmissiveAreaLight();
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

	if (objectSphereLight_) {
		objectSphereLight_->Draw();
	}
}

void EmissiveObject::Debug(){
#ifdef USE_IMGUI
	ImGui::Begin("Rectangle");

	ImGui::DragFloat3("translate", &objectRectangleLight_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectRectangleLight_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &objectRectangleLight_->transform_.scale_.x, 0.01f);

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


	ImGui::Begin("Sphere");

	ImGui::DragFloat3("translate", &objectSphereLight_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectSphereLight_->transform_.rotate_.x, 0.01f);
	ImGui::DragFloat3("scale", &objectSphereLight_->transform_.scale_.x, 0.01f);

	ImGui::Separator();

	ImGui::DragFloat3("color", &objectSphereLight_->materialCB_.data_->emissiveColor.x, 0.01f);
	ImGui::DragFloat("intensity", &objectSphereLight_->materialCB_.data_->emissiveIntensity, 0.01f);

	ImGui::End();
#endif
}