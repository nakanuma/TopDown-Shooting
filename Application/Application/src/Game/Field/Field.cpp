#include "Field.h"

// C++
#include <numbers>

// externals
#include <ImguiWrapper.h>

void Field::Initialize()
{
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// 床オブジェクト生成
	modelFloor_ = ModelManager::LoadModelFile("resources/Models", "plane.obj", dxBase->GetDevice());
	modelFloor_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	objectFloor_ = std::make_unique<Object3D>();
	objectFloor_->model_ = &modelFloor_;
	objectFloor_->transform_.rotate = { -std::numbers::pi_v <float> / 2.0f, 0.0f, 0.0f };
	objectFloor_->transform_.scale = { 500.0f, 500.0f, 1.0f };
	objectFloor_->materialCB_.data_->color = { 0.5f, 0.5f, 0.5f, 1.0f };
}

void Field::Update()
{
	objectFloor_->UpdateMatrix();
}

void Field::Draw()
{
	objectFloor_->Draw();

#ifdef _DEBUG
	Debug();
#endif // _DEBUG
}

void Field::Debug() {
#ifdef _DEBUG
	ImGui::Begin("Field");

	/* Translate */
	ImGui::Text("Translate");

	ImGui::DragFloat3("translate", &objectFloor_->transform_.translate.x, 0.01f);

	ImGui::DragFloat3("rotate", &objectFloor_->transform_.rotate.x, 0.01f);

	ImGui::DragFloat3("scale", &objectFloor_->transform_.scale.x, 0.01f);

	/*  */

	ImGui::End();
#endif
}
