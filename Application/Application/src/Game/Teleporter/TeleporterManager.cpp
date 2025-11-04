#include "TeleporterManager.h"

// Engine
#include <ImguiWrapper.h>

void TeleporterManager::Initialize(const std::vector<Loader::TransformData>& datas) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	///
	///	テレポーターの生成
	///

	for (const auto& data : datas) {
		if (data.tag == "TELEPORTER") {
			auto teleporter = std::make_unique<Teleporter>();
			teleporter->Initialize(data.translate, &ModelManager::GetInstance()->GetModel("Teleporter"));
			// ペアIDを設定
			teleporter->SetPairID(data.pairID);

			teleporters_.emplace_back(std::move(teleporter));
		}
	}

	// ペアのテレポーターを設定
	for (auto& p1 : teleporters_) {
		for (auto& p2 : teleporters_) {
			if (p1.get() == p2.get())
				continue; // 同一テレポーターは弾く

			// 同じペアIDを持つテレポーターをリンクさせる
			if (p1->GetPairID() == p2->GetPairID()) {
				p1->SetPair(p2.get());
				p2->SetPair(p1.get());
			}
		}
	}
}

void TeleporterManager::Update() {
	// 全てのテレポーターを更新
	for (auto& teleporter : teleporters_) {
		teleporter->Update();
	}
}

void TeleporterManager::Draw() {
	// 全てのテレポーターを描画
	for (auto& teleporter : teleporters_) {
		teleporter->Draw();
	}
}

void TeleporterManager::Debug() {
#ifdef _DEBUG
	ImGui::Begin("TeleporterManager");

	for (size_t i = 0; i < teleporters_.size(); ++i) {
		Teleporter* teleporter = teleporters_[i].get();
		if (!teleporter)
			continue;

		std::string label = "Teleporter[" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {
			// 位置の表示
			const Float3& translate = teleporter->GetTranslate();
			ImGui::Text("Translate : (%.2f, %.2f, %.2f)", translate.x, translate.y, translate.z);

			// IDの表示
			const std::string& id = teleporter->GetPairID();
			ImGui::Text("pairID : %s", id.c_str());

			ImGui::TreePop();
		}
	}

	ImGui::End();
#endif
}